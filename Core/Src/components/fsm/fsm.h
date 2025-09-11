#include "main.h"

#ifndef FSM_H_
#define FSM_H_

typedef enum
{
    INIT,
    IDLE,
    INTERPRET,
    LOG,
    FAIL,
    PASSIVE,
    MIRROR
} state_t;
void state_handler(UART_HandleTypeDef *huart, SPI_HandleTypeDef *hspi1);
uint8_t waitForPin(GPIO_TypeDef *port, uint16_t pin, uint32_t timeout);
void state_get_label(char *formattedStr);
void state_set(state_t newState);
state_t state_get();
void sys_boot_check();
#endif