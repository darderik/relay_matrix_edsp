/**
 * @file      actions.c
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      04-09-2024
 * @copyright 2024, your company / association / school
 */

#include "actions.h"
#include "interpreter.h"
#include "commands.h"
#include "queues.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

// Here are defined all the commands and all the behaviour associated
const command_table_entry_t command_table[] = {
    {(unsigned char *)"switch:commute", switch_commute},
    {(unsigned char *)"sys:log?", sys_log}};
uint8_t command_table_size()
{
    uint8_t elemCount = sizeof(command_table) / sizeof(command_table[0]);
    return elemCount;
}

// SWITCH:COMMUTE --------------------------------------------------------------
//  TBD Use pins.h
GPIO_TypeDef *GPIOPortsIn[4] = {GPIOA, GPIOA, GPIOB, GPIOB};
uint16_t GPIOPinsIn[4] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_1}; // CS Pins for selecting the bank
uint8_t bankRelayStatus[4] = {0x00, 0x00, 0x00, 0x00};
/**
 * @brief Switches the input and output of a relay. Does not alter previous state of other relays.
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
        uint8_t curInBank = (uint8_t)curParam[0] - 97;
        uint8_t curOutBank = (uint8_t)curParam[1] - 1;
        if (curInBank >= 0 && curInBank < 4 && curOutBank >= 0 && curOutBank < 4)
        { // 4x4 relay matrix
            // Setting latch relay
            uint8_t TPLMask = 0x00000001 << 2 * curOutBank;
            // Switch the CS low
            HAL_GPIO_WritePin(GPIOPortsIn[curInBank], GPIOPinsIn[curInBank], GPIO_PIN_RESET);
            // TBD Send SPI, wait for cs to stabilize
            // Wait for relay to switch(min time)
            // RESET TPL9201
            // Switch the CS high
            int_status->action_return.status = ACTION_OK;
        }
        else
        {
            int_status->action_return.status = ACTION_INVALID_ARGS;
        }
    }
}
// END SWITCH:COMMUTE ----------------------------------------------------------
void sys_log(interpreter_status_t *int_status)
{
    // int_status is unused, but it is put to respect the function prototype
    unsigned char logString[SYS_CHECK_MAXLENGTH] = {'\0'};
    for (uint8_t i = 0; i < MAX_STATUS_QUEUE_SIZE; i++)
    {
        interpreter_status_t *curStatus = statusQueue[i];
        if (curStatus != NULL)
        {
            action_return_t action = curStatus->action_return;
            command_t command = curStatus->command;
            interpreter_flag_t curStatusFlag = curStatus->status;
            unsigned char **args = command.parameters;
            uint8_t argsCount = command.paramsCount;
            char formattedStr[200] = {'\0'};
            snprintf(formattedStr, sizeof(formattedStr), 
            "\r\n ---COMMAND DIAGNOSTIC--- \n\r" 
            "Command: %s Status: %d 0->ok, Interpreter Status: %s,Message: %s"
            " \n\r Args:", command.rootCommand,action.status,interpreter_flag_msg[curStatusFlag], action.data);
            if (strlen((char *)logString) + strlen(formattedStr) < SYS_CHECK_MAXLENGTH)
            {
                strncat((char *)logString, formattedStr, sizeof(logString) - strlen((char *)logString) - 1);
                for (uint8_t j = 0; j < argsCount; j++)
                {
                    char *curArg = (char *)args[j];
                    char argStr[64];
                    snprintf(argStr, sizeof(argStr), "\n\r ---> Arg %d: %s \n\r", j, curArg);
                    strncat((char *)logString, argStr, sizeof(logString) - strlen((char *)logString) - 1);
                }
            }
            else
            {
                break;
            }
        }
    }
    HAL_UART_Transmit(&huart2, logString, strlen((char *)logString), 1000);
}
/*
void example2(command_t* command) {

}



*/
// end of file actions.c
