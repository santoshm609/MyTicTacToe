#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>


#define PORT1 3300
#define PORT2 3400
#define BUFFER_SIZE 1024

typedef struct {
    int client_sock;
    char *name;
} Player;

typedef struct node {
    Player player;
    struct node *next;
}Node;

Node* node_head = NULL;

int num_players = 0;

void print_ll() {
    // print out all names in list
    Node *ptr = node_head;
    printf("NAMES: \n");
    while (ptr != NULL) {

        printf("%s\n", ptr->player.name);
        ptr = ptr->next;
    }
    printf("NAMES PRINTED\n");
}

// void parse_message(char* buffer) {
//     char code[5], length_str[4];
//     int length, i, j;
    
//     char* fields[7]; // array to hold fields
//     int num_fields = 0;
//     char* token = strtok(buffer, "|"); // split by vertical bar
//     while (token != NULL) {
//         fields[num_fields] = token;
//         num_fields++;
//         token = strtok(NULL, "|");
//     }

//     // check that message has at least two fields
//     if (num_fields < 2) {
//         char message[256] = "INVL|15|Invalid Format|";
//         //send(s1, message, sizeof(message), 0);
//         return 0;
//     }

//     char* code = fields[0];
//     int length = atoi(fields[1]);
//     printf("%s, %d\n", code, length);

//     // Parse the code
//     strncpy(code, buffer, 4);
//     code[4] = '\0';

//     // Parse the length
//     strncpy(length_str, buffer + 5, 3);
//     length_str[3] = '\0';
//     length = atoi(length_str);

//     // Extract the remaining message
//     for (i = 0, j = 9; i < length; i++, j++) {
//         printf("%c", buffer[j]);
//     }
//     printf("\n");
//     printf("%s\n", code);
//     printf("%d\n", length);
// }


int read_message(int s1) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int read_size;

    // // Receive messages and parse them
    // while ((read_size = recv(s1, buffer, sizeof(buffer), 0)) > 0) {
    //     buffer[read_size] = '\0';
    //     parse_message(buffer);
    // }
    
    // split message into fields
    read_size = recv(s1, buffer, sizeof(buffer), 0);
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
    //printf("buffer: %s\n", buffer);
    //printf("cpy: %s\n", cpy);


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
    //printf("cpy size %d\n", strlen(cpy));
    //printf("%d\n", (5 + strlen(fields[1]) + length));
    
    
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
    if (strcmp(code, "PLAY") == 0) {
       //printf("WOOHOO WE GOT A PLAY\n");
        
        // player wants to play
        //printf("Name: %s\n", fields[2]);
        // check if name exists
        Node *n = node_head;
       // printf("Set n == node_head\n");
        while (n != NULL) {
            if (strcmp(n->player.name, fields[2]) == 0) {
                char* m = "INVL|22|Choose Different Name|";
                send(s1, m, strlen(m), 0);
                // go back to client loops
                return 1;
            }
            //printf("N != NULL\n");
            n = n->next;
        }
        //printf("N == NULL\n");

        // create player struct
        Player player1 = {0};
        player1.client_sock = s1;
        //printf("Start copy\n");
        player1.name = strdup(fields[2]);
        //printf("End copy\n");
        
        Node *new_node = (Node*) malloc(sizeof(Node));
        new_node->player = player1;
        new_node->next = NULL;

        // add player to linked list
        if (node_head == NULL) {
            printf("No Nodes. Added new node to beginning :)\n");
            node_head = new_node;
            //printf("No Nodes. Added new node to beginning :)\n");
        } else {
            printf("Nodes detected :)\n");
            Node *last_node = node_head;
            while (last_node->next != NULL) {
                printf("searching through nodes :)\n");
                last_node = last_node->next;
            }
            printf("On Last Node. Adding to list :)\n");
            last_node->next = new_node;
        }

        // send player wait message
        char * w = "WAIT|0|";
        // player is ready - increment num_players
        num_players++;
        send(s1, w, strlen(w), 0);
        printf("Player Added MF\n");

        print_ll();
        // return successfully
        return 0;
    }


}

