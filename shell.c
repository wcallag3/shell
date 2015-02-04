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
    char* tokens[CMD_MAX];
    int i, n;

    //Get the username
    user = getUserName();

    while(1)
    {
        //Print prompt
        printf("%s> ", user);

        //Read the characters from the stream
        if (fgets(input_line,MAX,stdin) != NULL)
        {
            //Parse the input
            n= make_tokenlist(input_line, tokens);
            //If the first token is null then nothing was entered.
            //Go back to beginning of while loop.
            if(tokens[0] == NULL)
            {
                continue;
            }
            //If the user entered 'exit' then exit the shell.
            if(strcmp(tokens[0],"exit") == 0)
            {
                printf("NOTICE: Exiting shell.\n");
                return 0;
            }
            for (i = 0; i < n; i++)
                printf("extracted token is %s\n", tokens[i]);
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

