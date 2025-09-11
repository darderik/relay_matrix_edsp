
#include "config.h"
#include <inttypes.h>
#ifndef COMMAND_H_
#define COMMAND_H_

typedef struct command
{
    unsigned char unformattedString[MAX_COMMAND_LENGTH];
    unsigned char rootCommand[MAX_COMMAND_LENGTH];
    unsigned char parameters[MAX_PARAMS][MAX_PARAM_LENGTH];
    uint8_t paramsCount;
} command_t;
void command_constructor(command_t *instance, uint8_t *unformattedString);
uint8_t command_isconsistent(command_t *instance);
#endif // COMMAND_H_
