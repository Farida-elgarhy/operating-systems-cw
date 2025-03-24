#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Helper to trim newline
void remove_newline(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}
// Array with usernames and passwords
const char *userstable[][2] = {
    {"farida", "farida@!"},
    {"user", "p@ssword2"},
    {"admin", "@dmin"}
};

// Authentication function
int authenticate(const char username[], const char password[]) {
    int total_users = sizeof(userstable) / sizeof(userstable[0]);  
    for (int i = 0; i < total_users; i++) {
        if (strcmp(username, userstable[i][0]) == 0 &&
            strcmp(password, userstable[i][1]) == 0) {
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
    char username[23], password[23];
    int bytes;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept a client connection
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Step 1: Send and receive username
    if(send(new_socket, "Enter username: ", strlen("Enter username: "), 0)<0){
        perror("Sending username failed");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    memset(username, 0, sizeof(username));
    bytes = read(new_socket, username, sizeof(username));
    if (bytes <= 0) {
        perror("Username read failed");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    remove_newline(username);
    printf("Username received: %s\n", username);

    // Step 2: Send and receive password
    if(send(new_socket, "Enter password: ", strlen("Enter password: "), 0)<0){
        perror("Sending password failed");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    memset(password, 0, sizeof(password));
    bytes = read(new_socket, password, sizeof(password));
    if (bytes <= 0) {
        perror("Password read failed");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    remove_newline(password);
    printf("Password received: %s\n", password);

    // Step 3: Authenticate
    if (authenticate(username, password)) {
        if(send(new_socket, "Authentication successful", strlen("Authentication successful"), 0)<0){
            perror("Send auth success failed");
            close(new_socket);
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        printf("authentication successful\n");

        // Receive message
        memset(buffer, 0, BUFFER_SIZE);
        bytes = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Client message: %s\n", buffer);
            if(send(new_socket, "Message received", strlen("Message received"), 0)<0){
                perror("Send response failed");
            }
        }
    } else {
        if(send(new_socket, "Authentication failed", strlen("Authentication failed"), 0)<0){
            perror("Send auth failed");
        }
        printf("authentication failed\n");
        
    }

    // Close sockets
    close(new_socket);
    close(server_fd);
    return 0;
}