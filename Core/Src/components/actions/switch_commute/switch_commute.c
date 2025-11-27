#include "actions.h"
#include "parser.h"
#include "interpreter.h"
#include "command.h"
#include "queues.h"
#include "config.h"
#include "main.h"
#include "switch_commute.h"
#include "fsm.h"
#include "string.h"
#include <stdlib.h>
void switch_commute_status(interpreter_status_t *int_status)
{
    int_status->action_return.toTransmit = 1;
    // Check all relays and make AND
    uint8_t isPowered = 1;
    for (uint8_t i = 0; i < sizeof(relayGroups) / sizeof(relay_group_t); i++)
    {
        isPowered &= checkResetStatus(&relayGroups[i], int_status, 1);
        if (!isPowered)
        {
            break;
        }
    }
}
// SWITCH:COMMUTE --------------------------------------------------------------
void switch_commute_reset_group(interpreter_status_t *int_status)
{
    for (uint8_t i = 0; i < int_status->command.paramsCount; i++)
    {
        // ROOT:CMD A B C ...
        unsigned char *curParam = int_status->command.parameters[i];
        uint8_t curIn = (uint8_t)curParam[0] - 'a'; // Convert a-c to 0-3
        if (curIn >= sizeof(relayGroups) / sizeof(relay_group_t) || curIn < 0)
        {
            // Invalid args
            int_status->status = INTERPRETER_INVALID_ARGS;
        }
        else
        {
            relay_group_t *curGroup = &relayGroups[curIn];
            if (checkResetStatus(curGroup, int_status, 0))
            {
                switch (SWITCH_COMMUTE_MODE)
                {
                case SWITCH_SSR:
                    transmitSPI(curGroup, 0, 0);
                    break;
                case SWITCH_EMR:
                    transmitSPI(curGroup, RSTBYTE, 1);
                default:
                    break;
                }
                curGroup->statusByte = 0;
            }
        }
    }
}
void switch_commute_handler(interpreter_status_t *int_status)
{
    command_t commandData = int_status->command;
    uint8_t curIn, curOut;
    for (uint8_t i = 0; i < commandData.paramsCount; i++)
    {
        // ROOT:CMD A1 B2 C3 ... D1
        unsigned char *curParam = commandData.parameters[i];
        curIn = (uint8_t)curParam[0] - 'a'; // Convert a-c to 0-3
        curOut = curParam[1] - '0' - 1;     // Convert '1'-'4' to 0-3
        if (checkParamsSelector(curIn, curOut, int_status))
        {
            if (checkResetStatus(&relayGroups[curIn], int_status, 0))
            {
                // Maybe hash comparison?
                if (strcicmp((char *)commandData.rootCommand, "SWITCH:COMMUTE:EXCLUSIVE") == 0)
                {
                    switch_commute_exclusive(int_status, curIn, curOut);
                }
                else if (strcicmp((char *)commandData.rootCommand, "SWITCH:COMMUTE:RESET") == 0)
                {
                    switch_commute_reset(int_status, curIn, curOut);
                }
                else if (strcicmp((char *)commandData.rootCommand, "SWITCH:COMMUTE") == 0)
                {
                    switch_commute(int_status, curIn, curOut);
                }
                else if (strcicmp((char *)commandData.rootCommand, "SWITCH:COMMUTE:MONO") == 0)
                {
                    // Call reset all
                    switch_commute_reset_all(int_status);
                    // Now switch the requested relay(s)
                    switch_commute(int_status, curIn, curOut);
                }
            }
            else
            {
                // No power on relay
                int_status->action_return.status = ACTION_ERROR;
            }
        }
        else
        {
            // Invalid args
            int_status->status = INTERPRETER_INVALID_ARGS;
        }
    }
}

/**
 * @brief      SWITCH:COMMUTE:RESET:ALL command. This command is used to reset all the coils
 *             of the relay matrix. It takes no argument.
 *
 * @param[in]  int_status  Interpreter status structure pointer
 *
 * @return     None
 */
