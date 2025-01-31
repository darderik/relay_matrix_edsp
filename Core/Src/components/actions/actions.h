#ifndef ACTIONS_H_
#define ACTIONS_H_
#include "command.h"
#include <inttypes.h>
typedef enum action_status
{
    ACTION_OK,
    ACTION_ERROR,
    ACTION_INVALID_ARGS
} action_status_t;

/**
 * @brief Structure used to store the result of the action associated with a
 *        command
 *
 * @param status      The status of the action
 * @param data        The data associated with the action (up to
 *                    ACTION_DATA_MAXLENGTH bytes)
 * @param message     A pointer to a string to send through the uart channel once task completed
 * @param toTransmit  A boolean indicating if the string in message is to be
 *                    sent through the uart channel
 */
typedef struct action_return
{
    uint8_t status;
    //    unsigned char data[ACTION_DATA_MAXLENGTH];
    uint8_t *message;
    uint8_t toTransmit;
} action_return_t;
uint8_t command_table_size();

#include "interpreter.h"
// Actions prototypes
void action_return_constructor(action_return_t *instance);
void idn(interpreter_status_t *int_status);
void sys_getstate(interpreter_status_t *int_status);
void action_return_addMessage(action_return_t *instance, char *message, uint8_t toTransmit);
void action_return_deconstructor(action_return_t *instance);
void sys_halt(interpreter_status_t *int_status);
void opc(interpreter_status_t *int_status);
#endif // ACTIONS_H_
