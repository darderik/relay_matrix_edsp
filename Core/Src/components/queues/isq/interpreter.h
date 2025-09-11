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
    uint8_t available;
    struct interpreter_status *next;
} interpreter_status_t;

// All functions actions, including parsing
typedef void (*action_function_ptr)(interpreter_status_t *int_status);

extern const char *interpreter_flag_msg[];
extern interpreter_status_t isq_queue[ISQ_QUEUE_SIZE];

typedef struct interpreter_status_list_container
{
    interpreter_status_t *head;
    interpreter_status_t *tail;
} interpreter_status_list_container_t;

typedef struct command_table_entry
{
    const unsigned char *rootCommand;
    const action_function_ptr function;
} command_table_entry_t;
extern const command_table_entry_t command_table[];
void interpretAndExecuteCommand(interpreter_status_t *int_status);
void interpreter_status_deconstructor(interpreter_status_t *int_status);
void sys_log(interpreter_status_t *int_status);
void interpreter_status_constructor(interpreter_status_t *int_status);
interpreter_status_t *isq_add_element();

#endif