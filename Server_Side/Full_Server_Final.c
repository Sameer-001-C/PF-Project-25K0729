#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <direct.h>
#include <io.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8888
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 10

struct Client_Info {
    char UserID[8]; 
    char IP_Address[16];
};

struct Client_Info Clients[MAX_CLIENTS];
int Online_Count = 0;

void Setup_Directories() {
    _mkdir("Server_Data");
    _mkdir("Server_Data/Users");
    _mkdir("Server_Data/Messages");
    
    FILE *fp = fopen("Server_Data/Users/User_Details.txt", "a");
    if (fp) fclose(fp);
}

char Get_Code(char *buf) { return buf[0]; }

void Get_ID(char *buf, char *out) {
    strncpy(out, buf + 1, 7);
    out[7] = '\0';
}

void Get_ID_2(char *buf, char *out) {
    strncpy(out, buf + 8, 7);
    out[7] = '\0';
}

int Check_Login_Credentials(char *buffer, int len) {
    char input_creds[32];
    strncpy(input_creds, buffer + 1, len - 1);
    input_creds[len - 1] = '\0';

    FILE *fp = fopen("Server_Data/Users/User_Details.txt", "r");
    if (!fp) return 2;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, input_creds) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 2;
}

int Create_Account(char *buffer, int len) {
    char user_id[8];
    Get_ID(buffer, user_id);

    FILE *fp = fopen("Server_Data/Users/User_Details.txt", "r");
    char line[256];
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, user_id, 7) == 0) {
                fclose(fp);
                return 8;
            }
        }
        fclose(fp);
    }

    fp = fopen("Server_Data/Users/User_Details.txt", "a");
    char input_creds[32];
    strncpy(input_creds, buffer + 1, len - 1);
    input_creds[len - 1] = '\0';
    
    fprintf(fp, "%s\n", input_creds);
    fclose(fp);
    return 1;
}

int Check_User_Exists(char *target_id) {
    FILE *fp = fopen("Server_Data/Users/User_Details.txt", "r");
    if (!fp) return 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, target_id, 7) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void Save_Message_To_Inbox(char *sender, char *receiver, char *msg) {
    char path[64];
    sprintf(path, "Server_Data/Messages/%s.txt", receiver);
    
    FILE *fp = fopen(path, "a");
    if (fp) {
        fprintf(fp, "[%s]: %s\n", sender, msg);
        fclose(fp);
    } else {
        printf("Error: Could not open inbox for %s.\n", receiver);
    }
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    Setup_Directories();

    WSAStartup(MAKEWORD(2,2), &wsa);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Is Port %d busy?\n", PORT);
        return 1;
    }

    listen(server_socket, 5);
    printf("Server listening on Port %d...\n", PORT);

    while (1) {
        client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket == INVALID_SOCKET) continue;

        char *client_ip = inet_ntoa(client_addr.sin_addr);
        printf("Request received from IP: %s | ", client_ip);

        int bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            char code = buffer[0];
            printf("Processing Code: %c\n", code);

            memset(response, 0, BUFFER_SIZE);

            if (code == '1') {
                int status = Check_Login_Credentials(buffer, bytes);
                if (status == 1) {
                    char uid[8];
                    Get_ID(buffer, uid);
                    int already_in = 0;
                    for(int i=0; i<Online_Count; i++) {
                        if(strcmp(Clients[i].UserID, uid) == 0) already_in = 1;
                    }

                    if (already_in) {
                        sprintf(response, "3");
                    } else if (Online_Count >= MAX_CLIENTS) {
                        sprintf(response, "3");
                    } else {
                        sprintf(response, "1");
                        strcpy(Clients[Online_Count].UserID, uid);
                        strcpy(Clients[Online_Count].IP_Address, client_ip);
                        Online_Count++;
                        printf("User %s logged in. Total Online: %d\n", uid, Online_Count);
                    }
                } else {
                    sprintf(response, "2");
                }
            }
            else if (code == '2') {
                int status = Create_Account(buffer, bytes);
                if (status == 1) {
                    sprintf(response, "1");
                    char uid[8];
                    Get_ID(buffer, uid);
                    strcpy(Clients[Online_Count].UserID, uid);
                    strcpy(Clients[Online_Count].IP_Address, client_ip);
                    Online_Count++;
                } else {
                    sprintf(response, "8");
                }
            }
            else if (code == '3') {
                char target[8];
                Get_ID(buffer, target); 
                if (Check_User_Exists(target)) sprintf(response, "4");
                else sprintf(response, "5");
            }
            else if (code == '4') {
                char sender[8], receiver[8], msg[BUFFER_SIZE];
                Get_ID(buffer, sender);
                Get_ID_2(buffer, receiver);
                
                if (bytes > 15) strcpy(msg, buffer + 15);
                else strcpy(msg, "");

                Save_Message_To_Inbox(sender, receiver, msg);
                sprintf(response, "9");
            }
            else if (code == '5') {
                char uid[8];
                Get_ID(buffer, uid);
                int idx = -1;
                for(int i=0; i<Online_Count; i++) {
                    if (strcmp(Clients[i].UserID, uid) == 0) idx = i;
                }

                if (idx != -1) {
                    Clients[idx] = Clients[Online_Count - 1];
                    Online_Count--;
                    printf("User %s logged out. Count: %d\n", uid, Online_Count);
                }
            }
            else if (code == '6') {
                char uid[8];
                Get_ID(buffer, uid);
                
                char path[64];
                sprintf(path, "Server_Data/Messages/%s.txt", uid);
                
                FILE *fp = fopen(path, "r");
                if (fp) {
                    response[0] = '1'; 
                    char line[256];
                    while(fgets(line, sizeof(line), fp)) {
                        if (strlen(response) + strlen(line) < BUFFER_SIZE - 1) {
                            strcat(response, line);
                        }
                    }
                    fclose(fp);
                    remove(path);
                } else {
                    sprintf(response, "0");
                }
            }

            send(client_socket, response, strlen(response), 0);
        }
        
        closesocket(client_socket);
    }

    WSACleanup();
    return 0;
}
