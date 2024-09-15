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
#include "switch_commute.h"

// Here are defined all the commands and all the behaviour associated
const command_table_entry_t command_table[] = {
    {(unsigned char *)"switch:commute", switch_commute},
    {(unsigned char *)"sys:log?", sys_log}};
uint8_t command_table_size()
{
    uint8_t elemCount = sizeof(command_table) / sizeof(command_table[0]);
    return elemCount;
}


void sys_log(interpreter_status_t *int_status)
{
    // int_status is unused, but it is put to respect the function prototype
    HAL_UART_Transmit(&huart2, sysLogMessage, strlen((char *)sysLogMessage), 1000);
    memset(sysLogMessage, 0, sizeof(sysLogMessage));
}


