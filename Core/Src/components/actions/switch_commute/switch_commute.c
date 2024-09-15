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

/**
 * @brief Switches the input and output of a relay. Does not alter previous state of other relays in same group.
 *
 * @param int_status The status of the interpreter.
 */

void switch_commute(interpreter_status_t *int_status)
{
    command_t commandData = int_status->command;
    for (uint8_t i = 0; i < commandData.paramsCount; i++)
    {
        unsigned char *curParam = commandData.parameters[i];
        // Relay matrix -> [letter, number] in,out eg: a1 commute a on out 1
        uint8_t curIn = (uint8_t)curParam[0] - 'a'; // Convert a-c to 0-3
        uint8_t curOut = (uint8_t)curParam[1] - 1;  // Convert 1-4 to 0-3
        if (curIn < 4 && curOut < 4)
        {
            relay_group_t curGroup = relayGroups[curIn];
            // Check rst is up
            uint8_t rstStatus = configAndRead(curGroup.gpio_port_nrst, curGroup.nrst_pin);
            if (rstStatus == GPIO_PIN_RESET)
            {
                // Critical failure
                int_status->action_return.status = ACTION_ERROR;
                strcpy(int_status->action_return.data, "TPL9201: No Power RST 0");
                state_set(FAIL);
                return;
            }
            // CS Goes low, we shall wait for the minimum time to set(10 ns,is even worth it?)
            HAL_GPIO_WritePin(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_RESET);
            uint8_t byteP = 0b01;
            if (SWITCH_COMMUTE_MODE == SWITCH_EMR)
            {
                // Assemble packet, DO NOT RESET PREVIOUS STATE IF BISTABLE
                byteP = byteP << curOut * 2;
                // Write TO SPI
                HAL_SPI_Transmit(&hspi1, &byteP, 1, HAL_MAX_DELAY);
                waitMultiple20ns(8);
                HAL_GPIO_WritePin(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_SET);
                HAL_Delay(8); // Debounce  relay
                // Reset, we have latching relays
                configAndWrite(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_RESET);
            }
            else if (SWITCH_COMMUTE_MODE == SWITCH_SSR)
            {
                byteP = oldByteSSR | (0b01 << 2 * curOut);
                oldByteSSR = byteP;
                // Write TO SPI
                HAL_SPI_Transmit(&hspi1, &byteP, 1, HAL_MAX_DELAY);
                // 160 ns for  latching
                waitMultiple20ns(8);
                HAL_GPIO_WritePin(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_SET);
            }
        }
    }
}
void switch_commute_exclusive(interpreter_status_t *int_status)
{
    command_t commandData = int_status->command;
    for (uint8_t i = 0; i < commandData.paramsCount; i++)
    {
        unsigned char *curParam = commandData.parameters[i];
        // Relay matrix -> [letter, number] in,out eg: a1 commute a on out 1
        uint8_t curIn = (uint8_t)curParam[0] - 'a'; // Convert a-c to 0-3
        uint8_t curOut = (uint8_t)curParam[1] - 1;  // Convert 1-4 to 0-3
        if (curIn < 4 && curOut < 4)
        {
            relay_group_t curGroup = relayGroups[curIn];
            // Check rst is up
            uint8_t rstStatus = configAndRead(curGroup.gpio_port_nrst, curGroup.nrst_pin);
            if (rstStatus == GPIO_PIN_RESET)
            {
                // Critical failure
                int_status->action_return.status = ACTION_ERROR;
                strcpy(int_status->action_return.data, "TPL9201: No Power RST 0");
                state_set(FAIL);
                return;
            }
            // CS Goes low, we shall wait for the minimum time to set(10 ns,is even worth it?)
            HAL_GPIO_WritePin(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_RESET);
            uint8_t rstByte = 0b10101010;
            uint8_t byteP = 0b01;
            if (SWITCH_COMMUTE_MODE == SWITCH_EMR)
            {
                byteP = byteP << curOut * 2;
                byteP = byteP | (rstByte - (byteP << 1));
                // Write TO SPI
                HAL_SPI_Transmit(&hspi1, &byteP, 1, HAL_MAX_DELAY);
                waitMultiple20ns(8);
                HAL_GPIO_WritePin(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_SET);
                HAL_Delay(8); // Debounce relay
                // Reset, we have latching relays
                configAndWrite(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_RESET);
            }
            else if (SWITCH_COMMUTE_MODE == SWITCH_SSR)
            {
                byteP = 0b01 << 2 * curOut;
                oldByteSSR = byteP;
                // Write TO SPI
                HAL_SPI_Transmit(&hspi1, &byteP, 1, HAL_MAX_DELAY);
                // 160 ns for  latching
                waitMultiple20ns(8);
                HAL_GPIO_WritePin(curGroup.gpio_port_ncs, curGroup.ncs_pin, GPIO_PIN_SET);
            }
        }
    }
}
GPIO_PinState configAndRead(GPIO_TypeDef *GPIOx, uint16_t Pin) {
    /*
    GPIO_InitTypeDef gpio;
    gpio.Pin = Pin;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &gpio); */
    HAL_GPIO_WritePin(GPIOx, Pin, GPIO_PIN_SET); //High impedance
    waitMultiple20ns(10);
    GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOx, Pin);
    return pinState;
}
void configAndWrite(GPIO_TypeDef *GPIOx, uint16_t Pin, GPIO_PinState PinState) {
    /*
    GPIO_InitTypeDef gpio;
    gpio.Pin = Pin;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW; 
    HAL_GPIO_Init(GPIOx, &gpio);*/
    HAL_GPIO_WritePin(GPIOx, Pin, PinState);
}