void switch_commute_reset_all(interpreter_status_t *int_status)
{
    for (size_t i = 0; i < sizeof(relayGroups) / sizeof(relay_group_t); i++)
    {
        relay_group_t *curGroup = &relayGroups[i];
        if (checkResetStatus(curGroup, int_status, 0))
        {
            switch (SWITCH_COMMUTE_MODE)
            {
            case SWITCH_SSR:
                transmitSPI(curGroup, 0, 0);
                break;
            case SWITCH_EMR:
                transmitSPI(curGroup, RSTBYTE, 1);
            default:
                break;
            }
            curGroup->statusByte = 0;
        }
    }
}

void switch_commute_byte_status(interpreter_status_t *int_status)
{
    char message[64] = {0};
    uint8_t statusByte;
    uint8_t representativeByte;
    relay_group_t *curGroup;

    for (size_t i = 0; i < sizeof(relayGroups) / sizeof(relay_group_t); i++)
    {
        curGroup = &relayGroups[i];
        statusByte = curGroup->statusByte;

        representativeByte = ((statusByte & 0b01) << 3) |
                             ((statusByte & 0b100) >> 0) |
                             ((statusByte & 0b10000) >> 3) |
                             ((statusByte & 0b1000000) >> 6);

        char curMsg[16] = {0};
        uint8_to_bin(representativeByte, curMsg);
        char trimmedMsg[5];
        strncpy(trimmedMsg, curMsg + 4, 4);
        trimmedMsg[4] = '\0';
        if (i > 0)
        {
            strncat(message, ",", sizeof(message) - strlen(message) - 1);
        }
        strncat(message, trimmedMsg, sizeof(message) - strlen(message) - 1);
    }

    action_return_addMessage(&(int_status->action_return), message, 1);
}
/**
 * @brief      SWITCH:COMMUTE command. This command is used to activate/deactivate coils
 *             of the relay board. The command takes as argument a single string of the form
 *             "a1", "b2", "c3", "d4", where "a", "b", "c", "d" are the relay group
 *             letters, and 1, 2, 3, 4 are the output numbers.
 *
 * @param[in]  int_status  Interpreter status structure pointer
 *
 * @return     None
 */
