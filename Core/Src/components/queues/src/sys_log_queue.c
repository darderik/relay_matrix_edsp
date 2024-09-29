#include "queues.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"

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
{    {
        if (strlen((char *)sysLogMessage)+SYSLOG_SINGLE_MESSAGE_LENGTH < MAX_SYSLOG_BUFFER_SIZE)
        {
            // Bring in scope struct fields and pop element
            interpreter_status_t *curStatus = &(statusQueue_list.head->status);
            action_return_t action = curStatus->action_return;
            command_t command = curStatus->command;
            interpreter_flag_t curStatusFlag = curStatus->status;
            statusQueue_popElement();

            unsigned char **args = command.parameters;
            uint8_t argsCount = command.paramsCount;
            char formattedStr[SYSLOG_SINGLE_MESSAGE_LENGTH] = {'\0'};
            snprintf(formattedStr, sizeof(formattedStr),
                     "\r\n ---COMMAND DIAGNOSTIC--- \n\r"
                     "Command: %s Status: %d 0->ok, Interpreter Status: %s,Message: %s"
                     " \n\r Args:",
                     command.rootCommand, action.status, interpreter_flag_msg[curStatusFlag], action.data);
            if (strlen((char *)sysLogMessage) + strlen(formattedStr) < MAX_SYSLOG_BUFFER_SIZE)
            {
                strncat((char *)sysLogMessage, formattedStr, sizeof(sysLogMessage) - strlen((char *)sysLogMessage) - 1);
                for (uint8_t j = 0; j < argsCount; j++)
                {
                    char *curArg = (char *)args[j];
                    char argStr[64];
                    snprintf(argStr, sizeof(argStr), "\n\r ---> Arg %d: %s \n\r", j, curArg);
                    strncat((char *)sysLogMessage, argStr, sizeof(sysLogMessage) - strlen((char *)sysLogMessage) - 1);
                }
            }
        } else {
            // Queue is full
            memset(sysLogMessage, 0, sizeof(sysLogMessage));
            sysLogQueue_addMessage("--WARNING--\n\r sysLog was wiped.\n\r");
        }
    }
}

void sysLogQueue_addMessage(char *message)
{
    if (strlen((char *)sysLogMessage) + strlen(message) < MAX_SYSLOG_BUFFER_SIZE)
    {
        strcat((char *)sysLogMessage, message);
    }
}