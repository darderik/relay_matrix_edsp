#include "main.h"

#ifndef FSM_H_
#define FSM_H_

typedef enum {
    INIT,
    IDLE,
    RECEIVE,
    INTERPRET,
    LOG,
    EXECUTE,
    FAIL,
    OUTPUT,
    PASSIVE
} state_t;
extern unsigned char* unparsed_command_queue[8];
extern state_t state;
extern uint8_t unparsed_command_counter;
state_t state_get();
void state_set(state_t newState);
void state_handler(UART_HandleTypeDef *huart,SPI_HandleTypeDef *hspi1);

#endif