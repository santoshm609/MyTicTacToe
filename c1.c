#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 3300
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int socketfd, read_size;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE], message[BUFFER_SIZE];

    // Create the socket
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert the server IP address from string to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid server IP address");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(socketfd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }

    // Read input from the user
        printf("Enter text: ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0'; // Remove newline character

        // Send the message to the server
        if (send(socketfd, message, strlen(message), 0) == -1) {
            perror("Failed to send message to server");
            exit(EXIT_FAILURE);
        }

    // Continuously read from and send text to the server
    while (1) {

        // Read the response from the server
        read_size = read(socketfd, buffer, BUFFER_SIZE);
        if (read_size == -1) {
            perror("Failed to read from server");
            exit(EXIT_FAILURE);
        } else if (read_size == 0) {
            printf("Server closed the connection\n");
            break;
        } else {
            printf("Read Size: %d\n", read_size);
            printf("%s\n", buffer);
            char cpy[strlen(buffer)];
            strcpy(cpy, buffer);
            char* fields[7]; // array to hold fields
            int num_fields = 0;
            char* token = strtok(buffer, "|"); // split by vertical bar
            while (token != NULL) {
            
                fields[num_fields] = token;
                num_fields++;
                printf("field #%d\n", num_fields);
                token = strtok(NULL, "|");
            }
            printf("buffer: %s\n", buffer);
            printf("cpy: %s\n", cpy);
            printf("Server response: %.*s\n", read_size, buffer);
            if (strcmp(fields[0], "BEGN") == 0) {
                // Read input from the user
                printf("Enter text: ");
                fgets(message, BUFFER_SIZE, stdin);
                message[strcspn(message, "\n")] = '\0'; // Remove newline character

                // Send the message to the server
                if (send(socketfd, message, strlen(message), 0) == -1) {
                    perror("Failed to send message to server");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    // Close the socket
    close(socketfd);
    return 0;
}