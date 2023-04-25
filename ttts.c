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
    char *role;
} Player;

typedef struct node {
    Player player;
    struct node *next;
}Node;

Node* node_head = NULL;

int num_players = 0;
int game_started = 0;
int begn_sent = 0;
int game_over = 0;

// tic tac toe game logic
    char board[3][3] = 
    {
        {'.', '.', '.'},
        {'.', '.', '.'},
        {'.', '.', '.'}
    };

// string representation of board

    char string_board[10];
// x = 1, y = 1 -> middle square

// void make_string_board() {
    
//     for (int i = 0; i < 3; i ++) {
//         for (int j = 0 ; j < 3; j ++) {
//             buf[(i * 3) + j] = board[i][j];
//         }
//     }
// }

void helper() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%c ", board[i][j]);
        }
    }
    printf("\n");
}

void make_string_board() {
    for (int i = 0; i < 3; i ++) {
        for (int j = 0 ; j < 3; j ++) {
            string_board[(i * 3) + j] = board[i][j];
        }
    }
    string_board[9] = '\0';
}



int check_move(int x, int y) {

    // if board is not empty
    if (board[x][y] != '.') {
        // move is invalid
        return 1;
    }
    return 0;
}

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


int read_message(int s1, int s2) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int read_size;

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
        printf("field #%d, token: %s, token length: %d\n", num_fields,token, strlen(token));
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
        printf("%d\n", num_bars);
        printf("%d\n",num_fields);
        printf("FIELDS AND BARS NOT EQUAL\n");
        return 1;
    }
    printf("NUM BARS == NUM_FIELDS\n");

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

    printf("CODE: %s\n", code);
   
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

    if (strcmp(code, "MOVE") == 0) {
        
        // move called before game started
        if (game_started == 0) {
            char* m = "INVL|23|Incorrect Move Request|";
            send(s1, m, strlen(m), 0);
            // go back to client loops
            return 1;
        }
        
        // valid move request
        printf("WOOHOO WERE MOVING!\n");
        // check that the move is correct
        
        // get player

        Node * curr = node_head;
        while (curr != NULL) {
            if (curr->player.client_sock == s1) {
                // found current player
                printf("Found Current player %s\n", curr->player.name);
                // printf("String role: %s\n", curr->player.role);
                // printf("Char role: %c\n", curr->player.role[0]);
                // check if role matches what was inputted.
                
                // role is correct
                //if ((strlen(fields[2]) == 1) && ((strcmp(fields[2][0], curr->player.role) == 0) {
                    // printf("ARE WE IN IF SSTATEMENT\n");
                    // // properly formatted move
                    // if (strlen(fields[3]) != 3) {
                    //     printf("IMPROPERLY FORMATTED MOVE\n");
                    //     return 1;
                    // }
                    

                    char* move = strdup(fields[3]);
                    printf("String Rep of MOVE: %s\n", move);



                    // int array for x, y
                    int coordinates[2];

                    // token - make sure we only read the first
                    char* tok;
                    int i = 0;
                    tok = strtok(move, ",");
                    while (tok != NULL) {
                        if (i > 1) {
                            printf("INVALID FORMAT\n");
                        }
                        coordinates[i] = atoi(tok);
                        tok = strtok(NULL, ",");
                        i++;
                    }
                    // should print out 2, 2
                    printf("X: %d, Y: %d\n", coordinates[0], coordinates[1]);
                    
                    //printf("Begin X\n");
                    int x = coordinates[0];
                    x--;
                    //printf("X: %d\n", x);
                    //printf("End X\n");
                    
                    //printf("Begin Y\n");
                    int y = coordinates[1];
                    //printf("Y: %d\n", y);
                    y--;
                    //printf("End Y\n");

                    if ((x < 0 || x > 2) || (y < 0 || y > 2)) {
                        printf("INVALID MOVE\nNUMBERS OUT OF RANGE\n");
                        return 1;
                    }

                    // should print out 1,
                    printf("MOVE: (%d,%d)\n", x, y);

                    // move syntax is valid
                    // check if move on board is valid.
                    
                    // print string_board
                    helper();

                    make_string_board();
                    printf("STRING REP OF BOARD: %s\n", string_board);

                    // print string_board as string
                    //xprintf("STRING REP\n%s\n", string_board);

                    if (check_move(x, y) == 0) {
                        // move is valid
                        printf("MOVE IS VALID\n");
                        board[x][y] = curr->player.role[0];
                    }

                    // print string_board
                    //print_board();
                    helper();

                    make_string_board();
                    printf("STRING REP OF BOARD: %s\n", string_board);

                    // print string_board as string
                    //printf("STRING REP\n%s\n", string_board);


               // } // need to comment this back in
            }
            if(curr->next != NULL){
                curr = curr->next;
                printf("iterated");
            }
            else{
                break;
            }
        }
        //fix this so that is formatted properly
        char* temp = "MOVD";

        // make buffer
            char buffer[1000];
            memset(buffer, 0, BUFFER_SIZE);

            // store message for player 1
            sprintf(buffer, "MOVD|11|%c|%s|", curr->player.role[0], string_board);
            
            // send message to player 1
            int send_size = send(s1, strdup(buffer), strlen(buffer), 0);
            //printf("Send Size: %d\n", send_size);

            // reset buffer
            memset(buffer, 0, BUFFER_SIZE);

            // store message for player 2
            sprintf(buffer, "MOVD|11|%c|%s|", curr->player.role[0], string_board);
            
            // send message to player 2
            send_size = send(s2, strdup(buffer), strlen(buffer), 0);
            //printf("Send Size: %d\n", send_size);

        //send(s1, temp, strlen(temp), 0);
        read_message()
        return 0;
    }
    else if(strcmp(code,"RSGN") == 0){
        char* m = "OVER";

        Node * curr = node_head;
        char buffer[1000];
        memset(buffer, 0, BUFFER_SIZE);

        // store message for player 1
        sprintf(buffer, "MOVD|11|%c|%s|", curr->player.role[0], string_board);
        
        // send message to player 1
        int send_size = send(s1, strdup(buffer), strlen(buffer), 0);
        //printf("Send Size: %d\n", send_size);

        // reset buffer
        memset(buffer, 0, BUFFER_SIZE);

        // store message for player 2
        sprintf(buffer, "MOVD|11|%c|%s|", curr->player.role[0], string_board);
        
        // send message to player 2
        send_size = send(s2, strdup(buffer), strlen(buffer), 0);
        //printf("Send Size: %d\n", send_size);

        send(s1, m, strlen(m), 0);
        return 1;
    }

}

