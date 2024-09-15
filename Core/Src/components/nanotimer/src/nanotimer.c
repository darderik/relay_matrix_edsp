#include "main.h"
#include "nanotimer.h"
void waitMultiple20ns(uint8_t multiple) {
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < multiple);
}