#include "fsm.h"
#include "main.h"
#include "config.h"
#include "command.h"
#include "parser.h"
#include "queues.h"
#include "interpreter.h"

state_t state = INIT;
static uint8_t mcu_firstRun = 1;
/**
 * @brief This function is the state machine of the FSM.
 *
 *        This function is responsible for changing the state of the system
 *        according to the current state and the events that occur.
 *
 *        @param huart Pointer to a UART_HandleTypeDef structure
 *        @param hspi Pointer to a SPI_HandleTypeDef structure
 */
void state_handler(UART_HandleTypeDef *huart, SPI_HandleTypeDef *hspi)
{
    switch (state)
    {
    case INIT:
        state_set(IDLE);
        // Turn on PSU by pulling down PC9
        HAL_GPIO_WritePin(PS_ON_GPIO, PS_ON_PIN, GPIO_PIN_RESET);
        if (PSU_CHECK_MODE == PSU_TIMEOUT)
        {
            HAL_Delay(PSU_TURNON_TIMEOUT); // 600ms for psu stabilization, or PB5 PWR_OK
        }
        else
        {
            sys_boot_check(); // Internal command, not callable by operator
        }

        if (mcu_firstRun)
        {
            mcu_firstRun = 0;
            if (QUEUE_MODE == DMA)
            {
                HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, MAX_COMMAND_LENGTH);
            }
            else
            {
                HAL_UART_Receive_IT(huart, &rx_data_ptr, 1);
            }
        }
        break;

    case IDLE:
        // Switch to interpret state if there is a command
        if (unparsed_list.head != NULL)
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
        if (unparsed_list.head != NULL)
        {
            interpreter_status_t *int_status = (interpreter_status_t *)malloc(sizeof(interpreter_status_t));
            interpreter_status_constructor(int_status);
            interpretAndExecuteCommand(int_status);
            statusQueue_addElement(int_status);
            if (int_status->action_return.status == ACTION_ERROR)
            {
                state_set(FAIL);
            }
        }
        else if (statusQueue_getSize() != 0)
        {
            state_set(IDLE);
        }
        break;

    case LOG:
        sysLogQueue_populate();
        state_set(IDLE);
        break;
    case PASSIVE:
        if (unparsed_list.head != NULL)
        {
            state_set(INIT); // Power on again
        }
        else
        {
            HAL_Delay(5);
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
            HAL_Delay(5);
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
void state_get_label(char *formattedStr)
{
    static char *labels[] = {"INIT", "IDLE", "INTERPRET", "LOG", "FAIL", "PASSIVE"};
    strcpy(formattedStr, labels[state]);
}
void state_set(state_t newState)
{
    state = newState;
}
uint8_t waitForPin(GPIO_TypeDef *port, uint16_t pin, uint32_t timeout)
{
    uint32_t ctr = 0;
    uint8_t toReturn;
    while (!HAL_GPIO_ReadPin(port, pin) && ctr <= timeout)
    {
        HAL_Delay(1);
        ctr++;
    }
    toReturn = ctr <= timeout ? 1 : 0;
    return toReturn;
}
void sys_boot_check()
{
    if (!waitForPin(PWR_OK_GPIO, PWR_OK_PIN, PSU_TURNON_TIMEOUT))
    {
        // PWR_OK Not reached, fail state
        unsigned char msg[96];
        snprintf((char *)msg, sizeof(msg), "sys:boot->PWR_OK not reached.ATX PSU Malfunctioning.Stopping..%s", TERM_CHAR);
        HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen((char *)msg), HAL_MAX_DELAY);
        state_set(FAIL);
    }
}