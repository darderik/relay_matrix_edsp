#include "main.h"
#include "actions.h"
#define RSTBYTE 0b10101010
#define GR_OUTCOUNT 4

uint8_t checkParamsSelector(uint8_t curIn, uint8_t curOut, interpreter_status_t *int_status);
uint8_t checkResetStatus(relay_group_t *curGroup, interpreter_status_t *int_status);
void transmitSPI(relay_group_t *curGroup, uint8_t byteP, uint8_t isLatching);
void switch_commute(interpreter_status_t *int_status, uint8_t curIn, uint8_t curOut);
void switch_commute_exclusive(interpreter_status_t *int_status, uint8_t curIn, uint8_t curOut);
void switch_commute_reset(interpreter_status_t *int_status,uint8_t curIn, uint8_t curOut);
void switch_commute_reset_all(interpreter_status_t *int_status);
void switch_commute_handler(interpreter_status_t *int_status);

GPIO_PinState configAndRead(GPIO_TypeDef *GPIOx, uint16_t Pin);
