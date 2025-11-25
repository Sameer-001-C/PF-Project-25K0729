#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "192.168.0.113"
#define PORT 8888
#define BUFFER_SIZE 4096
#define USERID_LEN 7
#define PASS_MAX_LEN 16

char current_user_id[USERID_LEN + 1] = {0}; 

int send_request_and_get_response(const char *request, char *response_buffer);
void handle_login();
void handle_create_account();
void handle_add_friend();
void handle_send_message();
void handle_check_inbox();
void handle_logout();
void format_id(const char *input, char *output);
void print_menu();

void format_id(const char *input, char *output) {
    strncpy(output, input, USERID_LEN);
    output[USERID_LEN] = '\0';
}

int send_request_and_get_response(const char *request, char *response_buffer) {
    SOCKET sock;
    struct sockaddr_in server;
    int client_code = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) return 0;

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("\n[ERROR] Connection failed. Is the server on or IP incorrect?\n");
        closesocket(sock);
        return 0;
    }

    if (send(sock, request, strlen(request), 0) < 0) {
        printf("\n[ERROR] Send failed.\n");
        closesocket(sock);
        return 0;
    }

    memset(response_buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(sock, response_buffer, BUFFER_SIZE - 1, 0);
    
    if (bytes_received > 0) {
        response_buffer[bytes_received] = '\0';
        client_code = response_buffer[0] - '0';
    }

    closesocket(sock);
    return client_code;
}

void handle_login() {
    char user_id[USERID_LEN + 1], password[PASS_MAX_LEN + 1];
    char request[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE];

    printf("\n--- Login ---\n");
    printf("Enter 7-character UserID: ");
    scanf("%7s", user_id);
    printf("Enter Password: ");
    scanf("%16s", password);

    sprintf(request, "1%s%s", user_id, password);

    int code = send_request_and_get_response(request, response);

    if (code == 1) {
        printf("\n[SUCCESS] Login successful. Welcome back, %s!\n", user_id);
        strcpy(current_user_id, user_id);
    } else if (code == 2) {
        printf("\n[FAILURE] Wrong credentials (ID or Password incorrect).\n");
    } else if (code == 3) {
        printf("\n[FAILURE] Server is full or user is already online.\n");
    } else {
        printf("\n[ERROR] Unknown Code received.\n");
    }
}

void handle_create_account() {
    char user_id[USERID_LEN + 1], password[PASS_MAX_LEN + 1];
    char request[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE];

    printf("\n--- Create Account ---\n");
    printf("Enter NEW 7-character UserID: ");
    scanf("%7s", user_id);
    printf("Enter Password: ");
    scanf("%16s", password);

    if(strlen(user_id) != 7) {
        printf("\n[FAILURE] UserID must be exactly 7 characters.\n");
        return;
    }

    sprintf(request, "2%s%s", user_id, password);

    int code = send_request_and_get_response(request, response);

    if (code == 1) {
        printf("\n[SUCCESS] Account created! Logged in automatically.\n");
        strcpy(current_user_id, user_id);
    } else if (code == 8) {
        printf("\n[FAILURE] This UserID already exists. Try something else.\n");
    } else {
        printf("\n[FAILURE] Could not create account.\n");
    }
}

void handle_add_friend() {
    char friend_id[USERID_LEN + 1];
    char request[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE];

    printf("\n--- Add Friend ---\n");
    printf("Enter Friend's UserID: ");
    scanf("%7s", friend_id);

    sprintf(request, "3%s", friend_id);

    int code = send_request_and_get_response(request, response);

    if (code == 4) printf("\n[SUCCESS] User '%s' found! You can add them to friend list.\n", friend_id);
    else printf("\n[FAILURE] User '%s' not found in server database.\n", friend_id);
}

void handle_send_message() {
    char receiver_id[USERID_LEN + 1];
    char message_text[BUFFER_SIZE];
    char request[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE];

    printf("\n--- Send Message ---\n");
    printf("To (UserID): ");
    scanf("%7s", receiver_id);
    
    while (getchar() != '\n');

    printf("Message: ");
    fgets(message_text, sizeof(message_text), stdin);
    message_text[strcspn(message_text, "\n")] = 0;

    request[0] = '4';
    format_id(current_user_id, &request[1]);
    format_id(receiver_id, &request[8]);
    strcpy(&request[15], message_text);

    int code = send_request_and_get_response(request, response);

    if (code == 9) printf("\n[SUCCESS] Message sent to Inbox.\n");
    else printf("\n[FAILURE] Could not send message.\n");
}

void handle_check_inbox() {
    char request[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE];

    printf("\n--- Checking Inbox ---\n");

    sprintf(request, "6%s", current_user_id);

    int code = send_request_and_get_response(request, response);

    if (strlen(response) > 1) {
        printf("--------------------------------\n");
        printf("%s", response + 1);
        printf("--------------------------------\n");
    } else {
        printf("[INFO] No new messages.\n");
    }
}

void handle_logout() {
    char request[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE];

    sprintf(request, "5%s", current_user_id);
    
    send_request_and_get_response(request, response);
    
    printf("\n[SUCCESS] Logged out successfully.\n");
    memset(current_user_id, 0, sizeof(current_user_id));
}

void print_menu() {
    printf("\n===================================\n");
    if (current_user_id[0] != 0) {
        printf("  LOGGED IN AS: %s\n", current_user_id);
        printf("===================================\n");
        printf("1. Add Friend (Search)\n");
        printf("2. Send Message\n");
        printf("3. Check Inbox (Refresh Messages)\n");
        printf("4. Logout\n");
        printf("5. Exit Client\n");
    } else {
        printf("  (NOT LOGGED IN)\n");
        printf("===================================\n");
        printf("1. Login\n");
        printf("2. Create New Account\n");
        printf("3. Exit Client\n");
    }
    printf("Choice: ");
}

int main() {
    WSADATA wsa;
    int choice;
    
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            continue;
        }

        if (current_user_id[0] != 0) {
            switch(choice) {
                case 1: handle_add_friend(); break;
                case 2: handle_send_message(); break;
                case 3: handle_check_inbox(); break;
                case 4: handle_logout(); break;
                case 5: WSACleanup(); return 0;
                default: printf("Invalid choice.\n");
            }
        } else {
            switch(choice) {
                case 1: handle_login(); break;
                case 2: handle_create_account(); break;
                case 3: WSACleanup(); return 0;
                default: printf("Invalid choice.\n");
            }
        }
    }
    return 0;
}