void *handle_client(void *arg) {
    
    int socket1 = *((int *)arg);
    int socket2 = *((int *)arg+1);
    int player1_turn = 1;

    while (1) {
        // game is set up
        printf("NUMBER OF PLAYERS = %d\n\n", num_players);
        if (num_players == 2 && game_started == 0) {
            // TODO FIX THISW
            // iterate through each node - get players sock_fd. compare to socket1 / socket 2
            Node * curr = node_head;
            Player p1 = {0};
            Player p2 = {0};
            while (curr != NULL) {
                if (curr->player.client_sock == socket1) {
                    curr->player.role = "X";
                    p1 = curr->player;

                }
                else if (curr->player.client_sock == socket2) {
                    curr->player.role = "O";
                    p2 = curr->player;
                }
                curr = curr->next;
            }
            // BEGN messages have been sent to both players.
            game_started = 1;
            printf("GAME STARTED!\ngame_started = %d\n", game_started);
            printf("Player1\nName: %s\nSocket: %d\nRole: %s\n", p1.name, p1.client_sock, p1.role);
            printf("Player2\nName: %s\nSocket: %d\nRole: %s\n", p2.name, p2.client_sock, p2.role);

            // make buffer
            char buffer[1000];
            memset(buffer, 0, BUFFER_SIZE);

            // store message for player 1
            sprintf(buffer, "BEGN|%d|%s|Opponent is %s|", (15 + strlen(p2.name)), p1.role, p2.name);
            
            // send message to player 1
            int send_size = send(socket1, strdup(buffer), strlen(buffer), 0);
            printf("Send Size: %d\n", send_size);

            // reset buffer
            memset(buffer, 0, BUFFER_SIZE);

            // store message for player 2
            sprintf(buffer, "BEGN|%d|%s|Opponent is %s|", (15 + strlen(p1.name)), p2.role, p1.name);
            
            // send message to player 2
            send_size = send(socket2, strdup(buffer), strlen(buffer), 0);
            printf("Send Size: %d\n", send_size);


        }
        if (player1_turn) {
            printf("PLAYER 1 TURN:\n\n");
            // Read from the first socket
            if (read_message(socket1, socket2) == 0) {
                // read message was successful and a move was made.
                player1_turn = !player1_turn;
            }
            // read unsuccessful - stay in client loop
        }
        else if (!player1_turn) {
            printf("PLAYER 2 TURN:\n\n");
            if (read_message(socket2, socket1) == 0) {
                // read message was successful and a move was made.
                player1_turn = !player1_turn;
            }
            // read unsuccessful - stay in client loop
        }
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