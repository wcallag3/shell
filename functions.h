/*
 * functions.h
 * Author: William Callaghan
 * Student #: 250564293
 * GAUL ID: wcallag3
 * CS 3305B
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX 256
#define CMD_MAX 10
#define HIST_MAX 50
#define NO_HIST -1

int make_tokenlist(char *buf, char *tokens[], char* delims);
const char* getUserName();
int addToHistory(char* history[], char* command, int count);
void printHistory(char* history[], int count, int num);
void handle_pipes(char *tokens[], int numTokens);
void exec_command(char *tokens[], int numTokens);
void exec_pipes(char *command);
void handle_io(char* input_line, char* tokens[], int numTokens);
void prepare_arguments(char** input, char** output, int numTokens);
void execute(char* tokens[], int numTokens);
void redir_in(char* inputFile, int* fd);
void redir_out(char* output, int* fd);
void redir_both(char* input, char* output, int* fd);
void prepare_io(char* command, char** output);
void prepare_one_io(char* command, int fd[]);

#endif /* FUNCTIONS_H_ */
