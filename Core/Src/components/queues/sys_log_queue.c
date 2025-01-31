#include "queues.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "config.h"
/**
 * @brief Populates sysLogMessage with the status of the interpreter
 *        in a human-readable format.
 *
 * @details This function will remove the head of the statusQueue_list
 *          and use its contents to generate a sysLogMessage. If the
 *          sysLogMessage would grow larger than MAX_SYSLOG_BUFFER_SIZE,
 *          sysLogMessage is reset and a warning is added to the start
 *          of the message.
 */
void sysLogQueue_populate()
{
    int currentSize = strlen((char *)sysLogMessage);
    int projectedSize = currentSize + SYSLOG_SINGLE_MESSAGE_LENGTH;
    if (projectedSize < MAX_SYSLOG_BUFFER_SIZE && statusQueue_getSize() > 0)
    {
        // Bring in scope struct fields and pop element
        interpreter_status_t *curStatus = statusQueue_list.head->status;
        action_return_t action = curStatus->action_return;
        command_t command = curStatus->command;
        interpreter_flag_t curStatusFlag = curStatus->status;
        statusQueue_popElement();

        unsigned char **args = command.parameters;
        uint8_t argsCount = command.paramsCount;
        char formattedStr[SYSLOG_SINGLE_MESSAGE_LENGTH] = {'\0'};

        // Prepare args list
        char argsList[128] = {'\0'};
        for (uint8_t j = 0; j < argsCount; j++)
        {
            char *curArg = (char *)args[j];
            char argStr[64];
            snprintf(argStr, sizeof(argStr), "---> Arg %d: %s %s", j, curArg, NEWLINE_STR);
            if (strlen(argsList) + strlen(argStr) < sizeof(argsList) - 1)
            {
                strncat((char *)argsList, argStr, strlen(argStr));
            }
        }

        snprintf(formattedStr, sizeof(formattedStr),
                 "---COMMAND DIAGNOSTIC--- %s"
                 "Command: %s Status: %d, Interpreter Status: %s %s"
                 "Message: %s %s"
                 "Args: %s %s",
                 NEWLINE_STR, command.rootCommand, action.status, interpreter_flag_msg[curStatusFlag], NEWLINE_STR, action.message, NEWLINE_STR, argsList, NEWLINE_STR);

        if (currentSize + strlen(formattedStr) + 1 < MAX_SYSLOG_BUFFER_SIZE)
        {
            strncat((char *)sysLogMessage, formattedStr, strlen(formattedStr));
        }
    }
    else if (statusQueue_getSize() > 0)
    {
        // Queue is full
        memset(sysLogMessage, 0, sizeof(sysLogMessage));
        char msg[64];
        snprintf(msg, sizeof(msg), "--WARNING--%s sysLog was wiped.%s", NEWLINE_STR, NEWLINE_STR);
        sysLogQueue_addMessage(msg);
    }
}

void sysLogQueue_addMessage(char *message)
{
    if (strlen((char *)sysLogMessage) + strlen(message) + 1 < MAX_SYSLOG_BUFFER_SIZE)
    {
        strcat((char *)sysLogMessage, message);
    }
}