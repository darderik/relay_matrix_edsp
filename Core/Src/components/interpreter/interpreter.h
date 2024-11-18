#include "actions.h"
#ifndef INTERPRETER_H_
#define INTERPRETER_H_

// Pointer to the function associated with a root commands
typedef enum interpreter_flag
{
    INTERPRETER_OK,
    INTERPRETER_INVALID_COMMAND,
    INTERPRETER_INVALID_ARGS
} interpreter_flag_t;


/**
 * @struct interpreter_status
 *
 * @brief      Structure used to store the result of the interpretation of a
 *             command. It contains the command itself, the result of the
 *             action associated with the command and the status of the
 *             interpretation.
 *
 * @param command     The command interpreted.
 * @param action_return The result of the action associated with the command.
 * @param status      The status of the interpretation.
 */
typedef struct interpreter_status
{
    command_t command;
    action_return_t action_return;
    interpreter_flag_t status;
} interpreter_status_t;

void interpretAndExecuteCommand(interpreter_status_t *int_status);

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
void sys_log(interpreter_status_t *int_status);
void interpreter_status_constructor(interpreter_status_t* int_status);

#endif // INTERPRETER_H_
