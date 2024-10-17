#include "config.h"
#include "main.h"

//-----Switch commute Action------//
relay_group_t relayGroups[4] = {
    {'a', GPIOA, GPIOB, GPIO_PIN_12, GPIO_PIN_12,0},
    {'b', GPIOA, GPIOB, GPIO_PIN_9, GPIO_PIN_9,0},
    {'c', GPIOB, GPIOB, GPIO_PIN_6, GPIO_PIN_14,0},
    {'d', GPIOC, GPIOC, GPIO_PIN_5, GPIO_PIN_6,0}};
    