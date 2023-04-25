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

int read_message(int s1) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int read_size;

    // split message into fields
    read_size = recv(s1, buffer, sizeof(buffer), 0);
    if (read_size == 0) {
        return 1;
    }
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
        token = strtok(NULL, "|");
    }
    printf("buffer: %s\n", buffer);
    printf("cpy: %s\n", cpy);


    // check that message has at least two fields
    if (num_fields < 2) {
        char* m = "INVL|15|Invalid Format|";
        send(s1, m, strlen(m), 0);
        // go back to client loop
        return 1;
    }

    int num_bars = 0;
    for (int i = 0; i < strlen(cpy); i++) {
        if (cpy[i] == '|') {
            num_bars++;
        }
    }

    if (num_bars != num_fields) {
        // fields not equal return 1
        return 1;
    }

    char* code = fields[0];
    int length = atoi(fields[1]);
    printf("length of size field: %d\n", strlen(fields[1]));
    printf("code: %s\n", code);
    printf("length as a number: %d\n", length);
    printf("REMAINING MESSAGE:\n");
    for (int i = 0; i < length; i++) {
        printf("%c", cpy[6 + strlen(fields[1])+ i]);
    }
    printf("\n");
    printf("cpy size %d\n", strlen(cpy));
    printf("%d\n", (5 + strlen(fields[1]) + length));
    
    
    // check how many bytes were read 
    
    
    int diff = read_size - (5 + strlen(fields[1]) + length);
    if (diff == 1) {
        // required number of bytes were read
        printf("Correct number of bytes read\n");
    }
    else if (diff > 1) {
        // not enough bytes were read
        // TODO: fix this
        printf("Not enough bytes read\n");
    }
    else if (diff < 1) {
        // read too many bytes: weird
        // TODO: fix this
        printf("Too many bytes read\n");
    }
    if (cpy[5 + strlen(fields[1]) + length] == '|') {
        printf("true mf - last char was a vertical bar!\n");
    }
    else {
        char* m = "INVL|21|Incorrect Formatting|";
        send(s1, m, strlen(m), 0);
        printf("false mf\n");
        // go back to client loop
        return 1;
    }
   
   // error checking done
    if (strcmp(code, "BEGN") == 0) {
        printf("WAHOO WE HAVE A BEGN!\n");
        
        // only one begn message should be sent. Check if we are to play, or not.

        // if 3rd field is an X
        if (strcmp(fields[2], "X")) {
            char  message[BUFFER_SIZE];
            memset(message, 0, BUFFER_SIZE);
            // we play first
            // enter move
            printf("Enter text: ");
            fgets(message, BUFFER_SIZE, stdin);
            // message[strcspn(message, "\n")] = '\0'; // Remove newline character

            // Send the message to the server
            if (send(s1, message, strlen(message), 0) == -1) {
                perror("Failed to send message to server");
                exit(EXIT_FAILURE);
            }

        }
        return 0;
    }


}

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

    // Continuously read from and send text to the server
    
        // Read input from the user
        printf("Enter text: ");
        fgets(message, BUFFER_SIZE, stdin);
        // message[strcspn(message, "\n")] = '\0'; // Remove newline character

        // Send the message to the server
        if (send(socketfd, message, strlen(message), 0) == -1) {
            perror("Failed to send message to server");
            exit(EXIT_FAILURE);
        }
        while(1) {
        // Read the response from the server
        if (read_message(socketfd) == 0) {
            break;
        }
        // read_size = read(socketfd, buffer, BUFFER_SIZE);
        // if (read_size == -1) {
        //     perror("Failed to read from server");
        //     exit(EXIT_FAILURE);
        // } else if (read_size == 0) {
        //     printf("Server closed the connection\n");
        //     break;
        // } else {
        //     printf("Server response: %.*s\n", read_size, buffer);
        // }

    }

    // Close the socket
    close(socketfd);
    return 0;
}