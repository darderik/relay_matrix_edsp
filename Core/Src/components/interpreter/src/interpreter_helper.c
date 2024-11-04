#include "main.h"
#include <stdlib.h>
#include "interpreter.h"

/**
 * @brief Deconstruct the interpreter status
 * 
 * @param int_status 
 */
void interpreter_status_deconstructor(interpreter_status_t* int_status) {
    if (int_status != NULL) {
        command_deconstructor(&(int_status->command));
        action_return_deconstructor(&(int_status->action_return));
        free(int_status);
    }
}
void interpreter_status_constructor(interpreter_status_t* int_status) {
    if (int_status != NULL) {
        command_constructor(&(int_status->command));
        action_return_constructor(&(int_status->action_return));
        int_status->status = INTERPRETER_OK;
    }
}