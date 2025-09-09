#include "interpreter.h"
#include "interpreter_status_queue.h"
#include "sys_log_queue.h"

interpreter_status_t isq_queue[ISQ_QUEUE_SIZE];
isq_container_t isq_container;

void isq_init()
{
    isq_container.head = NULL;
    isq_container.tail = NULL;
    // Set to 1 all available flags
    for (size_t i = 0; i < ISQ_QUEUE_SIZE; i++)
    {
        interpreter_status_t *cur_status = &isq_queue[i];
        cur_status->available = 1;
        cur_status->next = NULL;
        action_return_constructor(&(cur_status->action_return));
    }
}

interpreter_status_t *isq_add_element()
{
    interpreter_status_t *to_return = NULL;
    for (size_t i = 0; i < ISQ_QUEUE_SIZE; i++)
    {
        interpreter_status_t *cur_entry = &isq_queue[i];
        if (cur_entry->available)
        {
            to_return = cur_entry;
            cur_entry->available = 0;
            interpreter_status_constructor(to_return);
            if (isq_container.head == NULL)
            {
                // First element
                isq_container.head = to_return;
                isq_container.tail = to_return;
            }
            else
            {
                // Append to the end
                isq_container.tail->next = to_return;
                isq_container.tail = to_return;
            }
            break;
        }
    }
    return to_return;
}

void isq_pop_element()
{
    if (isq_container.head != NULL)
    {
        interpreter_status_t *temp = isq_container.head;
        if (isq_container.head == isq_container.tail)
        {
            // One element
            isq_container.head = NULL;
            isq_container.tail = NULL;
        }
        else
        {
            isq_container.head = temp->next;
        } // Add to syslog
        slq_add_element(temp);
        // Reset entry state for reuse (no need to free with static allocation)
        temp->next = NULL;
        temp->available = 1;
    }
}

uint8_t isq_get_size()
{
    interpreter_status_t *last = isq_container.head;
    uint8_t size = 0;
    while (last != NULL)
    {
        size++;
        last = last->next;
    }
    return size;
}
