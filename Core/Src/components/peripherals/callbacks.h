#ifndef CALLBACKS_H_
#define CALLBACKS_H_
#include <stdint.h>
#include "config.h"

uint8_t is_query(unsigned char *command);
typedef struct prequeue_entry
{
    unsigned char command[MAX_COMMAND_LENGTH];
    uint8_t available;
} prequeue_entry_t;

extern prequeue_entry_t prequeue[MAX_COMMAND_QUEUE_SIZE];
void prequeue_init();
uint8_t prequeue_add(unsigned char *command);
void prequeue_remove(int index);
void prequeue_to_ucq();
uint8_t prequeue_has_elements();
#endif