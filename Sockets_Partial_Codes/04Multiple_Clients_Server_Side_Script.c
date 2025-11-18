//we will be assighning different ports to different clients
//port must be assigned during login, and stored in the server's database
//However, the code up to now does not necessarily implement this perfectly to fully, which would require more stuff to be added
//More stuff will be added sooner or later

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 512

typedef struct {
    char username[50];
    int port;
    char ip[50];
} User;

User users[MAX_CLIENTS];
int user_count = 0;

int find_user_index(const char* username) {
    for(int i=0;i<user_count;i++) {
        if(strcmp(users[i].username, username) == 0) return i;
    }
    return -1;
}

int main() {
    WSADATA wsa;
    SOCKET server_sock;
    struct sockaddr_in server_addr;

    printf("Starting server...\n");
    if(WSAStartup(MAKEWORD(2,2), &wsa)!=0){
        printf("WSAStartup failed.\n");
        return 1;
    }

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == INVALID_SOCKET){ printf("Socket failed.\n"); return 1; }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr))==SOCKET_ERROR){
        printf("Bind failed.\n"); return 1;
    }

    listen(server_sock, 5);
    printf("Server listening on port %d\n", SERVER_PORT);

    while(1){
        SOCKET client_sock;
        struct sockaddr_in client_addr;
        int addr_len = sizeof(client_addr);
        char buffer[BUFFER_SIZE];

        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        if(client_sock == INVALID_SOCKET){ printf("Accept failed.\n"); continue; }

        memset(buffer, 0, BUFFER_SIZE);
        recv(client_sock, buffer, BUFFER_SIZE-1, 0);

        // Expect either LOGIN:<username> or MESSAGE:<recipient>:<msg>
        if(strncmp(buffer, "LOGIN:",6)==0){
            char username[50];
            sscanf(buffer,"LOGIN:%s", username);

            // Assign port (just incrementing starting 5001)
            int assigned_port = 5001 + user_count;
            strcpy(users[user_count].username, username);
            users[user_count].port = assigned_port;
            sprintf(users[user_count].ip,"%s", inet_ntoa(client_addr.sin_addr));
            user_count++;

            char reply[100];
            sprintf(reply, "%d", assigned_port);
            send(client_sock, reply, strlen(reply),0);
            printf("User %s logged in, assigned port %d\n", username, assigned_port);
        }
        else if(strncmp(buffer,"MESSAGE:",8)==0){
            char recipient[50], msg[BUFFER_SIZE];
            sscanf(buffer,"MESSAGE:%[^:]:%[^\n]", recipient, msg);

            int idx = find_user_index(recipient);
            if(idx!=-1){
                // Forward message
                SOCKET forward_sock;
                struct sockaddr_in forward_addr;
                forward_sock = socket(AF_INET, SOCK_STREAM,0);

                forward_addr.sin_family = AF_INET;
                forward_addr.sin_port = htons(users[idx].port);
                forward_addr.sin_addr.s_addr = inet_addr(users[idx].ip);

                if(connect(forward_sock, (struct sockaddr*)&forward_addr, sizeof(forward_addr))==0){
                    send(forward_sock, msg, strlen(msg),0);
                    closesocket(forward_sock);
                    printf("Forwarded message to %s: %s\n", recipient, msg);
                } else {
                    printf("Could not connect to recipient %s\n", recipient);
                }
            } else {
                printf("Recipient %s not found.\n", recipient);
            }
        }

        closesocket(client_sock);
    }

    closesocket(server_sock);
    WSACleanup();
    return 0;
}
