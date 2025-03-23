#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// User database
char *users_table[][2] = {
    {"farida", "farida@!"},
    {"user", "p@ssword2"}
};

// Function to authenticate user
int authentication(char username[], char password[]) {
    int total_users = sizeof(users_table) / sizeof(users_table[0]);  
    for (int i = 0; i < total_users; i++) {
        if (strcmp(username, users_table[i][0]) == 0 && strcmp(password, users_table[i][1]) == 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char username[50], password[50];
    int bytes_received;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept a client connection
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Receive and respond to client message
    read(new_socket, buffer, BUFFER_SIZE);
    printf("Client: %s\n", buffer);
    send(new_socket, "Hello from server", strlen("Hello from server"), 0);

    // Clear buffer and receive credentials
    memset(buffer, 0, BUFFER_SIZE);
    bytes_received = read(new_socket, buffer, BUFFER_SIZE);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  
        printf("Credentials received: %s\n", buffer);
        sscanf(buffer, "%s %s", username, password);
        // Extract username and password
        if (authentication(username, password)) {
        send(new_socket, "Authentication successful", strlen("Authentication successful"), 0);
        
        // Wait for client message after successful authentication
        memset(buffer, 0, BUFFER_SIZE);
        read(new_socket, buffer, BUFFER_SIZE);
        printf("Client message: %s\n", buffer);
        send(new_socket, "Message received", strlen("Message received"), 0);
    } else {
        send(new_socket, "Authentication failed", strlen("Authentication failed"), 0);
    }
    }
    // Close sockets
    close(new_socket);
    close(server_fd);

    return 0;
}