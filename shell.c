/*
 * Shell.c
 * Author: William Callaghan
 * Student #: 250564293
 * GAUL ID: wcallag3
 * CS 3305B
 */
#include "functions.h"

int main(int argc, char** argv)
{
    const char* user;           //Username
    char input_line[MAX];       //Stores user input
    char history_input[MAX];    //Copy of user input
    char* tokens[CMD_MAX];      //Stores tokenized input
    char* history[HIST_MAX];    //Stores history of previous commands
    int n, numCommands;

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

            //If the number of tokens is greater than 1
            //(ie. there is a pipe)
            if (n > 1)
            {
                //Handle the pipes.
                handle_pipes(tokens, n);
                //Add command to history.
                numCommands = addToHistory(history,history_input, numCommands);
                continue;
            }
            else
            {
                //Check to see if there is i/o redirection ('<' and/or '>')
                //If so, deal with it appropriately.
                n = make_tokenlist(input_line, tokens, "<>");
                if(n > 1)
                {
                    handle_io(history_input, tokens, n);
                    //Add command to history.
                    numCommands = addToHistory(history,history_input, numCommands);
                    continue;
                }
            }
            //If we reached this point, there are no pipes or i/o
            //redirection. Split up the input line by the delimiters specified.
            n = make_tokenlist(input_line, tokens, " \t\n");

            //If the first token is null, nothing was entered.
            //Go to beginning of loop.
            if(tokens[0]==NULL)
            {
                continue;
            }
            //If the user entered 'exit' then exit the shell.
            if(strcmp(tokens[0],"exit") == 0)
            {
                //Add command to history.
                numCommands = addToHistory(history,history_input, numCommands);
                printf("NOTICE: Exiting shell.\n");
                return 0;
            }
            //If the user entered 'history' then list the history
            if(strcmp(tokens[0], "history") == 0)
            {
                //If the user specifies a numerical argument
                //take this into account.
                if(n >= 2 && atoi(tokens[1]) != 0)
                    printHistory(history, numCommands, atoi(tokens[1]));
                //Otherwise, print the last 10 commands.
                else
                    printHistory(history,numCommands,NO_HIST);

                //Add command to history.
                numCommands = addToHistory(history,history_input, numCommands);
                continue;
            }
            //If we have reached this point, there are no pipes, i/o redirection
            //or built-in functions (just a regular command). Execute this command.
            exec_command(tokens,n);

            //Add command to history.
            numCommands = addToHistory(history,history_input, numCommands);
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

