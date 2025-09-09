#include "queues.h"
#include <stdlib.h>
#include "parser.h"
#include "fsm.h"
#include "callbacks.h"
#include "sys_log_queue.h"
//--------------------Unparsed Command Queue Helper--------------------
unparsed_entry_t ucq_queue[MAX_COMMAND_QUEUE_SIZE];
unparsed_list_container_t ucq_container;
char sys_log_cmd[32] = "sys:log?";
void ucq_init()
{
    ucq_container.head = 0;
    ucq_container.tail = 0;
    strcat(sys_log_cmd, TERM_CHAR);
    for (size_t i = 0; i < MAX_COMMAND_QUEUE_SIZE; i++)
    {
        unparsed_entry_t *cur_ucq_entry = &ucq_queue[i];
        cur_ucq_entry->available = 1;
        cur_ucq_entry->next = NULL;
        memset(cur_ucq_entry->command, '\0', MAX_COMMAND_LENGTH);
    }
}

uint8_t ucq_add_element(unsigned char *command)
{
    uint8_t report = 0;
    unparsed_entry_t *ucq_available = ucq_get_available();
    if (ucq_available != NULL)
    {
        if (state_get() != FAIL || strcicmp((char *)command, sys_log_cmd) == 0)
        {
            ucq_available->available = 0;
            ucq_available->next = NULL;
            strncpy((char *)ucq_available->command, (char *)command, MAX_COMMAND_LENGTH - 1);
            ucq_available->command[MAX_COMMAND_LENGTH - 1] = '\0';
            if (ucq_container.head == 0)
            { // Empty list
                ucq_container.head = ucq_available;
                ucq_container.tail = ucq_available;
            }
            else
            {
                unparsed_entry_t *last_entry = ucq_container.tail;
                last_entry->next = ucq_available;
                ucq_container.tail = ucq_available;
            }
        }
        report = 1;
    }
    else if (state_get() != FAIL)
    {
        char msg[82];
        snprintf(msg, sizeof(msg), "--CRITICAL--%s Command queue full,executing queued commands.%s", NEWLINE_STR, TERM_CHAR);
        slq_add_element_manual(msg);
        state_set(INTERPRET);
    }
    return report;
}

/**
 * @brief Returns the first available entry in the unparsed command queue.
 *
 * @return The first available entry, or NULL if none available.
 */
unparsed_entry_t *ucq_get_available()
{
    unparsed_entry_t *to_return = NULL;
    for (size_t i = 0; i < MAX_COMMAND_QUEUE_SIZE; i++)
    {
        unparsed_entry_t *cur_entry = &ucq_queue[i];
        if (cur_entry->available)
        {
            to_return = cur_entry;
            break;
        }
    }
    return to_return;
}

void ucq_pop_element()
{
    if (ucq_container.head != NULL)
    {
        unparsed_entry_t *temp = ucq_container.head;
        if (ucq_container.head == ucq_container.tail)
        {
            // One element
            ucq_container.head = NULL;
            ucq_container.tail = NULL;
        }
        else
        {
            ucq_container.head = temp->next;
        }

        // Restore unparsed_entry to original state
        temp->next = NULL;
        temp->available = 1;
    }
}

/**
 * @brief Finds an element in the unparsed command queue by its root command. First match.
 *
 * @param[in] rootcmd The root command to search for.
 *
 * @return The found element, or NULL if not found.
 */
unparsed_entry_t *ucq_find_element(char *rootcmd)
{
    unparsed_entry_t *found = NULL;
    unparsed_entry_t *curElem = ucq_container.head;
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
void ucq_clear_queue()
{
    while (ucq_container.head != NULL)
    {
        ucq_pop_element();
    }
}
