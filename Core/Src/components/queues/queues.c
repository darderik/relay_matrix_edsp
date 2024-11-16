#include "queues.h"
#include <stdlib.h>
#include <inttypes.h>
#include "interpreter.h"
#include <string.h>
#include "parser.h"


//---UART RX Buffer---
unsigned char rx_data_ptr;
unsigned char rx_buffer[MAX_COMMAND_LENGTH+1];

//--------------------Unparsed Command Queue --------------------
// Head for unparsed command queue
unparsed_list_container_t unparsed_list;
//unparsed_entry_t *unparsed_head = unparsed_list.head;

// ---------------- Interpreter Status Queue -------------------
interpreter_status_t *statusQueue[MAX_STATUS_QUEUE_SIZE] = {NULL};
interpreter_status_list_container_t statusQueue_list;

// ---------------- Sys log messages -------------------
unsigned char sysLogMessage[MAX_SYSLOG_BUFFER_SIZE] = {'\0'};



