#include "config.h"
#include "interpreter.h"
#include "main.h"

#ifndef QUEUES_H
#define QUEUES_H
#include "unparsed_command_queue.h"
extern unsigned char rx_data_ptr;
extern unsigned char rx_buffer[MAX_COMMAND_LENGTH + 1];
extern unsigned char rx_buffer_temp[MAX_COMMAND_LENGTH + 1];

#endif