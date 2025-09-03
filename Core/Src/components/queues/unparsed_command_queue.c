#include "queues.h"
#include <stdlib.h>
#include "parser.h"
#include "fsm.h"
#include "callbacks.h"
#include "queues.h"
#include <string.h>
//--------------------Unparsed Command Queue Helper--------------------

void ucq_appendCommand(unsigned char *command, uint8_t countList)
{
    // Create a new entry
    unparsed_entry_t *newEntry = (unparsed_entry_t *)malloc(sizeof(unparsed_entry_t));
    strcpy((char *)newEntry->command, (char *)command);
    newEntry->next = NULL;
    // If the queue is empty, set the new entry as the head
    if (unparsed_list.head == NULL)
    {
        // One element
        unparsed_list.head = newEntry;
    }
    else
    {
        unparsed_list.tail->next = newEntry;
    }
    unparsed_list.tail = newEntry;
}
/**
 * @brief Adds a new command to the unparsed command queue. If the queue is already at
 *        its maximum size, the command is discarded and the system is put into the FAIL state.
 * @param command The command to be added to the queue
 */
uint8_t ucq_addElement(unsigned char *command)
{
    uint8_t countList = ucq_getQueueSize();
    if (!(countList >= MAX_COMMAND_QUEUE_SIZE))
    {
        char sys_log_cmd[32] = "sys:log?";
        strcat((char *)sys_log_cmd, TERM_CHAR);
        if (state_get() != FAIL)
        {
            ucq_appendCommand(command, countList);
        }
        else if (strcicmp((char *)command, sys_log_cmd) == 0)
        {
            ucq_appendCommand(command, countList);
        }
    }
    else
    {
        if (state_get() != FAIL)
        {
            char msg[64];
            snprintf(msg, sizeof(msg), "--CRITICAL--%s Command queue full,executing queued commands.%s", NEWLINE_STR, TERM_CHAR);
            sysLogQueue_addNodeManual(msg);
            state_set(INTERPRET);
        }
    }
    return countList >= MAX_COMMAND_QUEUE_SIZE ? 1 : 0;
}

/**
 * @brief Removes the first occurrence of the given command from the unparsed command queue
 * @param [in] command The command to be removed from the queue
 * @return 1 if the command was found and removed, 0 otherwise
 */
uint8_t ucq_removeElement(unsigned char *command)
{
    uint8_t removed = 0;
    unparsed_entry_t *current = unparsed_list.head;
    unparsed_entry_t *previous = NULL;

    while (current != NULL && strcmp((char *)current->command, (char *)command) != 0)
    {
        previous = current;
        current = current->next;
    }

    if (current != NULL)
    {
        removed = 1;
        if (previous == NULL)
        {
            // Remove the head
            unparsed_list.head = current->next;
        }
        else
        {
            previous->next = current->next;
            // Update tail
            if (current == unparsed_list.tail)
            {
                unparsed_list.tail = previous;
            }
        }
        free(current);
    }

    return removed;
}

/**
 * @brief Returns the number of elements in the unparsed command queue.
 *
 * @return The number of elements in the unparsed command queue.
 */
uint8_t ucq_getQueueSize()
{
    uint8_t count = 0;
    unparsed_entry_t *current = unparsed_list.head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

/**
 * @brief Removes the first element from the unparsed command queue.
 *
 * @details Pops the first element from the unparsed command queue, freeing its memory
 *          and updating the head of the queue.
 */
void ucq_popElement()
{
    if (unparsed_list.head != NULL)
    {
        unparsed_entry_t *temp = unparsed_list.head;
        if (unparsed_list.head == unparsed_list.tail)
        {
            // One element
            unparsed_list.head = NULL;
            unparsed_list.tail = NULL;
        }
        else
        {
            unparsed_list.head = temp->next;
        }
        free(temp);
    }
}

/**
 * @brief Finds an element in the unparsed command queue by its root command. First match.
 *
 * @param[in] rootcmd The root command to search for.
 *
 * @return The found element, or NULL if not found.
 */
unparsed_entry_t *ucq_findElem(char *rootcmd)
{
    unparsed_entry_t *found = NULL;
    unparsed_entry_t *curElem = unparsed_list.head;
    while (curElem != NULL)
    {
        if (strcicmp((char *)curElem->command, rootcmd) == 0)
        {
            found = curElem;
            break;
        }
        curElem = curElem->next;
    }
    return found;
}
// Function to clear the unparsed command queue
void ucq_clearQueue()
{
    while (unparsed_list.head != NULL)
    {
        ucq_popElement();
    }
}
