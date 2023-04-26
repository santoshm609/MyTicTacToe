## Tic Tac Toe ##
Name: Santosh Maryala, netid: sm2378
Name: Advait Borkar, netid: adb222

Implementation Choice: Multi Threading Concurrent Games

Test Plan:
(a) Can two players connect succesfully using ip address and the same port as the server?
- By entering a specific domain and port number which matches the sever port, each player should be able to join the game and connect succesfully meeting with a "Enter Text" statement.

(b) Can a player send a PLAY message and will they meet with a WAIT message from the server?
- By inserting a message like "PLAY|10|Joe Smith|", the player succesfully selects play and meets with a "WAIT|0|" message from the server until the other player also says PLAY. After both say play, they both encounter with a BGN message from server.

(c) Will a player have the oppurtunity to reenter a message if they encounter with a game level error or applicatiom level error?
- When encountered with a a game level error or applicatiom level error, the player will see a "INVL" message with asks them to "Enter Text" again with a valid input.

(d) Will turns alternate for each player?
- Once the game begins, each player's turn will alternate for every valid input they provide.

(e) Will the game end if a player resigns?
- When either player inputs "RSGN", it ends the game for both players and sends an OVER message to both indicating if they won or not.

(f) Will a player have the oppurtunity to draw mid game?
- During any of the player's turns, if a player sends "DRAW|2|S|", it will ask the other player respond with "DRAW|2|A|" to accept or "DRAW|2|R" to reject

(g) What happens during a session level error?
- During any session level error, it will allow for the server to write to socket indicating the the connection was lost.

(h) What happens when a player wins the game through diagonal, horizontal, or vertical?
- Both players will encounter an OVER message indicating if they won, lost, or draw because the grid was full.

(i) Can more than one game occur at once?
- For every two players in the game, it will link them and start a respective game between each.

** Tested with multiple clients logging onto many ports all at once **
Test 1: 
1. Two players connect succesfully using ip address and the port as the server
2. When successful, running ./ttt 127.0.0.1 3301  and ./c2 127.0.0.3 3301 and ./ttts 3301, it sucessfully connects both players into one game - PASSED

Test 2: 
1. When player 1 sends PLAY|10|Joe Smith|, they encounter with WAIT|0| until the next player inserts PLAY - PASSED

Test 3: 
1. When player 1 sends PLAY|10|Joe Smith| and player 2 sends PLAY|4|Max|, player 1 encounters with BEGN|18|X|Opponent is Max| and player 2 enxounters BEGN|24|O|Opponent is Joe Smith| - PASSED

Test 4: 
1. When player 1 sends PLAY|10|Joe Smith| and player 2 sends PLAY|4|Max|, if player 1 decides to say PLAY|10|Joe Smith| again, server will notify INVL|46|You can not click play after you started game!| message - PASSED

Test 5:
1. When player 1 or 2 sends an invalid message such as MOVD|11|X|....X....|, server will respond with INVL|22|Incorrect Formatting!| since only server can send MOVD message - PASSED

Test 6: 
1.  When player 1 or 2 sends RSGN, server will end the game and respond with OVER|L|19|YOU HAVE RESIGNED.| and OVER|W|23|OPPONENT HAS RESIGNED.| to respective players - PASSED

Test 7:
1. When four distinct players log on to the same port as the server, the server splits them into twi games, all of their names have to be the same or else the server states INVL|23|Choose Different Name!| - PASSED

Test 8:
1. When player 1 wins the game with 3 X's in a row horizontally, vertically, or diagonally, sevrer sends OVER|22|W|You Got 3 In A Row!| for player 1. - PASSED

Test 9:
1. When player 1 wins the game with 3 X's in a row horizontally, vertically, or diagonally, sevrer sends OVER|10|L|You Lost.| for player 2. - PASSED


Design Description: 

int checkConnection(): 
- This function checks if the socket connection is still running and if not, if "write()" returns "-1", the function checks the value of the "errno"  variable to determine the type of error that occurred. If "errno" equals "EPIPE", this means that the socket has disconnected or disruppted, so the function prints "Session-level Error." to stdout 

int board_full()(): 
- This function essentially checks if the tic tac toe board is full by using nested for loops to iterate over the board. If counter becomes 0, it means board is full and returns 0 to indicate it.

int checkRow():
- This function essentially checks if there are three consectutive O's or X's in any row of the tic tac toe board. It uses to for loops by comparing the element in each spot with the first column first row mark. If there are 3 consecutive, it returns an int indicating if anyone one in terms of any row.

checkCol():
- Like checkRow(), this function  checks if there are three consectutive O's or X's in any column of the tic tac toe board. It uses to for loops by comparing the element in each spot with the first row first column mark. If there are 3 consecutive, it returns an int indicating if anyone one in terms of any column.

checkDiagonals():
- In this function, it checks individually at each diagonal location, returning an int for the match or not.

check_win():
- This method, based off, the ints returned from the previous methods, checks if any of the players won and if so, handles the win case accordingly.

make_string_board():
- This initializes the tictactoe board.

check_move():
- This function checks if at the given x,y position inserted by the player, if the spot is free or not and returns 1 if valid.

read_message():
- This is a function where it reads and parses over the network connection between two players. It takes in two file descriptors, each representing of each socket for each client, and performs appropriate validation on it after parsing. Establishes RSGN check in here and checks if the bytes are equal in length so that they are valid. Also implements PlayCheck so that no player inserts PLAY more than once.

handle_client():
- This is the main function where it handles the client-server communication for each thread. It makes appropriate function calls where it also initializes each player in the linked list as a role after connecting them. 

** We were able to run all test cases in the command line terminal and all test cases passed accordingly. **

** We were able to prove all of our design properties through our rigourous testing methods and error statements. **