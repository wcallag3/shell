/*
 * functions.c
 *
 *  Created on: Feb 2, 2015
 *      Author: williamcallaghan
 */
#include "functions.h"

int make_tokenlist(char *buf, char *tokens[], char* delims)
{
    char *line;
    int i;

    i = 0;

    line =   buf;
    tokens[i] = strtok(line, delims);
    do  {
        i++;
        line = NULL;
        tokens[i] = strtok(line, delims);
    } while(tokens[i] != NULL);

    return i;
}

const char* getUserName()
{
    char* user = getenv("USER");
    if(user)
        return user;

    return "";
}

int addToHistory(char* history[], char* command, int count)
{
    int index;
    if(count < HIST_MAX)
    {
        history[count] = strdup(command);
        count++;
    }
    else
    {
        free(history[0]);
        for(index=1; index < HIST_MAX; index++)
        {
            history[index-1] = history[index];
        }
        history[HIST_MAX-1] = strdup(command);
    }

    return count;
}

void printHistory(char* history[], int count, int desiredHistory)
{
    int toPrint;
    int index;
    if(desiredHistory == -1)
    {
        if (count < 10) toPrint = count;
        else toPrint = 10;
    }
    else if (count%HIST_MAX < desiredHistory)
    {
        toPrint = count%HIST_MAX;
    }
    else
        toPrint = desiredHistory;

    for(index=0;index < toPrint; index++)
    {
        printf("%s\n", history[index]);
    }
}

void handle_pipes(char *tokens[], int numTokens)
{
    int numPipes = numTokens - 1;
    int fd[numPipes*2];
    int i;
    int fd_prev;
    int fd_forw;
    pid_t pid;

    for(i=0; i< (numPipes*2); i++)
    {
        if(pipe(fd + i*2) < 0)
        {
            perror("Fatal error");
            exit(EXIT_FAILURE);
        }
    }
    for(i=0; i<numTokens;i++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            //Position of the previous fd.
            fd_prev = (i-1)*2;
            //Position of the next fd.
            fd_forw = i*2+1;

            //If the command is not the first command
            //then the child gets the input from the
            //previous command.
            if(i!=0)
            {
                if(dup2(fd[fd_prev], 0) < 0)
                {
                    perror("Cannot dup");
                    exit(EXIT_FAILURE);
                }
            }

            //If the command is not the last command
            //then the child will send its output
            //to the next command.
            if(i!=(numTokens-1))
            {
                if(dup2(fd[fd_forw], 1) < 0)
                {
                    perror("Cannot dup");
                    exit(EXIT_FAILURE);
                }
            }

            close(fd[fd_prev]);
            close(fd[fd_forw]);
            exec_command(tokens[i]);
        }
    }

    for(i=0; i < 2*numPipes; i++)
    {
        close(fd[i]);
    }
}

void exec_command(char* command)
{
    int n;
    int status;
    char* tokens[CMD_MAX];
    printf("COMMAND IS: %s\n", command);
    n = make_tokenlist(command,tokens, " ");
    if(n < 1)
        perror("Cannot tokenize");
    if (n == 1)
    {
        printf("TOKEN IS: %s\n", tokens[0]);
        status = execlp(tokens[0],tokens[0], NULL);
    }
    else
    {
        printf("TOKEN IS: %s ARGS IS: %s\n", tokens[0], tokens[1]);
        status = execlp(tokens[0], tokens[0], tokens[1], NULL);
    }
    if(status < 0)
    {
        perror("Execution error");
        exit(EXIT_FAILURE);
    }
}
