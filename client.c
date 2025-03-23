#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    char username[50], password[50], credentials[BUFFER_SIZE];
    int bytes_received;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket creation failed.\n");
        return 1;
    }

    // Set server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Connection failed.\n");
        return 1;
    }

    // Send greeting
    send(sock, "Hello from client", strlen("Hello from client"), 0);

    // Receive greeting from server
    bytes_received = read(sock, buffer, BUFFER_SIZE);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Server: %s\n", buffer);
    }

    // Get username and password
    printf("Enter username: ");
    scanf("%s", username);

    printf("Enter password: ");
    scanf("%s", password);

    // Combine username and password
    snprintf(credentials, sizeof(credentials), "%s %s", username, password);
    send(sock, credentials, strlen(credentials), 0);

    // Receive authentication result
    memset(buffer, 0, BUFFER_SIZE);
    bytes_received = read(sock, buffer, BUFFER_SIZE);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Server: %s\n", buffer);

        // Send message if authenticated
        if (strcmp(buffer, "Authentication successful") == 0) {
            char message[BUFFER_SIZE];
            printf("Enter a one worded message to server: ");
            scanf("%s", message);

            send(sock, message, strlen(message), 0);

            // Receive acknowledgment
            memset(buffer, 0, BUFFER_SIZE);
            read(sock, buffer, BUFFER_SIZE);
            printf("Server: %s\n", buffer);
        }
    }

    // Close socket
    close(sock);
    return 0;
}