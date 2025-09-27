#include "actions.h"
#include "fsm.h"
#include "interpreter.h"
#include "command.h"
#include "queues.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "switch_commute.h"
#include "config.h"
#include "actions_list.h"
#include "sys_log_queue.h"

// Here are defined all the commands and all the behaviour associated
// Each string has a corresponding function pointer
const command_table_entry_t command_table[] = {
    {(unsigned char *)"switch:commute", switch_commute_handler},
    {(unsigned char *)"sys:log?", sys_log},
    {(unsigned char *)"sys:getstate?", sys_getstate},
    {(unsigned char *)"switch:commute:status?", switch_commute_status},
    {(unsigned char *)"switch:commute:exclusive", switch_commute_handler},
    {(unsigned char *)"switch:commute:mono", switch_commute_handler},
    {(unsigned char *)"switch:commute:reset", switch_commute_handler},
    {(unsigned char *)"switch:commute:reset:all", switch_commute_reset_all},
    {(unsigned char *)"switch:commute:byte:status?", switch_commute_byte_status},
    {(unsigned char *)"*IDN?", idn}, // IDN Command
    {(unsigned char *)"sys:halt", sys_halt},
    {(unsigned char *)"*OPC?", opc},
    //,{(unsigned char *)"help?", help }
};
uint8_t command_table_size()
{
    uint8_t elemCount = sizeof(command_table) / sizeof(command_table[0]);
    return elemCount;
}

void sys_halt(interpreter_status_t *int_status)
{
    // Turn off PSU. The system will need to initialize on next command
    HAL_GPIO_WritePin(PS_ON_GPIO, PS_ON_PIN, GPIO_PIN_SET);
    state_set(PASSIVE);
}
// This outputs each command with its description, help command TODO
void help(interpreter_status_t *int_status)
{
    int_status->action_return.toTransmit = 1;
}
void sys_log(interpreter_status_t *int_status)
{
    uint16_t full_message_size = slq_get_total_length() + 1;
    uint16_t effective_message_size = full_message_size > SYSLOG_SINGLE_MESSAGE_LENGTH * SYSLOG_MAX_MESSAGES
                                          ? SYSLOG_SINGLE_MESSAGE_LENGTH * SYSLOG_MAX_MESSAGES
                                          : full_message_size;
    char full_message[effective_message_size];
    // Initialize full message to null char
    memset(full_message, '\0', effective_message_size);

    slq_get_full_message(full_message, effective_message_size);
    action_return_addMessage(&(int_status->action_return), full_message, 1);
}
void sys_getstate(interpreter_status_t *int_status)
{
    char output[16] = {0};
    char message[64] = {0};
    state_get_label(output);
    sprintf(message, "STATE:%s", output);
    action_return_addMessage(&(int_status->action_return), message, 1);
}
void idn(interpreter_status_t *int_status)
{
    uint32_t uid[3];
    uid[0] = HAL_GetUIDw0();
    uid[1] = HAL_GetUIDw1();
    uid[2] = HAL_GetUIDw2();
    // 3*8 + 1 FF FF FF FF
    char uid_string[26] = {0};
    snprintf(uid_string, sizeof(uid_string), "%08lX%08lX%08lX",
             (unsigned long)uid[0],
             (unsigned long)uid[1],
             (unsigned long)uid[2]);
    char full_message[96] = {'\0'};
    snprintf(full_message, sizeof(full_message), "DIIEM,Relay Matrix,%s,V1.0", uid_string);
    action_return_addMessage(&(int_status->action_return), full_message, 1);
}
void opc(interpreter_status_t *int_status)
{
    action_return_addMessage(&(int_status->action_return), "1", 1);
}
