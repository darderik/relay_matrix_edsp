#include "actions.h"
#include "interpreter.h"
#include "commands.h"
#include "queues.h"
#include "config.h"
#include "main.h"
#include "switch_commute.h"
#include "nanotimer.h"
#include "fsm.h"
#include "string.h"

// SWITCH:COMMUTE --------------------------------------------------------------

uint8_t oldByteSSR = 0;

void switch_commute(interpreter_status_t *int_status)
{
    command_t commandData = int_status->command;
    for (uint8_t i = 0; i < commandData.paramsCount; i++)
    {
        unsigned char *curParam = commandData.parameters[i];
        uint8_t curIn, curOut;

        if (checkParamsSelector(curParam, &curIn, &curOut,int_status ) && checkResetStatus(relayGroups[curIn], int_status))
        {

            relay_group_t curGroup = relayGroups[curIn];

            uint8_t byteP = 0b01;
            if (SWITCH_COMMUTE_MODE == SWITCH_EMR)
            {
                byteP = byteP << curOut * 2;
                transmitSPI(curGroup, byteP, 1);
            }
            else if (SWITCH_COMMUTE_MODE == SWITCH_SSR)
            {
                byteP = oldByteSSR | (0b01 << 2 * curOut);
                oldByteSSR = byteP;
                transmitSPI(curGroup, byteP, 0);
            }
        }
    }
}

/**
 * @brief      SWITCH:COMMUTE:EXCLUSIVE command. This command is equivalent to
 *             SWITCH:COMMUTE; but when a coil is activated, all the other coils are reset.
 *
 * @param[in]  int_status  Interpreter status structure pointer
 *
 * @return     None
 */
void switch_commute_exclusive(interpreter_status_t *int_status)
{
    command_t commandData = int_status->command;
    for (uint8_t i = 0; i < commandData.paramsCount; i++)
    {
        unsigned char *curParam = commandData.parameters[i];
        uint8_t curIn, curOut;

        if (checkParamsSelector(curParam, &curIn, &curOut,int_status) && checkResetStatus(relayGroups[curIn], int_status))
        {
            relay_group_t curGroup = relayGroups[curIn];
            // This byte activate all reset coils of relays
            uint8_t rstByte = 0b10101010;
            uint8_t byteP = 0b01;

            if (SWITCH_COMMUTE_MODE == SWITCH_EMR)
            {
                // If EMR, the exclusive mode translates in an OR bitwise between byteP and rstByte without a specific reset coil
                byteP = byteP << curOut * 2;
                byteP = byteP | (rstByte - (byteP << 1));
                transmitSPI(curGroup, byteP, 1);
            }
            else if (SWITCH_COMMUTE_MODE == SWITCH_SSR)
            {
                byteP = 0b01 << 2 * curOut;
                oldByteSSR = byteP;
                transmitSPI(curGroup, byteP, 0);
            }
        }
    }
}

/**
 * @brief      Check the validity of the parameters for the SWITCH:COMMUTE command
 *             and convert them to the corresponding values.
 *
 * @param[in]  curParam    Current parameter to be checked
 * @param[out] curIn       Input pin number
 * @param[out] curOut      Output pin number
 * @param[in]  int_status  Interpreter status structure pointer
 *
 * @return     0 if the parameters are invalid, 1 otherwise
 */
uint8_t checkParamsSelector(unsigned char *curParam, uint8_t *curIn, uint8_t *curOut, interpreter_status_t *int_status)
{
    uint8_t chkCond = !(curParam[0] < 'a' || curParam[0] > 'c' || curParam[1] < 1 || curParam[1] > 4);
    // Check non negative values
    if (chkCond)
    {
        *curIn = (uint8_t)curParam[0] - 'a'; // Convert a-c to 0-3
        *curOut = (uint8_t)curParam[1] - 1;  // Convert 1-4 to 0-3
    }
    else
    {
        int_status->action_return.status = ACTION_ERROR;
        strcpy((char*)int_status->action_return.data, "SWITCH:COMMUTE->Invalid params");
    }
    return chkCond;
}

/**
 * @brief      Check the status of the Reset pin of the relay group. If the pin is
 *             low, it means the relay is not powered and the command is aborted
 *             with an error.
 *
 * @param[in]  curGroup    Relay group structure pointer
 * @param[in]  int_status  Interpreter status structure pointer
 *
 * @return     0 if the relay is not powered, 1 otherwise
 */
uint8_t checkResetStatus(relay_group_t curGroup, interpreter_status_t *int_status)
{
    GPIO_PinState rstStatus = configAndRead(curGroup.gpio_port_nrst, curGroup.nrst_pin);
    if (rstStatus == GPIO_PIN_RESET)
    {
        int_status->action_return.status = ACTION_ERROR;
        strcpy((char*)int_status->action_return.data, "TPL9201: No Power RST 0");
        state_set(FAIL);
        return 0;
    }
    return 1;
}

/**
 * @brief      Transmit a byte over SPI to the relay group.
 *
 * @param[in]  curGroup    Relay group structure pointer
 * @param[in]  byteP       Byte to be transmitted
 * @param[in]  isLatching  Flag indicating whether the byte is a latching coil command or not
 *
 * @details    This function implements a SPI transaction to the relay group. It first
 *             sets the NCS pin low, transmits the byte, waits for the transmission delay
 *             and then sets the NCS pin high. If isLatching is set, it waits an additional
 *             8ms for the coil to latch and then resets the RST pin.
 */
void transmitSPI(relay_group_t curGroup, uint8_t byteP, uint8_t isLatching)
{
    HAL_GPIO_WritePin(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &byteP, 1, HAL_MAX_DELAY);
    waitMultiple20ns(8); // Wait for transmit delay
    HAL_GPIO_WritePin(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_SET);
    if (isLatching)
    {
        HAL_Delay(8); // Debounce
        HAL_GPIO_WritePin(curGroup.gpio_port_nrst, curGroup.nrst_pin, GPIO_PIN_RESET);
    }
}

GPIO_PinState configAndRead(GPIO_TypeDef *GPIOx, uint16_t Pin)
{
    HAL_GPIO_WritePin(GPIOx, Pin, GPIO_PIN_SET); // H-Z
    waitMultiple20ns(10);
    GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOx, Pin);
    return pinState;
}
