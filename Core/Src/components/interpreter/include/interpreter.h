/**
 * @file      interpreter.h
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      02-09-2024
 * @copyright 2024, your company / association / school
 */

#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include "actions.h"

// Pointer to the function associated with a root commands
typedef enum interpreter_flag
{
    INTERPRETER_OK,
    INTERPRETER_INVALID_COMMAND,
    INTERPRETER_INVALID_ARGS
} interpreter_flag_t;
typedef struct interpreter_status
{
    command_t command;
    action_return_t action_return;
    interpreter_flag_t status;
} interpreter_status_t;
interpreter_status_t *interpretAndExecuteCommand();

// All functions actions, including parsing
typedef void (*action_function_ptr)(interpreter_status_t *int_status);

typedef struct command_table_entry
{
    const unsigned char *rootCommand;
    const action_function_ptr function;
} command_table_entry_t;
extern const command_table_entry_t command_table[];
extern const char* interpreter_flag_msg[];
void interpreter_status_deconstructor(interpreter_status_t* int_status);
void switch_commute(interpreter_status_t *int_status);
void sys_log(interpreter_status_t *int_status);
void interpreter_status_constructor(interpreter_status_t* int_status);

#endif // INTERPRETER_H_
