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

#define MAX 256
#define CMD_MAX 10

int make_tokenlist(char *buf, char *tokens[]);
const char* getUserName();

#endif /* FUNCTIONS_H_ */
