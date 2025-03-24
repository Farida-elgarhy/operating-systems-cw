// client.c
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

int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};
    char username[23], password[23];
    int bytes;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY; // Connect to localhost
    

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Step 1: Receive "Enter username:"
    memset(buffer, 0, BUFFER_SIZE);
    bytes = read(sock, buffer, BUFFER_SIZE);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    } else{
        perror("Failed to receive username prompt");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Step 2: Send username
    fgets(username, sizeof(username), stdin);
    remove_newline(username);
    if(send(sock, username, strlen(username), 0)<0){
        perror("Failed to send username");
        close(sock);
        exit(EXIT_FAILURE);   
    }

    // Step 3: Receive "Enter password:"
    //memset(buffer, 0, BUFFER_SIZE);
    bytes = read(sock, buffer, BUFFER_SIZE);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }else{
        perror("Failed to receive password prompt");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Step 4: Send password
    fgets(password, sizeof(password), stdin);
    remove_newline(password);
    if(send(sock, password, strlen(password), 0)<0){
        perror("Failed to send password");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Step 5: Receive authentication response
    //memset(buffer, 0, BUFFER_SIZE);
    bytes = read(sock, buffer, BUFFER_SIZE);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("Server: %s\n", buffer);

        if (strcmp(buffer, "Authentication successful") == 0) {
            // Step 6: Send message to server
            printf("Enter message to server: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            remove_newline(buffer);
            send(sock, buffer, strlen(buffer), 0);

            // Step 7: Receive acknowledgment
            //memset(buffer, 0, BUFFER_SIZE);
            bytes = read(sock, buffer, BUFFER_SIZE);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                printf("Server: %s\n", buffer);
            }
        }
    } 

    // Close socket
    close(sock);
    return 0;
}