/*
 * functions.h
 *
 *  Created on: Feb 2, 2015
 *      Author: williamcallaghan
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
void handle_io(char* input_line, char* tokens, int numTokens);
void prepare_arguments(char** input, char** output, int numTokens);

#endif /* FUNCTIONS_H_ */
