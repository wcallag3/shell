/*
 * functions.c
 *
 *  Created on: Feb 2, 2015
 *      Author: williamcallaghan
 */
#include "functions.h"

int make_tokenlist(char *buf, char *tokens[])
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
