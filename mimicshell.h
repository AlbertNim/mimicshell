/*******************************************************
* Author: Albert Nim
* Assignment: Small Shell 
* Date: 11/14/17
* Course: CS344
* Description: Header file for mimicshell.c
*********************************************************/

#ifndef SMALLSH_H
#define SMALLSH_H

//tokens for reference when taking them out of input
#define TOKENS " \t\r\n\a"

//global variables that measure status
int arguments;
int background;

//headers for functions
char *readLine();
char **parse(char *line);
int launch(char **args);

#endif