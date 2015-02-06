/*
 * shell.c
 *
 *  Created on: Feb 2, 2015
 *      Author: williamcallaghan
 */
#include "functions.h"

int main(int argc, char** argv)
{
    const char* user;
    char input_line[MAX];
    char history_input[MAX];
    char* tokens[CMD_MAX];
    char* history[HIST_MAX];
    int i, n, numCommands;
    int fd[2];
    pid_t pid;
    int status;

    //Get the username
    user = getUserName();

    //Initialize variables
    numCommands = 0;

    while(1)
    {
        //Print prompt
        printf("%s> ", user);

        //Read the characters from the stream
        if (fgets(input_line,MAX,stdin) != NULL)
        {
            //Make a copy of the input line before parsing
            //so that it can be added to history later.
            strcpy(history_input,input_line);

            //First tokenize by pipe to determine number of pipes needed.
            //(Later we will tokenize by space).
            n= make_tokenlist(input_line, tokens, "|");

            //If the first token is null then nothing was entered.
            //Go back to beginning of while loop.
            if(tokens[0] == NULL)
            {
                continue;
            }
            //If the user entered 'exit' then exit the shell.
            if(strcmp(tokens[0],"exit") == 0)
            {
                numCommands = addToHistory(history,history_input, numCommands);
                printf("NOTICE: Exiting shell.\n");
                return 0;
            }
            for (i = 0; i < n; i++)
            {
                printf("extracted token is %s\n", tokens[i]);
                if(strcmp(tokens[i],"|") == 0)
                {

                }

            }


        }
        //If there was an error reading the characters from stream
        //display an error message and exit the shell.
        else
        {
            printf("ERROR: A problem occurred while reading the characters from stream. Exiting shell.\n");
            exit(EXIT_FAILURE);
        }
    }
}

