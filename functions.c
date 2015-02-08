/*
 * functions.c
 * Author: William Callaghan
 * Student #: 250564293
 * GAUL ID: wcallag3
 * CS 3305B
 */
#include "functions.h"

/*
 * Tokenizer provided by Hanan
 */
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

/*
 * Gets the username from the environment
 * variable user.
 */
const char* getUserName()
{
    char* user = getenv("USER");
    if(user)
        return user;

    return "";
}

/**
 * Adds the current command to history
 */
int addToHistory(char* history[], char* command, int count)
{
    int index;
    //If the current count of commands is less than the
    //max that history can handle
    if(count < HIST_MAX)
    {
        //Add the command to the history.
        history[count] = strdup(command);
        count++;
    }
    else
    {
        //Otherwise, we need to bump out one command
        //move the rest and insert the new command.
        free(history[0]);
        for(index=1; index < HIST_MAX; index++)
        {
            history[index-1] = history[index];
        }
        history[HIST_MAX-1] = strdup(command);
    }

    //Return the new count.
    return count;
}

/*
 * Prints the command line history to the screen.
 */
void printHistory(char* history[], int count, int desiredHistory)
{
    int toPrint;
    int index;

    //If the user did not provide a numerical argument
    //Set toPrint to either 10 or less commands.
    if(desiredHistory == NO_HIST)
    {
        if (count < 10) toPrint = count;
        else toPrint = 10;
    }
    //Set toPrint to desired history or the
    //max number of commands possible.
    else if (count%HIST_MAX < desiredHistory)
    {
        toPrint = count%HIST_MAX;
    }
    //Set the desired history amount.
    else
        toPrint = desiredHistory;

    //Print the commands.
    for(index=count-toPrint;index < count; index++)
    {
        printf("%s", history[index]);
    }
}

void handle_pipes(char *tokens[], int numTokens)
{
    int i = 0;
    int k = 0;
    int numPipes = numTokens - 1;
    pid_t pid;
    int status;

    int pipefds[2*numPipes];
    printf("%lu\n", sizeof(pipefds));
    printf("%d\n", numPipes);

    for(i=0;i < numPipes; i++)
    {
        if(pipe(pipefds + i*2) < 0)
        {
            perror("Fatal error");
            exit(EXIT_FAILURE);
        }
    }

    int j=0;
    for(i=0; i<numTokens;i++)
    {
        printf("first check: i is: %d\n", i);
        printf("Tokens at %d: %s\n", i, tokens[i]);
        pid = fork();
        printf("FORK: Tokens at %d: %s - PID: %d\n", i, tokens[i], pid);
        if (pid < 0)
        {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            //If not the last command
            if (i < numTokens-1)
            {
                if(dup2(pipefds[j+1], 1) < 0)
                {
                    perror("dup error");
                    exit(EXIT_FAILURE);
                }
            }

            //If not the first command
            if (j!=0)
            {
                if(dup2(pipefds[j-2], 0) < 0)
                {
                    perror("dup error");
                    exit(EXIT_FAILURE);
                }
            }
            for(k=0; k < 2*numPipes; k++)
            {
                close(pipefds[k]);
            }
            printf("i is currently: %d\n", i);
            //Exec
            printf("Tokens at %d before passing: %s\n", i,tokens[i]);
            exec_pipes(tokens[i]);
        }
        j+=2;
    }

    for(i=0;i < 2*numPipes; i++)
    {
        close(pipefds[i]);
    }
    for(i=0;i < numPipes + 1; i++)
    {
        wait(&status);
    }
}

void exec_pipes(char* command)
{
    int n;
    char* tokens[CMD_MAX];
    printf("CMD: %s\n", command);

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
