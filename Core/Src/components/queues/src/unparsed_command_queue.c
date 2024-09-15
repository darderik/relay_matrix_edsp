#include "queues.h"
#include <stdlib.h>
#include "parser.h"
//--------------------Unparsed Command Queue Helper--------------------
// Function to add an element to the unparsed command queue, ucq unparsed command queue
void ucq_addElement(unsigned char *command)
{
    if (!(ucq_getQueueSize() >= MAX_COMMAND_QUEUE_SIZE))
    {
    // Create a new entry
    unparsed_entry_t *newEntry = (unparsed_entry_t *)malloc(sizeof(unparsed_entry_t));
    strcpy((char *)newEntry->command, (char *)command);
    newEntry->next = NULL;
    // If the queue is empty, set the new entry as the head
    if (unparsed_head == NULL)
    {
         unparsed_head = newEntry;
    }
    else
    {
        unparsed_entry_t *current = unparsed_head;
        while (current->next != NULL)
        {
            current = current->next;
        } // End of list reached
        current->next = newEntry;
    }
    }
}

// Function to remove specified element from the unparsed command queue, -1 no match, >0 match
int8_t ucq_removeElement(unsigned char *keyCommand)
{
    int8_t removed = -1;
    uint8_t count = 0;
    // If the queue is empty, do nothing
    if (unparsed_head != NULL)
    {
        unparsed_entry_t *current = unparsed_head;
        while (current->command != keyCommand && current != NULL)
        {
            count++;
            current = current->next;
        }
        if (current != NULL)
        {
            removed = 1;
            // If the head is the element to be removed
            if (current == unparsed_head)
            {
                unparsed_head = NULL;
            }
            else
            {
                unparsed_entry_t *previous = unparsed_head;
                while (previous->next != current)
                {
                    previous = previous->next;
                }
                previous->next = current->next;
            }
            free(current->command);
            free(current);
        }
    }
    return removed;
}

// Function to get the number of elements in the unparsed command queue
uint8_t ucq_getQueueSize()
{
    uint8_t count = 0;
    unparsed_entry_t *current = unparsed_head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

// Function to remove the first element from the unparsed command queue
void ucq_popElement()
{
    if (unparsed_head != NULL)
    {
        unparsed_entry_t *temp = unparsed_head;
        unparsed_head = unparsed_head->next;
        free(temp->command);
        free(temp);
    }
}
// Function to clear the unparsed command queue
unparsed_entry_t* ucq_findElem(char* rootcmd)
{
    unparsed_entry_t *found=NULL;
    unparsed_entry_t *curElem = unparsed_head;
    while (curElem != NULL)
    {
        if (strcicmp((char*)curElem->command, rootcmd) == 0)
        {
            found = curElem;
            break;
        }
    }
    return found;
}
// Function to clear the unparsed command queue
void ucq_clearQueue()
{
    while (unparsed_head != NULL)
    {
        ucq_popElement();
    }
}
