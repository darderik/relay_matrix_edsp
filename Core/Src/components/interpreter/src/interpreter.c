/**
 * @file      interpreter.c
 * @brief     Interpreter for executing commands
 * @version   0.1
 * @date      02-09-2024
 */

#include "parser.h"
#include "interpreter.h"
#include "queues.h"
#include "commands.h"
#include "main.h"
const char* interpreter_flag_msg[] = {"OK", "INVALID COMMAND", "INVALID ARGS"};

/**
 * @brief Interpret and Execute a command
 *
 * @param[out] int_status  Interpreter status structure pointer
 *
 * @return None
 */
 void interpretAndExecuteCommand(interpreter_status_t* int_status) {

    // Parse first command from list of unparsed commands
    command_t* curCommandPtr = &(int_status->command);
    interpreter_status_constructor(int_status);
    parseCommand(curCommandPtr, unparsed_list.head->command);
    ucq_popElement();
    
    // Do all checks on instance of command
    if (!command_isconsistent(curCommandPtr)) {
        int_status->status = INTERPRETER_INVALID_ARGS;
    } else {
        // Find the command in the command table
        uint8_t found = 0;
        uint8_t maxCount= command_table_size();
        for (int i = 0; i < maxCount; i++) {
            // Max one match for the search
            if (strcicmp( (char*)command_table[i].rootCommand, (char*)curCommandPtr->rootCommand) == 0) {
                // Execute the command
                command_table[i].function(int_status);
                found = 1;
                break;
            }
        }
        if (!found) {
            int_status->status = INTERPRETER_INVALID_COMMAND;
        }
    }
}


