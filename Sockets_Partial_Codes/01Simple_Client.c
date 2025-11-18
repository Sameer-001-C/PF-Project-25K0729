#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "192.168.0.113"  // change if server is on another PC
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    while (1) {
        printf("Enter message (or 'exit' to quit): ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0'; // remove newline

        if (strcmp(message, "exit") == 0)
            break;

        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            printf("Socket creation failed.\n");
            continue;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.s_addr = inet_addr(SERVER_IP);

        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
            printf("Connection failed.\n");
            closesocket(sock);
            continue;
        }

        // Send message
        send(sock, message, strlen(message), 0);

        // Optional: receive reply
        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Server replied: %s\n", buffer);
        }

        closesocket(sock);
    }

    WSACleanup();
    return 0;
}
