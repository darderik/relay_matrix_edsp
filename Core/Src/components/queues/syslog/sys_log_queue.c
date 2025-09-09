#include "queues.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "config.h"
#include "sys_log_queue.h"

// Static allocation approach following ISQ and UCQ pattern
syslog_entry_t slq_queue[SYSLOG_MAX_MESSAGES];
slq_container_t syslog_container;

void slq_init(void)
{
    syslog_container.head = NULL;
    syslog_container.tail = NULL;

    // Set all entries as available
    for (size_t i = 0; i < SYSLOG_MAX_MESSAGES; i++)
    {
        syslog_entry_t *cur_entry = &slq_queue[i];
        cur_entry->available = 1;
        cur_entry->next = NULL;
        memset(cur_entry->message, '\0', SYSLOG_SINGLE_MESSAGE_LENGTH);
    }
}

static syslog_entry_t *slq_get_available(void)
{
    syslog_entry_t *to_return = NULL;
    for (size_t i = 0; i < SYSLOG_MAX_MESSAGES; i++)
    {
        syslog_entry_t *cur_entry = &slq_queue[i];
        if (cur_entry->available)
        {
            to_return = cur_entry;
            break;
        }
    }
    return to_return;
}

void slq_add_element(interpreter_status_t *statusEntry)
{
    if (statusEntry == NULL)
        return;

    interpreter_status_t *curStatus = statusEntry;
    // Avoid adding diagnostic of sys log itself
    if (strstr((char *)curStatus->command.rootCommand, "sys:log") != NULL)
        return;

    char message[SYSLOG_SINGLE_MESSAGE_LENGTH] = {'\0'};
    slq_craft_message(message, SYSLOG_SINGLE_MESSAGE_LENGTH, curStatus);

    syslog_entry_t *newEntry = slq_get_available();
    if (newEntry != NULL)
    {
        // Check if queue is full, implement FIFO
        if (slq_get_size() >= SYSLOG_MAX_MESSAGES)
        {
            slq_pop_element();
            newEntry = slq_get_available(); // Get a fresh available entry
        }

        if (newEntry != NULL)
        {
            newEntry->available = 0;
            newEntry->next = NULL;
            strncpy(newEntry->message, message, SYSLOG_SINGLE_MESSAGE_LENGTH - 1);
            newEntry->message[SYSLOG_SINGLE_MESSAGE_LENGTH - 1] = '\0';

            if (syslog_container.head == NULL)
            {
                // First element
                syslog_container.head = newEntry;
                syslog_container.tail = newEntry;
            }
            else
            {
                // Append to the end
                syslog_container.tail->next = newEntry;
                syslog_container.tail = newEntry;
            }
        }
    }
}

void slq_add_element_manual(char *msg)
{
    if (msg == NULL)
        return; // Safety check for null input

    syslog_entry_t *newEntry = slq_get_available();
    if (newEntry != NULL)
    {
        // Check if queue is full, implement FIFO
        if (slq_get_size() >= SYSLOG_MAX_MESSAGES)
        {
            slq_pop_element();
            newEntry = slq_get_available(); // Get a fresh available entry
        }

        if (newEntry != NULL)
        {
            newEntry->available = 0;
            newEntry->next = NULL;
            strncpy(newEntry->message, msg, SYSLOG_SINGLE_MESSAGE_LENGTH - 1);
            newEntry->message[SYSLOG_SINGLE_MESSAGE_LENGTH - 1] = '\0';

            if (syslog_container.head == NULL)
            {
                // First element
                syslog_container.head = newEntry;
                syslog_container.tail = newEntry;
            }
            else
            {
                // Append to the end
                syslog_container.tail->next = newEntry;
                syslog_container.tail = newEntry;
            }
        }
    }
}

void slq_pop_element(void)
{
    if (syslog_container.head != NULL)
    {
        syslog_entry_t *temp = syslog_container.head;
        if (syslog_container.head == syslog_container.tail)
        {
            // One element
            syslog_container.head = NULL;
            syslog_container.tail = NULL;
        }
        else
        {
            syslog_container.head = temp->next;
        }

        // Restore syslog_entry to original state
        temp->next = NULL;
        temp->available = 1;
        memset(temp->message, '\0', SYSLOG_SINGLE_MESSAGE_LENGTH);
    }
}

void slq_get_full_message(char *fullMessage, uint16_t size)
{
    char *curTermChar = TERM_CHAR;
    uint8_t curTermCharLen = strlen(curTermChar);
    syslog_entry_t *current = syslog_container.head;
    size_t currentLength = strlen(fullMessage); // Track current length to avoid repeated strlen calls

    while (current != NULL)
    {
        char *curMsg = current->message;
        size_t msgLen = strlen(curMsg);
        int remainingLength = size - currentLength - curTermCharLen - 1;
        if (remainingLength > 0)
        {
            size_t toCopy = (msgLen < remainingLength) ? msgLen : remainingLength;
            strncat(fullMessage, curMsg, toCopy);
            currentLength += toCopy;
        }
        current = current->next;
        slq_pop_element();
    }
    // Ensure there is enough space for the termination character
    if (size > currentLength)
    {
        strncat(fullMessage, curTermChar, size - currentLength - 1);
    }
}

uint16_t slq_get_total_length(void)
{
    syslog_entry_t *current = syslog_container.head;
    uint16_t totalLength = 0;
    while (current != NULL)
    {
        totalLength += strlen(current->message);
        current = current->next;
    }
    return totalLength;
}

uint8_t slq_get_size(void)
{
    syslog_entry_t *current = syslog_container.head;
    uint8_t size = 0;
    while (current != NULL)
    {
        size++;
        current = current->next;
    }
    return size;
}

void slq_craft_message(char *message, uint16_t length, interpreter_status_t *curStatus)
{
    // Bring in scope struct fields and pop element
    action_return_t action = curStatus->action_return;
    command_t command = curStatus->command;
    interpreter_flag_t curStatusFlag = curStatus->status;

    unsigned char (*args)[MAX_PARAM_LENGTH] = command.parameters;
    uint8_t argsCount = command.paramsCount;

    // Prepare args list
    char argsList[1024] = {'\0'};
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
