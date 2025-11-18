//we will be assighning different ports to different clients
//port must be assigned during login, and stored in the server's database
//However, the code up to now does not necessarily implement this perfectly to fully, which would require more stuff to be added
//More stuff will be added sooner or later

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

#define SERVER_PORT 8080
#define BUFFER_SIZE 512

int main() {
    WSADATA wsa;
    SOCKET listen_sock, server_sock;
    struct sockaddr_in server_addr, listen_addr;
    int assigned_port;
    char username[50];

    WSAStartup(MAKEWORD(2,2), &wsa);

    // ===== LOGIN =====
    printf("Enter username: ");
    scanf("%s", username);

    // Connect to server to login
    server_sock = socket(AF_INET, SOCK_STREAM,0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    char login_msg[100];
    sprintf(login_msg,"LOGIN:%s", username);
    send(server_sock, login_msg, strlen(login_msg),0);

    char port_reply[10];
    recv(server_sock, port_reply, sizeof(port_reply)-1,0);
    assigned_port = atoi(port_reply);
    printf("Assigned port: %d\n", assigned_port);
    closesocket(server_sock);

    // ===== START LISTENING =====
    listen_sock = socket(AF_INET, SOCK_STREAM,0);
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(assigned_port);
    listen_addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_sock, (struct sockaddr*)&listen_addr, sizeof(listen_addr));
    listen(listen_sock,5);

    printf("Listening for incoming messages on port %d...\n", assigned_port);

    while(1){
        // Non-blocking user input
        printf("Enter recipient: ");
        char recipient[50], msg[256];
        scanf("%s", recipient);
        getchar(); // consume newline
        printf("Enter message: ");
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg,"\n")] = 0;

        // Connect to server to send message
        server_sock = socket(AF_INET, SOCK_STREAM,0);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        connect(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
        char send_msg[BUFFER_SIZE];
        sprintf(send_msg,"MESSAGE:%s:%s", recipient, msg);
        send(server_sock, send_msg, strlen(send_msg),0);
        closesocket(server_sock);

        // Check for incoming messages
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listen_sock,&readfds);
        struct timeval tv = {0,100000}; // 0.1 sec
        int activity = select(0,&readfds,NULL,NULL,&tv);
        if(activity>0 && FD_ISSET(listen_sock,&readfds)){
            SOCKET incoming;
            struct sockaddr_in from_addr;
            int addr_len = sizeof(from_addr);
            incoming = accept(listen_sock,(struct sockaddr*)&from_addr,&addr_len);
            char buffer[BUFFER_SIZE];
            int bytes = recv(incoming, buffer, sizeof(buffer)-1,0);
            buffer[bytes]='\0';
            printf("\nMessage received: %s\n", buffer);
            closesocket(incoming);
        }
    }

    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
