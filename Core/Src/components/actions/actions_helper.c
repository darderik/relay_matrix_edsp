#include "actions.h"
#include <stdlib.h>
#include <string.h>

void action_return_constructor(action_return_t *instance)
{
    instance->status = ACTION_OK;
    instance->message = NULL;
    instance->toTransmit = 0;
}
void action_return_addMessage(action_return_t *instance, char *message,uint8_t toTransmit) {
    instance->toTransmit = toTransmit;
    uint8_t* msgPtr = malloc(strlen(message) + 1);
    strcpy((char *)msgPtr, message);
    instance->message = msgPtr;
}
void action_return_deconstructor(action_return_t *instance)
{
    free(instance->message);
}