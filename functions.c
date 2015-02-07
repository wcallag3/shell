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
    if(desiredHistory == NO_HIST)
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

    for(index=count-toPrint;index < count; index++)
    {
        printf("%s", history[index]);
    }
}

/*
void spawn_process(int in, int out, char* command)
{
    pid_t pid;
    if((pid = fork()) == 0)
    {
        if (in != 0)
        {
            dup2(in,0);
            close(in);
        }
        if (out != 1)
        {
            dup2(out,1);
            close(out);
        }
        exec_pipes(command);
    }
}

void handle_pipes(char *tokens[], int numTokens)
{
    int fd[2];
    pid_t pid;
    int i, in;

    in = 0;

    for(i=0; i < numTokens - 1; i++)
    {
        printf("TOKEN AT i: %s\n", tokens[i]);
        pipe(fd);
        printf("TOKEN AT i: %s\n", tokens[i]);
        spawn_process(in, fd[1], tokens[i]);
        close(fd[1]);
        in = fd[0];
    }
    if (in != 0)
    {
        dup2(in,0);
    }
    exec_pipes(tokens[i]);

}*/

void handle_pipes(char *tokens[], int numTokens)
{
    int numPipes = numTokens - 1;
    int i;
    int fd[2];
    int fd2[2];
    pid_t pid;

    for(i=0;i<numTokens;i++)
    {
        //If there is a next command, then pipe
        if(i!= numTokens-1)
            pipe(fd2);

        //Fork a process
        pid = fork();
        if (pid < 0)
        {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        //If child
        if (pid == 0)
        {
            //If there is a previous command
            if(i!=0)
            {
                if(dup2(fd[0], 0) < 0)
                {
                    perror("Cannot dup");
                    exit(EXIT_FAILURE);
                }
                close(fd[0]);
                close(fd[1]);
            }
            //If there is a next command
            if(i!= numTokens-1)
            {
                close(fd2[0]);
                if(dup2(fd2[1], 1) < 0)
                {
                    perror("Cannot dup");
                    exit(EXIT_FAILURE);
                }
                close(fd2[1]);
            }
            exec_pipes(tokens[i]);
        }
        else
        {
            if(i!=0)
            {
                close(fd[0]);
                close(fd[1]);
            }
            if(i!= numTokens - 1)
            {
                fd[0] = fd2[0];
                fd[1] = fd2[1];
            }
        }
    }
}
/*
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
            printf("TOKENS AT i: %s\n", tokens[i]);
            exec_pipes(tokens[i]);
        }
        else
        {
            wait(0);
        }
    }

    for(i=0; i < (2*numPipes); i++)
    {
        close(fd[i]);
    }
}*/

void exec_pipes(char* command)
{
    int n;
    char* tokens[CMD_MAX];

    n = make_tokenlist(command,tokens, " ");
    if(n < 1)
    {
        perror("Cannot tokenize");
        exit(EXIT_FAILURE);
    }
    execute(tokens, n);
}

void exec_command(char* tokens[], int numTokens)
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        perror("Fork error");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        execute(tokens,numTokens);
    }
    else
    {
        wait(0);
    }
}

void prepare_arguments(char** input, char** output, int numTokens)
{
    int i;
    for(i=0;i<numTokens;i++)
    {
        output[i] = input[i];
    }
    output[numTokens] = NULL;
}

void execute(char* tokens[], int numTokens)
{
    int status;
    char* arguments[numTokens+1];

    prepare_arguments(tokens,arguments,numTokens);
    status = execvp(tokens[0], arguments);

    if(status<0)
    {
        perror("Execution Error");
        exit(EXIT_FAILURE);
    }
}

void handle_io(char* input_line, char* tokens[], int numTokens)
{
    char* input[CMD_MAX];
    char* output[CMD_MAX];
    int fd[2];
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        perror("Fork error");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        if(numTokens == 3)
        {
            //Prepare i/o tokens (make sure they
            //are correctly formated)
            prepare_io(tokens[1], input);
            prepare_io(tokens[2], output);
            //Redirect i/o
            redir_both(input[0],output[0], fd);
            //Execute
            exec_pipes(tokens[0]);
        }
        else
        {
            //Prepare a single i/o redirection
            //and execute.
            prepare_one_io(input_line, fd);
        }
    }
    else
    {
        //Wait for child process to finish.
        wait(0);
    }
}

void redir_in(char* inputFile, int* fd)
{
    fd[0] = open(inputFile,O_RDONLY, 0);
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
}

void redir_out(char* output, int* fd)
{
    fd[1] = creat(output, 0644);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);
}

void redir_both(char* input, char* output, int* fd)
{
    redir_in(input, fd);
    redir_out(output, fd);
}

void prepare_io(char* command, char** output)
{
    int n;

    n = make_tokenlist(command,output, " \t\n");
    if(n < 1)
    {
        perror("Cannot tokenize");
        exit(EXIT_FAILURE);
    }
}

void prepare_one_io(char* command, int fd[])
{
    int n;
    int status = -1;
    char* tokens[CMD_MAX];
    char copy[CMD_MAX];
    char* io[CMD_MAX];

    //Make a copy of the command
    //It will be needed if the first condition fails.
    strcpy(copy,command);

    //Check to see if there is a '<'
    n = make_tokenlist(command,tokens, "<");
    if(n < 1)
    {
        perror("Cannot tokenize");
        exit(EXIT_FAILURE);
    }
    //If there is more than one token, then a '<'
    //is present. Parse the token.
    if(n>1)
    {
        status = 0;
        prepare_io(tokens[1], io);
    }
    //In this case, there must be a '>' present
    else
    {
        //Check to make sure this is true
        n = make_tokenlist(copy,tokens, ">");
        if(n < 1)
        {
            perror("Cannot tokenize");
            exit(EXIT_FAILURE);
        }
        if(n>1)
        {
            status = 1;
            prepare_io(tokens[1], io);
        }
    }
    //If there is a '<'
    //Redirect the input and execute command
    if (status == 0)
    {
        redir_in(io[0],fd);
        exec_pipes(tokens[0]);

    }
    //If there is a '>'
    //Redirect the output and execute command
    else if (status == 1)
    {
        redir_out(io[0],fd);
        exec_pipes(tokens[0]);
    }
}
