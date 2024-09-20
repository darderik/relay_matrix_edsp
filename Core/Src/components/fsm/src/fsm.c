#include "fsm.h"
#include "main.h"
#include "config.h"
#include "commands.h"
#include "parser.h"
#include "queues.h"
#include "interpreter.h"

state_t state = INIT;

void state_handler(UART_HandleTypeDef *huart, SPI_HandleTypeDef *hspi)
{
    switch (state)
    {
    case INIT:
        // Turn on PSU TBD, high NRST NCS
        HAL_GPIO_WritePin(PS_ON_GPIO, PS_ON_PIN, GPIO_PIN_RESET);
        HAL_Delay(PSU_TURNON_TIMEOUT); // 600ms for psu stabilization, or PB5 PWR_OK
        if (QUEUE_MODE == CPU)
        {
            HAL_UART_Receive_IT(huart, &rx_data_ptr, 1);
        }
        state_set(IDLE);
        break;

    case IDLE:
        if (unparsed_head != NULL)
        {
            // Command received
            state_set(INTERPRET);
        }
        else
        {
            state_set(LOG);
        }
        break;

    case INTERPRET:
        if (unparsed_head != NULL)
        {
            interpreter_status_t *int_status = interpretAndExecuteCommand();
            statusQueue_addElement(int_status);
            if (int_status->action_return.status == ACTION_ERROR)
            {
                state_set(FAIL);
            }
        }
        else
        {
            state_set(IDLE);
        }
        break;

    case LOG:
        sysLogQueue_populate();
        state_set(IDLE);
        break;
    case PASSIVE:
        if (unparsed_head != NULL)
        {
            state_set(INIT); // Power on again
        }
        else
        {
            HAL_Delay(100);
        }
        break;
    case FAIL:
        // Wait for SYS:LOG? Command
        if (ucq_findElem("sys:log?") != NULL)
        {
            // Exit fail state, and output everything
            state_set(IDLE);
        }
        else
        {
            HAL_Delay(100);
        }
        break;

    default:
        break;
    }
}
state_t state_get()
{
    return state;
}
void state_set(state_t newState)
{
    state = newState;
}