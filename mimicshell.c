/*******************************************************
* Author: Albert Nim
* Assignment: Small Shell
* Date: 11/14/17
* Course: CS344
* Description: A shell programmed in C with various functions
*********************************************************/

#include "mimicshell.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

//main function
int main() {

//signs for exit
    int exitCalled = 0;
    int exitStatus = 0;
//continually runs while needed
    do {
        printf(": ");
        fflush(stdout);
//variables to store the input of user
        char *line = NULL;
        char **args;
        int fd;
        arguments = 0;
        background = 0;
// read values entered by user and parse them
        line = readLine();
        args = parse(line);

//check if the user is inputting a background process 
        if (!(strncmp(args[arguments - 1], "&", 1))) {
            background = 1;
            args[arguments - 1] = NULL;
        }
//if there is a blank command or comment, continue
        if(args[0] == NULL || !(strncmp(args[0], "#", 1))) {
            exitStatus = 0;
        }

//if the user wants to change directory
        else if(strcmp(args[0], "cd") == 0) {
            if(args[1]){
//attempt to find the directory the user wanted to change to
                if(chdir(args[1]) != 0){
                    printf("No such file or directory\n");
                    exitStatus = 1;
                }
//otherwise, go back to home directory
            } else {
                chdir(getenv("HOME"));
                exitStatus = 0;
            }
        }
//utilizing built in status command 
        else if (strcmp(args[0], "status") == 0) {
            printf("Exit status: %d\n", exitStatus);
            exitStatus = 0;
        }

//user has entered an exit command to leave shell
        else if(strcmp(args[0], "exit") == 0) {
//change exit status so that loop ends
            exitCalled = 1;
            exitStatus = 0;
        }

//tests input and outputs
        else if (arguments == 3 && ( (strcmp(args[1], ">") == 0) || (strcmp(args[1], "<") == 0) )) {
            int copy_stdout, copy_stdin;
//copies the file descriptors            
            copy_stdout = dup(1);
            copy_stdin = dup(0);
            if (strcmp(args[1], ">") == 0) {
//opens the file if possible
                fd = open(args[2], O_WRONLY|O_CREAT|O_TRUNC, 0644);
//if not, then output an error message
                if (fd == -1) {
                    printf("No such file or directory\n");
                    exitStatus = 1;
                } else {
//copies file and closes original
                    dup2(fd, 1);
                    args[1] = NULL;
                    close(fd);
//launches the arguments
                    exitStatus = launch(args);
                }
            } else if (strcmp(args[1], "<") == 0) {
//opens a file to be read from 
                fd = open(args[2], O_RDONLY);
//prints an error otherwise
                if (fd == -1) {
                    printf("No such file or directory\n");
                    exitStatus = 1;
                } else {
//redirects the file and closes original
                    dup2(fd, 0);
                    args[1] = NULL;
                    close(fd);
                    exitStatus = launch(args);
                }
            }
//close all files that arent necessary
            dup2(copy_stdout, 1);
            close(copy_stdout);
            dup2(copy_stdin, 0);
            close(copy_stdin);
        }
        else {
            exitStatus = launch(args);
        }
//frees memory
        free(line);
        free(args);
    } while (!exitCalled);
    return 0;
}

//gathers the user's stream and creates a buffer
char *readLine() {
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

//turns the user's input into tokens
//allows for input to be analyzed
char **parse(char *line) {
//set the buffer size for each token
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
//uses token constant to break up user's input
    token = strtok(line, TOKENS);
    while (token != NULL) {
        tokens[position] = token;
//finds total position of tokens
        arguments++;
        position++;
//creates more memory in case at end of buffer
        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char*));
        }
//continues process until NULL
        token = strtok(NULL, TOKENS);
    }
    tokens[position] = NULL;
    return tokens;
}

//passes the list of arguments that were collected
//forks the processes if necessary
int launch(char **args) {
//initializes pid for reference
    pid_t pid, wpid;
    int status, exitStatus = 0;
//forks and creates a child process
    pid = fork();
// if pid == 0, this is the child process
    if (pid == 0) {
//runs command if possible
        if (execvp(args[0], args) == -1) {
            printf("Command or file does not exist\n");
            exit(1);
        }
    }
//checks for errors in input
    else if (pid < 0) {
        perror("smallsh");
    }
//otherwise, this is parent process
    else {
// parent process waits for the child process
        do {
            if (background == 0)
                wpid = waitpid(pid, &status, WUNTRACED);
//if there is no background process, proceed
            else if (background == 1)
                wpid = waitpid(-1, &status, WNOHANG);
//wait for the child process to exit or kill
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

//prints out PID of background
    if (background == 1)
        printf("Background PID: %d\nExit status: %d", pid, exitStatus);

//WIFSIGNALED shows if child process was terminated form signal
    if (status != 0 || WIFSIGNALED(status))
        exitStatus = 1;
    return exitStatus;
}