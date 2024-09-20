#include "config.h"
#include "interpreter.h"
#include "main.h"

#ifndef QUEUES_H
#define QUEUES_H

typedef struct unparsed_entry {
    unsigned char command[MAX_COMMAND_LENGTH];
    struct unparsed_entry* next;
} unparsed_entry_t;

extern unsigned char rx_data_ptr;
extern unsigned char rx_buffer[MAX_COMMAND_LENGTH];
extern unparsed_entry_t *unparsed_head;
extern interpreter_status_t *statusQueue[MAX_STATUS_QUEUE_SIZE];
extern unsigned char sysLogMessage[MAX_SYSLOG_BUFFER_SIZE];

void ucq_addElement(unsigned char *command);
int8_t ucq_removeElement(unsigned char *keyCommand);
uint8_t ucq_getQueueSize();
void ucq_popElement();
void ucq_clearQueue();
unparsed_entry_t* ucq_findElem(char* rootcmd);
void statusQueue_addElement(interpreter_status_t *status);
void statusQueue_removeSpecificElement(interpreter_status_t* toRemove);
int8_t statusQueue_getFirstElementIdx();
void sysLogQueue_populate();
void statusQueue_clear();
#endif