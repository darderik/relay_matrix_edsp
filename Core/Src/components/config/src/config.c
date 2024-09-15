#include "config.h"
#include "main.h"

//-----Switch commute-----
const relay_group_t relayGroups[4] = {
    {'a', GPIOA, GPIOB, GPIO_PIN_12, GPIO_PIN_12},
    {'b', GPIOA, GPIOB, GPIO_PIN_9, GPIO_PIN_9},
    {'c', GPIOB, GPIOB, GPIO_PIN_6, GPIO_PIN_14},
    {'d', GPIOC, GPIOC, GPIO_PIN_5, GPIO_PIN_6}};