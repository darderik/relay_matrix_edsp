#include "queues.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
//--------------------Sys Log Queue Helper--------------------
void sysLogQueue_populate()
{    {
        int8_t curIdx = statusQueue_getFirstElementIdx();
        if (curIdx > -1 && strlen((char *)sysLogMessage)+SYSLOG_SINGLE_MESSAGE_LENGTH < MAX_SYSLOG_BUFFER_SIZE)
        {
            interpreter_status_t *curStatus = statusQueue[curIdx];
            action_return_t action = curStatus->action_return;
            command_t command = curStatus->command;
            interpreter_flag_t curStatusFlag = curStatus->status;
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
            statusQueue_removeSpecificElement(curStatus);
        } else {
            statusQueue_clear();
        }
    }
}