void switch_commute(interpreter_status_t *int_status, uint8_t curIn, uint8_t curOut)
{
    relay_group_t *curGroup = &relayGroups[curIn];
    uint8_t byteP = 0b01;
    switch (SWITCH_COMMUTE_MODE)
    {
    case SWITCH_EMR:
        byteP = byteP << curOut * 2;
        curGroup->statusByte |= byteP;
        transmitSPI(curGroup, byteP, 1);
        break;
    case SWITCH_SSR:
        byteP = curGroup->statusByte | (0b01 << 2 * curOut);
        curGroup->statusByte = byteP;
        transmitSPI(curGroup, byteP, 0);
        break;
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
void switch_commute_exclusive(interpreter_status_t *int_status, uint8_t curIn, uint8_t curOut)
{
    relay_group_t *curGroup = &relayGroups[curIn];
    // This byte is a set byte
    uint8_t byteP = 0b01;

    switch (SWITCH_COMMUTE_MODE)
    {
    case SWITCH_EMR:
        // If EMR, the exclusive mode translates in an OR bitwise between byteP and rstByte without a specific reset coil(all the others are reset)
        byteP = byteP << curOut * 2;
        // Assign statusByte before correcting it with reset coils
        curGroup->statusByte = byteP;
        byteP = byteP | (RSTBYTE - (byteP << 1));
        transmitSPI(curGroup, byteP, 1);
        break;
    case SWITCH_SSR:
        byteP = 0b01 << 2 * curOut;
        curGroup->statusByte = byteP;
        transmitSPI(curGroup, byteP, 0);
        break;
    default:
        break;
    }
}

/**
 * @brief      SWITCH:COMMUTE:RESET command. This command is used to reset specific coils
 *             of the relay matrix. The command takes as argument a single string of the form
 *             "a1", "b2", "c3", "d4", where "a", "b", "c", "d" are the relay group
 *             letters, and 1, 2, 3, 4 are the output numbers.
 *
 * @param[in]  int_status  Interpreter status structure pointer
 *
 * @return     None
 */
void switch_commute_reset(interpreter_status_t *int_status, uint8_t curIn, uint8_t curOut)
{
    relay_group_t *curGroup = &relayGroups[curIn];
    uint8_t byteP = 0b1;
    switch (SWITCH_COMMUTE_MODE)
    {
    case SWITCH_EMR:
        byteP = byteP << (curOut * 2 + 1);
        transmitSPI(curGroup, byteP, 1);
        curGroup->statusByte &= ~byteP >> 1;
        break;
    case SWITCH_SSR:
        byteP = byteP << curOut * 2;
        byteP = curGroup->statusByte & ~byteP;
        curGroup->statusByte = byteP;
        transmitSPI(curGroup, byteP, 0);
        break;
    default:
        break;
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
uint8_t checkParamsSelector(uint8_t curIn, uint8_t curOut, interpreter_status_t *int_status)
{
    uint8_t groupListSize = sizeof(relayGroups) / sizeof(relay_group_t);
    uint8_t chkCond = (curIn >= 0 && curIn <= groupListSize && curOut >= 0 && curOut < GR_OUTCOUNT);
    // Check non negative values
    if (!chkCond)
    {
        int_status->action_return.status = ACTION_INVALID_ARGS;
        action_return_addMessage(&(int_status->action_return), "SWITCH:COMMUTE->Invalid params", 0);
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
uint8_t checkResetStatus(relay_group_t *curGroup, interpreter_status_t *int_status, uint8_t queryMode)
{
    GPIO_PinState rstStatus = configAndRead(curGroup->gpio_port_nrst, curGroup->nrst_pin);
    if (rstStatus == GPIO_PIN_RESET)
    {
        int_status->action_return.status = ACTION_ERROR;
        char *msg = "TPL9201: ERR Power RST 0";
        action_return_addMessage(&(int_status->action_return), msg, queryMode ? 1 : 0);
        return 0;
    }
    else
    {
        char *msg = "TPL9201: OK Power RST 1";
        action_return_addMessage(&(int_status->action_return), msg, queryMode ? 1 : 0);
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
void transmitSPI(relay_group_t *curGroup, uint8_t byteP, uint8_t isLatching)
{
    HAL_GPIO_WritePin(curGroup->gpio_port_ncs, curGroup->ncs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &byteP, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(curGroup->gpio_port_ncs, curGroup->ncs_pin, GPIO_PIN_SET);
    if (isLatching)
    {
        HAL_Delay(30);         // Debounce, HFD2-012-S-L2 Datasheet
        manualReset(curGroup); // Open Drain OFF
        HAL_Delay(1);          // Delay before next word
    }
}
/**
 * @brief      Manual reset of the relay group. This function is used to reset all the
 *            coils of the relay group. Sends 00000000 over SPI.
 * @param[in]  curGroup    Relay group structure pointer
 */
void manualReset(relay_group_t *curGroup)
{
    uint8_t null_msg = 0;
    HAL_GPIO_WritePin(curGroup->gpio_port_ncs, curGroup->ncs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &null_msg, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(curGroup->gpio_port_ncs, curGroup->ncs_pin, GPIO_PIN_SET);
}
GPIO_PinState configAndRead(GPIO_TypeDef *GPIOx, uint16_t Pin)
{
    HAL_GPIO_WritePin(GPIOx, Pin, GPIO_PIN_SET); // H-Z
    HAL_Delay(1);
    GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOx, Pin);
    return pinState;
}
void uint8_to_bin(uint8_t num, char *str)
{
    for (int i = 7; i >= 0; i--)
    {
        str[7 - i] = (num & (1 << i)) ? '1' : '0';
    }
    str[8] = '\0'; // Null terminator
}