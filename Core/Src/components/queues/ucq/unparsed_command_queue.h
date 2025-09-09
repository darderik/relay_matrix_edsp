#ifndef UNPARSED_COMMAND_QUEUE_H
#include <stdint.h>
#include "config.h"
#define UNPARSED_COMMAND_QUEUE_H
//                      ---Unparsed Command Queue---

typedef struct unparsed_entry
{
    unsigned char command[MAX_COMMAND_LENGTH];
    struct unparsed_entry *next;
    uint8_t available;
} unparsed_entry_t;

typedef struct unparsed_list_container
{
    unparsed_entry_t *head;
    unparsed_entry_t *tail;
} unparsed_list_container_t;

extern unparsed_list_container_t ucq_container;
// UCQ Prototypes
void ucq_init(void);
uint8_t ucq_add_element(unsigned char *command);
unparsed_entry_t *ucq_get_available(void);
void ucq_pop_element(void);
unparsed_entry_t *ucq_find_element(char *rootcmd);
void ucq_clear_queue(void);

#endif
