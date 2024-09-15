#include "queues.h"
//--------------------Interpreter Status Queue Helper--------------------
void statusQueue_addElement(interpreter_status_t *status)
{
    for (uint8_t i = 0; i < MAX_STATUS_QUEUE_SIZE; i++)
    {   
        interpreter_status_t *curStatus = statusQueue[i];
        if (curStatus == NULL)
        {
            statusQueue[i] = status;
            break;
        }
    }
    
}
//Return index of first non null element from statusQueue array
int8_t statusQueue_getFirstElementIdx() {
    for (uint8_t i = 0; i < MAX_STATUS_QUEUE_SIZE; i++)
    {
        if (statusQueue[i] != NULL)
        {
            return i;
        }
    }
    return -1;
}
void statusQueue_removeSpecificElement(interpreter_status_t* toRemove)
{
    for (uint8_t i = 0; i < MAX_STATUS_QUEUE_SIZE; i++)
    {
        if (statusQueue[i] == toRemove)
        {
            interpreter_status_deconstructor(statusQueue[i]);
            statusQueue[i] = NULL;
            break;
        }
    }
    
}
void statusQueue_clear(){
    for (uint8_t i = 0; i < MAX_STATUS_QUEUE_SIZE; i++)
    {
        if (statusQueue[i] != NULL)
        {
            interpreter_status_deconstructor(statusQueue[i]);
            statusQueue[i] = NULL;
        }
    }
}
void statusQueue_popElement()
{
    if (statusQueue[0] != NULL)
    {
        interpreter_status_deconstructor(statusQueue[0]);
        for (uint8_t i = 0; i < MAX_STATUS_QUEUE_SIZE - 1; i++)
        {
            statusQueue[i] = statusQueue[i + 1];
        }
    }
}
