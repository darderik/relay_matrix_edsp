#ifndef ACTIONS_H_
#define ACTIONS_H_
#include "commands.h"
#include <inttypes.h>
typedef enum action_status
{
    ACTION_OK,
    ACTION_ERROR,
    ACTION_INVALID_ARGS
} action_status_t;

typedef struct action_return {
    uint8_t status;
    unsigned char data[ACTION_DATA_MAXLENGTH];
}  action_return_t;
uint8_t command_table_size();

#include "interpreter.h"
//Actions prototypes
void action_return_constructor(action_return_t *instance);
void idn(interpreter_status_t *int_status);
void sys_getstate(interpreter_status_t *int_status);

#endif // ACTIONS_H_

