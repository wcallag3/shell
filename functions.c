/*
 * functions.c
 *
 *  Created on: Feb 2, 2015
 *      Author: williamcallaghan
 */
#include "functions.h"

int make_tokenlist(char *buf, char *tokens[], char *delims)
{
    char *line;
    int i;

    i = 0;

    line =   buf;
    tokens[i] = strtok(line, " \n\t");
    do  {
        i++;
        line = NULL;
        tokens[i] = strtok(line, " \n\t");
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
    printf("HISTORY: %s\n", command);
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