void *handle_client(void *arg) {
    
    int socket1 = *((int *)arg);
    int socket2 = *((int *)arg+1);
    int player1_turn = 1;

    while (1) {
        // game is set up
        printf("NUMBER OF PLAYERS + %d\n\n", num_players);
        if (num_players % 2 == 0) {
            // iterate through each node - get players sock_fd. compare to socket1 / socket 2
            Node * curr = node_head;
            while (curr != NULL) {
                if (curr->player.client_sock == socket1) {
                    printf("Current Player is on Socket 1\n");
                    printf("NAME: %s\n", curr->player.name);
                    char buffer[1000];
                    memset(buffer, 0, BUFFER_SIZE);
                    sprintf(buffer, "BEGN|%d|X|%s|", (3 + strlen(curr->player.name)), curr->player.name);
                    printf("Server Message to be sent: %s\n", buffer);
                    send(socket1, buffer, sizeof(buffer), 0);
                }
                else if (curr->player.client_sock == socket2) {
                    printf("Current Player is on Socket 2\n");
                    printf("NAME: %s\n", curr->player.name);
                    char buffer[1000];
                    memset(buffer, 0, BUFFER_SIZE);
                    sprintf(buffer, "BEGN|%d|O|%s|", (3 + strlen(curr->player.name)), curr->player.name);
                    printf("Server Message to be sent: %s\n", buffer);
                    send(socket2, buffer, sizeof(buffer), 0);
                }
                curr = curr->next;
            }
        }
        if (player1_turn) {
            printf("PLAYER 1 TURN:\n\n");
            // Read from the first socket
            if (read_message(socket1) == 0) {
                // read message was successful and a move was made.
                player1_turn = !player1_turn;
            }
            // read unsuccessful - stay in client loop
        }
        else if (!player1_turn) {
            printf("PLAYER 2 TURN:\n\n");
            if (read_message(socket2) == 0) {
                // read message was successful and a move was made.
                player1_turn = !player1_turn;
            }
            // read unsuccessful - stay in client loop
        }
        
        
        //read_size = read(socket1, buffer, BUFFER_SIZE);
        // if (read_size <= 0) {
        //     // Either an error occurred or the client closed the connection
        //     break;
        // }
        // // Echo the message back to the client
        // write(socket2, buffer, read_size);

        // // Read from the second socket
        // read_size = read(socket2, buffer, BUFFER_SIZE);
        // if (read_size <= 0) {
        //     // Either an error occurred or the client closed the connection
        //     break;
        // }
        // // Echo the message back to the client
        // write(socket1, buffer, read_size);
/*
        if(counter == 0){
            // Wait for the second client to connect
            while (1) {
                if (read(socket1, buffer, BUFFER_SIZE) <= 0 || read(socket2, buffer, BUFFER_SIZE) <= 0) {
                    // Either an error occurred or the client closed the connection
                    break;
                }
                if (strcmp(buffer, "play") == 0) {
                    break;
                }
            }
            // Send a message to both clients indicating which player they are
            int player1size = strlen("You are player 1!");
            write(socket1, "You are player 1!", player1size);
            write(socket2, "You are player 2!", player1size);
            counter += 1;
        }
        else{
            // Read from the first socket
            read_size = read(socket1, buffer, BUFFER_SIZE);
            if (read_size <= 0) {
                // Either an error occurred or the client closed the connection
                break;
            }
            // Echo the message back to the client
            write(socket2, buffer, read_size);
        
            // Read from the second socket
            read_size = read(socket2, buffer, BUFFER_SIZE);
            if (read_size <= 0) {
                // Either an error occurred or the client closed the connection
                break;
            }
            // Echo the message back to the client
            write(socket1, buffer, read_size);
        }
*/
    }

    // Close both sockets
    close(socket1);
    close(socket2);
    free(arg);
    return NULL;
}
// void gameplay(*args) {
//     int socket1 = *((int *)arg);
//     int socket2 = *((int *)arg+1);
// }
int main(int argc, char *argv[]) {
    int socket1, socket2, client_socket1, client_socket2;
    struct sockaddr_in server1_address, server2_address, client_address1, client_address2;
    socklen_t client_address1_len, client_address2_len;
    pthread_t thread_id;
    int *arg;

    // Create the first socket
    socket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket1 == -1) {
        perror("Failed to create socket1");
        exit(EXIT_FAILURE);
    }

    // Set up the first server address
    memset(&server1_address, 0, sizeof(server1_address));
    server1_address.sin_family = AF_INET;
    server1_address.sin_addr.s_addr = INADDR_ANY;
    server1_address.sin_port = htons(PORT1);

    // Bind the first socket to the server address
    if (bind(socket1, (struct sockaddr *) &server1_address, sizeof(server1_address)) == -1) {
        perror("Failed to bind socket1");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections on the first socket
    if (listen(socket1, 1) == -1) {
        perror("Failed to listen on socket1");
        exit(EXIT_FAILURE);
    }

    // Create the second socket
    socket2 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket2 == -1) {
        perror("Failed to create socket2");
        exit(EXIT_FAILURE);
    }

    // Set up the second server address
    memset(&server2_address, 0, sizeof(server2_address));
    server2_address.sin_family = AF_INET;
    server2_address.sin_addr.s_addr = INADDR_ANY;
    server2_address.sin_port = htons(PORT2);

    // Bind the second socket to the server address
    if (bind(socket2, (struct sockaddr *) &server2_address, sizeof(server2_address)) == -1) {
        perror("Failed to bind socket2");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections on the second socket
    if (listen(socket2, 1) == -1) {
        perror("Failed to listen on socket2");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections on both sockets
    client_address1_len = sizeof(client_address1);
    client_address2_len = sizeof(client_address2);
    client_socket1 = accept(socket1, (struct sockaddr *) &client_address1, &client_address1_len);
    if (client_socket1 == -1) {
        perror("Failed to accept incoming connection on socket1");
        exit(EXIT_FAILURE);
    }

    client_socket2 = accept(socket2, (struct sockaddr *) &client_address2, &client_address2_len);
    if (client_socket2 == -1) {
        perror("Failed to accept incoming connection on socket2");
        exit(EXIT_FAILURE);
    }

    // Spawn a thread to handle the two clients
    pthread_t tid;
    int args[2] = {client_socket1, client_socket2};
    pthread_create(&tid, NULL, handle_client, (void *) args);

    // Wait for the thread to finish
    pthread_join(tid, NULL);

    // Close the sockets
    close(socket1);
    close(socket2);

    return 0;
}