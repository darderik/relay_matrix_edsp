#include "queues.h"
#include <stdlib.h>
#include <inttypes.h>
#include "interpreter.h"
#include <string.h>
#include "parser.h"
// UART Section
unsigned char rx_data_ptr;
unsigned char rx_buffer[MAX_COMMAND_LENGTH];

//--------------------Unparsed Command Queue --------------------
// Head for unparsed command queue
unparsed_entry_t *unparsed_head = NULL;
// ---------------- Interpreter Status Queue -------------------
interpreter_status_t *statusQueue[MAX_STATUS_QUEUE_SIZE] = {NULL};
// ---------------- Sys log messages -------------------
unsigned char sysLogMessage[MAX_SYSLOG_BUFFER_SIZE] = {'\0'};



