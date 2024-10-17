
#include "actions.h"
#include "fsm.h"
#include "interpreter.h"
#include "commands.h"
#include "queues.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "switch_commute.h"

// Here are defined all the commands and all the behaviour associated
// Each string has a corresponding function pointer
const command_table_entry_t command_table[] = {
    {(unsigned char *)"switch:commute", switch_commute_handler},
    {(unsigned char *)"sys:log?", sys_log},
    {(unsigned char *)"sys:getstate", sys_getstate},
    {(unsigned char *)"switch:commute:exclusive", switch_commute_handler},
    {(unsigned char *)"switch:commute:reset", switch_commute_handler},
    {(unsigned char *)"switch:commute:reset:all",switch_commute_reset_all},
    {(unsigned char *)"*IDN?",idn} //IDN Command
    };
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
void sys_getstate(interpreter_status_t *int_status) {
    char output[16] = {0};
    char message[64] = {0};
    state_get_label(output);
    sprintf(message, "Current STATE: %s", output);
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 1000);
}
void idn(interpreter_status_t *int_status) {
    char* message = "STM32F401RE Relay Matrix 4x4 V1";
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 1000);
}
void sys_halt(interpreter_status_t *int_status) {
     //Turn off PSU. The system will need to initialize on next command
     HAL_GPIO_WritePin(PS_ON_GPIO,PS_ON_PIN,GPIO_PIN_SET);
     state_set(PASSIVE);
}

