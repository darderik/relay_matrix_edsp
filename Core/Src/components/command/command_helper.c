#include "command.h"
#include <stdlib.h>
#include <inttypes.h>
#include "parser.h"

void command_constructor(command_t *instance, unsigned char *unformattedString)
{
    // Initialize parameters to ""
    for (int i = 0; i < MAX_PARAMS; i++)
    {
        memset(instance->parameters[i], '\0', MAX_COMMAND_LENGTH);
    }
    instance->paramsCount = 0;
    memset(instance->rootCommand, '\0', MAX_COMMAND_LENGTH);
    memset(instance->unformattedString, '\0', MAX_COMMAND_LENGTH);
    parseCommand(instance, unformattedString);
}

uint8_t command_isconsistent(command_t *instance)
{
    uint8_t isConsistent = 1;
    // Check if the root command is not empty
    if (strlen((char *)(instance->rootCommand)) == 0 || grabLastChar(instance->rootCommand, strlen((char *)(instance->rootCommand))) == -1)
    {
        isConsistent = 0;
    }
    // Check if the unformatted string is not empty
    if (strlen((char *)instance->unformattedString) == 0 || grabLastChar(instance->unformattedString, strlen((char *)instance->unformattedString)) == -1)
    {
        isConsistent = 0;
    }
    // Check if the parameters are consistent
    for (int i = 0; i < instance->paramsCount; i++)
    {
        if (strlen((char *)instance->parameters[i]) != 0)
        {
            if (strlen((char *)instance->parameters[i]) == 0)
            {
                isConsistent = 0;
                break;
            }
        }
    }
    return isConsistent;
}
// end of file command.c
