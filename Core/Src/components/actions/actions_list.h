#include "interpreter.h"
#include "actions.h"
#ifndef ACTIONS_H_LIST
#define ACTIONS_H_LIST

uint8_t command_table_size();

// Actions prototypes
void idn(interpreter_status_t *int_status);
void sys_getstate(interpreter_status_t *int_status);
void sys_halt(interpreter_status_t *int_status);
void opc(interpreter_status_t *int_status);
#endif // ACTIONS_H_