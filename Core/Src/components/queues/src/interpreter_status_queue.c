#include "queues.h"
#include <stdlib.h>
//--------------------Interpreter Status Queue Helper--------------------
void statusQueue_addElement(interpreter_status_t *status)
{
    if (!(statusQueue_getSize() >= MAX_STATUS_QUEUE_SIZE))
    {
        // Space left
        interpreter_status_entry_t *newEntry = (interpreter_status_entry_t *)malloc(sizeof(interpreter_status_entry_t));
        newEntry->status = *status;
        newEntry->next = NULL;

        if (statusQueue_list.head == NULL)
        {
            // First element
            statusQueue_list.head = newEntry;
        }
        else
        {
            statusQueue_list.tail->next = newEntry;
        }
        statusQueue_list.tail = newEntry;
    }
    else
    {
        // FIFO Logic, the first status will be deleted. Force append
        statusQueue_popElement();
        statusQueue_addElement(status);
    }
}

uint8_t statusQueue_getSize()
{
    uint8_t countList = 0;
    interpreter_status_entry_t *current = statusQueue_list.head;
    while (current != NULL)
    {
        countList++;
        current = current->next;
    }
    return countList;
}
void statusQueue_popElement()
{
    interpreter_status_entry_t *oldHead = statusQueue_list.head;
    interpreter_status_entry_t *curTail = statusQueue_list.tail;

    if (oldHead != NULL)
    {
        if (oldHead == curTail)
        {
            // Only one element
            free(statusQueue_list.head);
            statusQueue_list.head = NULL;
            statusQueue_list.tail = NULL;
        }
        else
        {
            statusQueue_list.head = oldHead->next;
            interpreter_status_deconstructor(&(oldHead->status));
        }
    }
}
