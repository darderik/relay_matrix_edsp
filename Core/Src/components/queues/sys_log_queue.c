#include "queues.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "config.h"

syslog_entry_t *syslog_head = NULL;

void sysLogQueue_addNode()
{
    if (statusQueue_list.head != NULL)
    {
        interpreter_status_t *curStatus = statusQueue_list.head->status;
        // Avoid adding diagnostic of sys log itself
        if (strstr((char *)curStatus->command.rootCommand, "sys:log") == NULL)
        {
            char message[SYSLOG_SINGLE_MESSAGE_LENGTH] = {'\0'};
            sysLogQueue_craftMessage(message, SYSLOG_SINGLE_MESSAGE_LENGTH, curStatus);

            syslog_entry_t *newNode = (syslog_entry_t *)malloc(sizeof(syslog_entry_t));
            char *fullMessage = (char *)malloc(strlen(message) + 1);
            strcpy(fullMessage, message);
            newNode->message = fullMessage;
            newNode->next = NULL;

            if (syslog_head == NULL)
            {
                syslog_head = newNode;
            }
            else
            {
                if (sysLogQueue_getSize() >= SYSLOG_MAX_MESSAGES)
                {
                    // FIFO mechanism
                    sysLogQueue_popNode();
                }

                syslog_entry_t *last = syslog_head;
                while (last->next != NULL)
                {
                    last = last->next;
                }
                last->next = newNode;
            }
        }
    }
    statusQueue_popElement();
}
void sysLogQueue_addNodeManual(char *msg)
{
    syslog_entry_t *newNode = (syslog_entry_t *)malloc(sizeof(syslog_entry_t));
    char *fullMessage = (char *)malloc(strlen(msg) + 1);
    strcpy(fullMessage, msg);
    newNode->message = fullMessage;
    newNode->next = NULL;

    if (syslog_head == NULL)
    {
        syslog_head = newNode;
    }
    else
    {
        if (sysLogQueue_getSize() >= SYSLOG_MAX_MESSAGES)
        {
            // FIFO mechanism
            sysLogQueue_popNode();
        }

        syslog_entry_t *last = syslog_head;
        while (last->next != NULL)
        {
            last = last->next;
        }
        last->next = newNode;
    }
}
void sysLogQueue_popNode()
{
    if (syslog_head != NULL)
    {
        syslog_entry_t *last = syslog_head;
        syslog_head = syslog_head->next;
        free(last->message);
        free(last);
    }
}
void sysLogQueue_forceUpdate()
{
    while (statusQueue_getSize() > 0)
    {
        sysLogQueue_addNode();
    }
}
void sysLogQueue_getFullMessage(char *fullMessage, uint16_t size)
{
    char *curTermChar = TERM_CHAR;
    uint8_t curTermCharLen = strlen(curTermChar);
    syslog_entry_t *last = syslog_head;
    while (last != NULL)
    {
        char *curMsg = last->message;
        int remainingLength = size - strlen(fullMessage) - curTermCharLen - 1;
        if (remainingLength > 0)
        {
            strncat(fullMessage, curMsg, remainingLength);
        }
        last = last->next;
        sysLogQueue_popNode();
    }
    //   // Ensure there is enough space for the termination character
    //   if (size > strlen(fullMessage))
    //   {
    //       strncat(fullMessage, curTermChar, size - strlen(fullMessage) - 1);
    //   }
}
uint16_t sysLogQueue_getTotalLength()
{
    syslog_entry_t *last = syslog_head;
    uint16_t totalLength = 0;
    while (last != NULL)
    {
        totalLength += strlen(last->message);
        last = last->next;
    }
    return totalLength;
}
uint8_t sysLogQueue_getSize()
{
    syslog_entry_t *last = syslog_head;
    uint8_t size = 0;
    while (last != NULL)
    {
        size++;
        last = last->next;
    }
    return size;
}
void sysLogQueue_craftMessage(char *message, uint16_t length, interpreter_status_t *curStatus)
{
    // Bring in scope struct fields and pop element
    action_return_t action = curStatus->action_return;
    command_t command = curStatus->command;
    interpreter_flag_t curStatusFlag = curStatus->status;

    unsigned char **args = command.parameters;
    uint8_t argsCount = command.paramsCount;

    // Prepare args list
    char argsList[384] = {'\0'};
    for (uint8_t j = 0; j < argsCount; j++)
    {
        char *curArg = (char *)args[j];
        char argStr[64];
        snprintf(argStr, sizeof(argStr), "---> Arg %d: %s %s", j, curArg, NEWLINE_STR);
        if (strlen(argsList) + strlen(argStr) + 1 < sizeof(argsList))
        {
            strncat((char *)argsList, argStr, strlen(argStr));
        }
    }
    snprintf(message, length,
             "---COMMAND DIAGNOSTIC--- %s"
             "Command: %s Status: %d, Interpreter Status: %s %s"
             "Message: %s %s"
             "Args: %s %s",

             NEWLINE_STR,
             command.rootCommand, action.status, interpreter_flag_msg[curStatusFlag], NEWLINE_STR,
             action.message, NEWLINE_STR,
             argsList, NEWLINE_STR);
}