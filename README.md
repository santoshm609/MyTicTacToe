## Tic Tac Toe ##
Name: Santosh Maryala, netid: sm2378
Name: Advait Borkar, netid: adb222

Implementation Choice: Multi Threading Concurrent Games

Test Plan:
(a) Does ?
- By entering a specific port

** Tested with multiple clients logging onto many ports all at once **
Test 1: 
1. Command line reads and tokenizes properly through our_split_line()
2. When successful, ourRead() and our_split_line() read from the command line and store each token into the token array. - PASSED

Test 2: 
1. Exit call ends the shell - PASSED

Test 3: 
1. With an input of "cd testDir", it changes directory and pwd returns the current new directory - PASSED

Test 4: 
1. pipe_implement() in execute() method runs properly
2. WHen successful, takes in arguments and flows in from one file to another - PASSED

Test 5:
1. wildcard() call in execute() method with *.c we want to switch to - PASSED

Test 6: 
1. bare_names() checks if the command is a file in respective directories and returns proper error message if not - PASSED

Test 7:
For this test, regarding our first extention, directory wildcards, when I input *.c, it returns all the .c files in our current working directory. - PASSED

Test 8:
For this test, regarding our second extention, when I call cd in the terminal it switches us and returns us to the home directory. When I enter a executable file like foo<~>, it redirects it to the home directory. - PASSED

Test 9:
Repeat for batch mode - PASSED


Design Description: 

** checkfile int checks whether or not the previous command was sucessful or not and therefore, prints !mysh< or mysh< accordingly

our_read_line(): 
- This function reads a line of input from the user, through the terminal, until it meets a newline or end-of-file character. IDynamically, it allocates memory for a buffer to store the input, and reallocates the buffer if necessary to accommodate longer input. It essentially implements line by line reading through storing in buffers to be split and tokenized later.

our_split_line(): 
- This function essentially splits the line of input character by character and tokenizes it. It initializes a token array and reads in each line until it meets with a space or any of the chars, "<,>,|". In the case of "<,>,|" it creates a duplicate of the character string and stores it into the token array. 

our_execute():
- This function essentially ties in all of our possible commands from the token array. It first addresses what the first token was and redirects into the following methods below based off what it matches. Also prints necessary error statements when met.

bare_names():
- This function checks whethere the first token, an executable file, is within the directories given. Iterates through them through the array and if found but no executable it immediately exits out. 

wildcard():
- In this method, we search for all of teh occurence of *. We then replace the * with all occurances of postring or prestring. The string before the * and the after are matched up withe the possible files in the working directory. 

file_redirection():
- This method iterates through the args array and searches for the first occurance of '<' or '>'. After it finds it it also finds the immediate token after that storing it into a char path variable. After running the first token and opening it up, it runs the file and read writes into it.

pipe_implement():
- This takes in a param of args and then connects two processes or subprocesses through the '|'. After locating the '|', we obtain the file descriptors of each file, and begin a process invoolving pipes. 

directory_wildcards():
- This function searches through the command line and obtains the tokene with the * and string part which is trailing the *. Then, it searches the current working directory and returns all matching files such as all c files as per our test.

home_directory():
- This is a function where it takes an array of arguments (args) and replaces any string that starts with a tilde character (~) with the corresponding home directory path. In addition, when cd is entered as a command, it automatically switch the current working directory to home directory through getenv("HOME") and chdir()

command_itterate():
- This is a iterate function which keeps the bash running till it reaches exit status 1. As long as it is running it passes the arguments into read, split, and execute to performa all of the appropriate functions.

** We were able to run all 8 commands in the command line terminal and all test cases passed as we wanted it to. We included all of the input files and extra directory with this project to prove our test cases to be true.

** We were able to prove all of our design properties through our rigourous testing methods and error statements.