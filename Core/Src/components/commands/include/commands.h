
#include "config.h"
#include <inttypes.h>
#ifndef COMMAND_H_
#define COMMAND_H_

#define MAX_PARAMS 8
typedef struct command
{
    unsigned char *unformattedString;
    unsigned char *rootCommand;
    unsigned char *parameters[MAX_PARAMS]; 
    uint8_t paramsCount;
} command_t;
void command_constructor(command_t *instance, uint8_t* unformattedString);
void command_deconstructor(command_t *instance);
uint8_t command_isconsistent(command_t *instance);
#endif // COMMAND_H_

