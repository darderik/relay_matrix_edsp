#include "callbacks.h"
#include "config.h"
#include "queues.h"
#include "cmsis_gcc.h" // For __get_PRIMASK, __disable_irq, __enable_irq
#include <string.h>
#include <stdlib.h>

prequeue_entry_t prequeue[MAX_COMMAND_QUEUE_SIZE];
void prequeue_init()
{
    for (int i = 0; i < MAX_COMMAND_QUEUE_SIZE; i++)
    {
        prequeue[i].available = 1;                          // Mark all entries as available
        memset(prequeue[i].command, 0, MAX_COMMAND_LENGTH); // Clear command buffer
    }
}

uint8_t prequeue_add(unsigned char *command)
{
    for (int i = 0; i < MAX_COMMAND_QUEUE_SIZE; i++)
    {
        if (prequeue[i].available)
        {
            strncpy((char *)prequeue[i].command, (char *)command, MAX_COMMAND_LENGTH - 1);
            prequeue[i].command[MAX_COMMAND_LENGTH - 1] = '\0'; // Ensure null termination
            prequeue[i].available = 0;                          // Mark as occupied
            return 1;
        }
    }
    return 0; // Queue full
}
void prequeue_to_ucq()
{
    for (int i = 0; i < MAX_COMMAND_QUEUE_SIZE; i++)
    {
        if (!prequeue[i].available)
        {
            ucq_add_element(prequeue[i].command);
            prequeue_remove(i);
            return;
        }
    }
}
void prequeue_remove(int index)
{ // Disable interrupts
    if (index >= 0 && index < MAX_COMMAND_QUEUE_SIZE)
    {
        prequeue[index].available = 1;                          // Mark as available
        memset(prequeue[index].command, 0, MAX_COMMAND_LENGTH); // Clear command buffer
    }
}

uint8_t prequeue_has_elements()
{
    for (int i = 0; i < MAX_COMMAND_QUEUE_SIZE; i++)
    {
        if (!prequeue[i].available)
        {
            return 1; // Found an occupied entry
        }
    }
    return 0; // No occupied entries found
}