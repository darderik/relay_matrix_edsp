#include "queues.h"
#include <stdlib.h>
#include <inttypes.h>
#include "interpreter.h"
#include <string.h>
#include "parser.h"

//---UART RX Buffer---
unsigned char rx_data_ptr;
unsigned char rx_buffer[MAX_COMMAND_LENGTH + 1];
unsigned char rx_buffer_temp[MAX_COMMAND_LENGTH + 1];
