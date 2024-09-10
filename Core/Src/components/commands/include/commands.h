/**
 * @file      command.h
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      02-09-2024
 * @copyright 2024, your company / association / school
 */
#ifndef COMMAND_H_
#define COMMAND_H_

#define MAX_PARAMS 8
#include <inttypes.h>
typedef struct command
{
    unsigned char *unformattedString;
    unsigned char *rootCommand;
    unsigned char *parameters[MAX_PARAMS]; 
    uint8_t paramsCount;
} command_t;
void command_constructor(command_t *instance);
void command_deconstructor(command_t *instance);
uint8_t command_isconsistent(command_t *instance);
#endif // COMMAND_H_

