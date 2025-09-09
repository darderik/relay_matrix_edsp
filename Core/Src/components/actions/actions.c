#include "actions.h"
#include <stdlib.h>
#include <string.h>

void action_return_constructor(action_return_t *instance)
{
    instance->status = ACTION_OK;
    memset(instance->message, 0, ACTION_MSG_LENGTH);
    instance->toTransmit = 0;
}
void action_return_addMessage(action_return_t *instance, char *message, uint8_t toTransmit)
{
    instance->toTransmit = toTransmit;
    uint8_t msg_length = strlen(message) < ACTION_MSG_LENGTH ? strlen(message) : ACTION_MSG_LENGTH - 1;
    strncpy(instance->message, (char *)message, msg_length);
    if (msg_length > ACTION_MSG_LENGTH - 2)
    {
        // Append null terminator
        instance->message[ACTION_MSG_LENGTH - 1] = '\0';
    }
}