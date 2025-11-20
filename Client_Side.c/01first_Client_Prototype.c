//saved in laptop as "Client_Simple_3.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "192.168.0.113" // Change this to your server's IP address
#define PORT 8080
#define BUFFER_SIZE 4096
#define USERID_LEN 7 // UserID must be exactly 7 characters long
#define PASS_MAX_LEN 16

// Globals to simulate client session state
char current_user_id[USERID_LEN + 1] = {0}; // Stores the 7-char UserID if logged in

// Function Prototypes
int send_request_and_get_response(const char *request, size_t request_len, char *response_buffer, size_t buffer_size);
void handle_login();
void handle_create_account();
void handle_add_friend();
void handle_send_message();
void handle_logout();
void format_id(const char *input, char *output);


/**
 * @brief Formats a user ID string. 
 *
 * This function enforces the 7-character length required by the server protocol.
 * For simplicity, it assumes the input is already 7 characters, otherwise, it
 * truncates or pads with nulls (which may fail server-side validation if not exactly 7).
 *
 * @param input The raw input string (expected to be 7 characters).
 * @param output The 8-byte buffer (7 chars + null terminator) to store the result.
 */
void format_id(const char *input, char *output) {
    // Copy up to USERID_LEN characters
    strncpy(output, input, USERID_LEN);
    // Ensure null termination at the 7th index
    output[USERID_LEN] = '\0';
}


/**
 * @brief Connects to the server, sends a single request, and receives a single-byte response.
 *
 * @param request The data to send (including the Server Code).
 * @param request_len The length of the request data.
 * @param response_buffer Buffer to store the server's single-byte response (Client Code).
 * @param buffer_size Size of the response buffer (should be at least 1).
 * @return int The Client Code received (as an integer 1-9), or 0 on failure.
 */
int send_request_and_get_response(const char *request, size_t request_len, char *response_buffer, size_t buffer_size) {
    SOCKET sock;
    struct sockaddr_in server;
    int client_code = 0;

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Error: Could not create socket.\n");
        return 0;
    }

    // 2. Prepare server address structure
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // 3. Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Error: Connection failed to %s:%d. Is the server running?\n", SERVER_IP, PORT);
        closesocket(sock);
        return 0;
    }

    // 4. Send data
    if (send(sock, request, request_len, 0) < 0) {
        printf("Error: Send failed.\n");
        closesocket(sock);
        return 0;
    }

    // 5. Receive a response (The server is expected to send a single-byte Client Code)
    int bytes_received = recv(sock, response_buffer, (int)buffer_size - 1, 0);
    if (bytes_received > 0) {
        response_buffer[bytes_received] = '\0';
        client_code = response_buffer[0] - '0'; // Convert char code ('1'-'9') to int (1-9)
    } else if (bytes_received == 0) {
        printf("Connection closed by server before receiving response.\n");
    } else {
        printf("Error: Receive failed.\n");
    }

    // 6. Close the connection (as per the server's design)
    closesocket(sock);
    return client_code;
}

/**
 * @brief Handles Server Code '1': Login to existing account.
 * Format: <Server_Code(1)><UserID(7)><Password(var)>
 */
void handle_login() {
    char user_id[USERID_LEN + 1];
    char password[PASS_MAX_LEN + 1];
    char request[USERID_LEN + PASS_MAX_LEN + 2] = {0}; // 1(code) + 7(id) + 16(pass) + 1(safety)

    printf("--- Login ---\n");
    printf("Enter 7-character UserID: ");
    scanf("%7s", user_id);
    printf("Enter Password: ");
    scanf("%16s", password);

    // 1. Construct the request message
    request[0] = '1';
    format_id(user_id, &request[1]);
    strcat(request, password);

    char response[BUFFER_SIZE];
    int code = send_request_and_get_response(request, strlen(request), response, sizeof(response));

    // 2. Handle Client Codes
    switch (code) {
        case 1:
            printf("\n[SUCCESS] Client Code 1: Login successful.\n");
            strcpy(current_user_id, user_id);
            break;
        case 2:
            printf("\n[FAILURE] Client Code 2: Wrong credentials (UserID or Password incorrect).\n");
            break;
        case 3:
            printf("\n[FAILURE] Client Code 3: Maximum online users reached. Cannot log in.\n");
            break;
        default:
            printf("\n[ERROR] Unknown Client Code received: %d\n", code);
    }
}


/**
 * @brief Handles Server Code '2': Create a new account.
 * Format: <Server_Code(2)><UserID(7)><Password(var)>
 */
void handle_create_account() {
    char user_id[USERID_LEN + 1];
    char password[PASS_MAX_LEN + 1];
    char request[USERID_LEN + PASS_MAX_LEN + 2] = {0};

    printf("--- Create Account ---\n");
    printf("Enter NEW 7-character UserID: ");
    scanf("%7s", user_id);
    printf("Enter Password (max 16 chars): ");
    scanf("%16s", password);

    // 1. Basic client-side check for ID length (server does its own validation too)
    if (strlen(user_id) != USERID_LEN) {
        printf("\n[FAILURE] UserID must be exactly %d characters long.\n", USERID_LEN);
        return;
    }

    // 2. Construct the request message
    request[0] = '2';
    format_id(user_id, &request[1]);
    strcat(request, password);

    char response[BUFFER_SIZE];
    int code = send_request_and_get_response(request, strlen(request), response, sizeof(response));

    // 3. Handle Client Codes
    switch (code) {
        case 1:
            printf("\n[SUCCESS] Client Code 1: Account created and logged in successfully.\n");
            strcpy(current_user_id, user_id);
            break;
        case 3:
            printf("\n[FAILURE] Client Code 3: Maximum online users reached. Cannot create account/log in.\n");
            break;
        case 8:
            printf("\n[FAILURE] Client Code 8: Username already exists. Please choose a different UserID.\n");
            break;
        default:
            printf("\n[ERROR] Unknown Client Code received: %d\n", code);
    }
}


