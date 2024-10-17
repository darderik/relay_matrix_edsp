#include "config.h"
#include "interpreter.h"
#include "main.h"

#ifndef QUEUES_H
#define QUEUES_H

extern unsigned char rx_data_ptr;
extern unsigned char rx_buffer[MAX_COMMAND_LENGTH+1];
//---Unparsed Command Queue---

/**
 * @struct unparsed_entry
 *
 * @brief Structure used to store an unparsed command in a linked list
 *
 * @param command    The command to be stored
 * @param next       The next element in the list
 */
typedef struct unparsed_entry {
    unsigned char command[MAX_COMMAND_LENGTH];
    struct unparsed_entry* next;
} unparsed_entry_t;

/**
 * @struct unparsed_list_container
 *
 * @brief Structure used to store a linked list of unparsed commands
 *
 * @param head       The first element in the list
 * @param tail       The last element in the list
 */
typedef struct unparsed_list_container {
    unparsed_entry_t* head;
    unparsed_entry_t* tail;
} unparsed_list_container_t;

/**
 * @brief Structure used to store an interpreter status in a linked list
 *
 * @param status     The interpreter status to store
 * @param next       The next element in the list
 */
typedef struct interpreter_status_entry {
    interpreter_status_t status;
    struct interpreter_status_entry* next;
} interpreter_status_entry_t;

typedef struct interpreter_status_list_container {
    interpreter_status_entry_t* head;
    interpreter_status_entry_t* tail;
} interpreter_status_list_container_t;

//extern unparsed_entry_t *unparsed_head;
extern unparsed_list_container_t unparsed_list;
extern interpreter_status_list_container_t statusQueue_list;
extern unsigned char sysLogMessage[MAX_SYSLOG_BUFFER_SIZE];

//---Unparsed Command Queue Helper---
uint8_t ucq_addElement(unsigned char *command);
uint8_t ucq_removeElement(unsigned char *keyCommand);
uint8_t ucq_getQueueSize();
void ucq_popElement();
void ucq_clearQueue();
unparsed_entry_t* ucq_findElem(char* rootcmd);

//---Interpreter Status Queue Helper---
void statusQueue_addElement(interpreter_status_t *status);
void statusQueue_popElement();
uint8_t statusQueue_getSize();

//---Sys Log Queue Helper---
void sysLogQueue_populate();
void sysLogQueue_addMessage(char *message);
#endif