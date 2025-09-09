#ifndef SYS_LOG_QUEUE_H
#define SYS_LOG_QUEUE_H

#include "config.h"
#include <stdint.h>

typedef struct syslog_entry
{
    char message[SYSLOG_SINGLE_MESSAGE_LENGTH];
    struct syslog_entry *next;
    uint8_t available;
} syslog_entry_t;

typedef struct slq_container
{
    syslog_entry_t *head;
    syslog_entry_t *tail;
} slq_container_t;

// SLQ Function prototypes following consistent naming
void slq_init(void);
void slq_add_element(interpreter_status_t *statusEntry);
void slq_add_element_manual(char *msg);
void slq_pop_element(void);
void slq_get_full_message(char *fullMessage, uint16_t size);
uint16_t slq_get_total_length(void);
uint8_t slq_get_size(void);
void slq_craft_message(char *message, uint16_t length, interpreter_status_t *curStatus);

#endif