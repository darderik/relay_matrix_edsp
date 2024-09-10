#include "fsm.h"
#include "pins.h"
#include "main.h"
#include "config.h"
#include "commands.h"
#include "parser.h"
#include "queues.h"
#include "interpreter.h"

state_t state = INIT;

state_t state_get()
{
    return state;
}
void state_set(state_t newState)
{
    state = newState;
}
void state_handler(UART_HandleTypeDef *huart, SPI_HandleTypeDef *hspi)
{
    switch (state)
    {
    case INIT:
        // Turn on PSU TBD, high NRST NCS
        //HAL_GPIO_WritePin(PS_ON_PORT, PS_ON_PIN, GPIO_PIN_RESET);
        if (QUEUE_MODE == CPU)
        {
            HAL_UART_Receive_IT(huart, &rx_data_ptr, 1);
        }
        state_set(IDLE);
        break;
    case IDLE:
        if (unparsed_head != NULL)
        {
            state_set(INTERPRET);
        }
        else
        {
            // TBD
            HAL_Delay(100);
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
        state_set(IDLE);
        break;
    case FAIL:
        // Wait for SYS:LOG? Command
        if (ucq_findElem("SYS:LOG?") != NULL)
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