/**
 * @brief Handles Server Code '3': Add a new friend.
 * Format: <Server_Code(3)><FriendID(7)>
 */
void handle_add_friend() {
    if (current_user_id[0] == 0) {
        printf("\n[ALERT] Please log in first.\n");
        return;
    }

    char friend_id[USERID_LEN + 1];
    char request[USERID_LEN + 2] = {0}; // 1(code) + 7(id) + 1(safety)

    printf("--- Add Friend ---\n");
    printf("Enter 7-character UserID of friend to add: ");
    scanf("%7s", friend_id);

    // 1. Construct the request message
    request[0] = '3';
    format_id(friend_id, &request[1]);

    char response[BUFFER_SIZE];
    int code = send_request_and_get_response(request, strlen(request), response, sizeof(response));

    // 2. Handle Client Codes
    switch (code) {
        case 4:
            printf("\n[SUCCESS] Client Code 4: User '%s' found in database and can be added as a friend.\n", friend_id);
            // NOTE: Actual friend list management would happen here in a real client
            break;
        case 5:
            printf("\n[FAILURE] Client Code 5: User '%s' not found in server database.\n", friend_id);
            break;
        default:
            printf("\n[ERROR] Unknown Client Code received: %d\n", code);
    }
}


/**
 * @brief Handles Server Code '4': Send a message.
 * Format: <Server_Code(4)><SenderID(7)><ReceiverID(7)><Message Text...>
 */
void handle_send_message() {
    if (current_user_id[0] == 0) {
        printf("\n[ALERT] Please log in first to send a message.\n");
        return;
    }

    char receiver_id[USERID_LEN + 1];
    char message_text[BUFFER_SIZE - 16]; // Max message size
    char request[BUFFER_SIZE] = {0};

    printf("--- Send Message ---\n");
    printf("To (7-character UserID): ");
    scanf("%7s", receiver_id);

    // Clear input buffer for potential longer message
    while (getchar() != '\n');
    printf("Message: ");
    fgets(message_text, sizeof(message_text), stdin);
    // Remove newline character added by fgets
    message_text[strcspn(message_text, "\n")] = 0; 


    // 1. Construct the request message
    // 1 byte Server Code '4'
    request[0] = '4';
    // 7 bytes Sender ID
    format_id(current_user_id, &request[1]);
    // 7 bytes Receiver ID
    format_id(receiver_id, &request[8]);
    // Message Text (starting at index 15)
    strcat(request, message_text);

    char response[BUFFER_SIZE];
    int code = send_request_and_get_response(request, strlen(request), response, sizeof(response));

    // 2. Handle Client Codes
    switch (code) {
        case 9:
            printf("\n[SUCCESS] Client Code 9: Message delivered to server for forwarding (Client B is online).\n");
            break;
        case 6:
            printf("\n[FAILURE] Client Code 6: Message failed to send. Receiver '%s' is offline.\n", receiver_id);
            break;
        default:
            printf("\n[ERROR] Unknown Client Code received: %d\n", code);
    }
}


/**
 * @brief Handles Server Code '5': Logout.
 * Format: <Server_Code(5)><UserID(7)>
 */
void handle_logout() {
    if (current_user_id[0] == 0) {
        printf("\n[ALERT] Not currently logged in.\n");
        return;
    }

    char request[USERID_LEN + 2] = {0}; // 1(code) + 7(id) + 1(safety)

    // 1. Construct the request message
    request[0] = '5';
    format_id(current_user_id, &request[1]);

    char response[BUFFER_SIZE];
    // The server just closes the socket after processing; it does not send a reply code back.
    // The client should not expect a code, only that the connection closed successfully.
    int code = send_request_and_get_response(request, strlen(request), response, sizeof(response));

    if (code == 0) {
        // If code is 0, it likely means the connection closed successfully without a reply.
        printf("\n[SUCCESS] Logged out successfully.\n");
        current_user_id[0] = 0; // Clear session state
    } else {
        printf("\n[ALERT] Logout request sent, but an unexpected code was received: %d. Clearing local session.\n", code);
        current_user_id[0] = 0;
    }
}


void print_menu() {
    printf("\n===================================\n");
    if (current_user_id[0] != 0) {
        printf("  Logged in as: %s\n", current_user_id);
    } else {
        printf("  (Not Logged In)\n");
    }
    printf("===================================\n");
    printf("1. Login (Server Code 1)\n");
    printf("2. Create New Account (Server Code 2)\n");
    printf("3. Add Friend (Server Code 3)\n");
    printf("4. Send Message (Server Code 4)\n");
    printf("5. Logout (Server Code 5)\n");
    printf("6. Exit Client\n");
    printf("Enter choice: ");
}


int main() {
    WSADATA wsa;
    int choice;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialized.\n");

    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            choice = 0; // Invalid input, handle below
            while (getchar() != '\n'); // Clear input buffer
        }

        switch (choice) {
            case 1:
                handle_login();
                break;
            case 2:
                handle_create_account();
                break;
            case 3:
                handle_add_friend();
                break;
            case 4:
                handle_send_message();
                break;
            case 5:
                handle_logout();
                break;
            case 6:
                if (current_user_id[0] != 0) {
                    handle_logout();
                }
                printf("Exiting application.\n");
                break;
            default:
                printf("\nInvalid choice. Please enter a number between 1 and 6.\n");
                break;
        }
    } while (choice != 6);

    WSACleanup();
    return 0;
}
