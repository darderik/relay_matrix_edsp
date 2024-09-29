#include "main.h"
#include "nanotimer.h"
/**
 * @brief Waits for the given number of 20ns periods to pass using timer 3
 * This applies only if APB1 is 50Mhz 1/50E-6 = 20ns
 * @param multiple The number of 20ns periods to wait
 */
void waitMultiple20ns(uint8_t multiple) {
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < multiple);
}