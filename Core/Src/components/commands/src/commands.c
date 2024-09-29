/**
 * @file      command.c
 * @brief     your file's description
 * @version   0.1
 * @date      02-09-2024
 * @copyright 2024, your company / association / school
 */

#include "commands.h"
#include <stdlib.h>
#include <inttypes.h>   
#include "parser.h"

void command_constructor(command_t *instance)
{
    // Initialize parameters array to NULL
    for (int i = 0; i < MAX_PARAMS; i++)
    {
        instance->parameters[i] = NULL;
    }
    instance->paramsCount = 0;
    instance->rootCommand = NULL;
    instance->unformattedString = NULL;
}
void command_deconstructor(command_t *instance)
{
    // Free the root command
    free(instance->rootCommand);
    // Free the unformatted string
    free(instance->unformattedString);
    // Free the parameters
    for (int i = 0; i < MAX_PARAMS; i++)
    { 
        if (instance->parameters[i] != NULL)
        {
            free(instance->parameters[i]);
        }
    }
}
/**
 * @brief      Check if the command instance is consistent
 * @details    The command is considered consistent if:
 *             - the root command is not empty
 *             - the unformatted string is not empty
 *             - the parameters are not empty and not null
 * @param      instance The command instance to be checked
 * @return     1 if the command is consistent, 0 otherwise
 */
uint8_t  command_isconsistent(command_t *instance)
{
    uint8_t isConsistent=1;
    // Check if the root command is not empty
    if (strlen((char*)(instance->rootCommand)) == 0 || grabLastChar(instance->rootCommand, strlen((char*)(instance->rootCommand))) == -1)
    {
        isConsistent=0;
    }
    // Check if the unformatted string is not empty
    if (strlen((char*)instance->unformattedString) == 0 || grabLastChar(instance->unformattedString, strlen((char*)instance->unformattedString)) == -1)
    {
        isConsistent=0;
    }
    // Check if the parameters are consistent
    for (int i = 0; i < instance->paramsCount; i++)
    {
        if (instance->parameters[i] != NULL)
        {
            if (strlen((char*)instance->parameters[i]) == 0)
            {
                isConsistent=0;
                break;
            }
        }
    }
    return isConsistent;
}
// end of file command.c
