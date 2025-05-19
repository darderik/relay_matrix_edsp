#include "parser.h"
#include "interpreter.h"
#include "queues.h"
#include "command.h"
#include "main.h"
#include "callbacks.h"
#include <string.h>

const char *interpreter_flag_msg[] = {"OK", "INVALID COMMAND", "INVALID ARGS"};

/**
 * @brief Interpret and Execute a command
 *
 * @param[out] int_status  Interpreter status structure pointer
 *
 * @return None
 */
void interpretAndExecuteCommand(interpreter_status_t *int_status)
{

    // Parse first command from list of unparsed commands
    command_t *curCommandPtr = &(int_status->command);

    // Call command constructor here so we can pop an element
    uint8_t *curUnformatted = unparsed_list.head->command;
    command_constructor(curCommandPtr, curUnformatted);

    ucq_popElement();

    // Do all checks on instance of command
    if (!command_isconsistent(curCommandPtr))
    {
        int_status->status = INTERPRETER_INVALID_ARGS;
    }
    else
    {
        // Find the command in the command table
        uint8_t found = 0;
        uint8_t maxCount = command_table_size();
        for (int i = 0; i < maxCount; i++)
        {
            // Max one match for the search
            char *curRootCommand = (char *)curCommandPtr->rootCommand;
            if (strcicmp((char *)command_table[i].rootCommand, curRootCommand) == 0)
            {
                // Execute the command
                command_table[i].function(int_status);
                found = 1;
                // Return data if needed, uart transmit of action_return data
                if (int_status->action_return.toTransmit)
                {
                    char fullMsg[strlen((char *)int_status->action_return.message) + 8];
                    memset(fullMsg, '\0', sizeof(fullMsg));
                    if (HANDSHAKE_SCPI)
                    {
                        snprintf((char *)fullMsg, sizeof(fullMsg), "%s%s%s%s", int_status->action_return.message, TERM_CHAR, "OK", TERM_CHAR);
                    }
                    else
                    {
                        snprintf((char *)fullMsg, sizeof(fullMsg), "%s%s", int_status->action_return.message, TERM_CHAR);
                    }

                    HAL_UART_Transmit(&huart2, (uint8_t *)fullMsg, strlen((char *)fullMsg), HAL_MAX_DELAY);
                }
                break;
            }
        }
        if (!found)
        {
            int_status->status = INTERPRETER_INVALID_COMMAND;
        }
    }
}
//        if (!found)
//        {
//            char fullMsg[64];
//            memset(fullMsg, '\0', sizeof(fullMsg));
//            if (is_query(curCommandPtr->rootCommand))
//            {
//                if (HANDSHAKE_SCPI)
//                {
//                    snprintf(fullMsg, sizeof(fullMsg), "NO_QUERY%s%s%s", TERM_CHAR, "OK", TERM_CHAR);
//                    HAL_UART_Transmit(&huart2, (uint8_t *)fullMsg, strlen((char *)fullMsg), HAL_MAX_DELAY);
//                }
//            }
//            int_status->status = INTERPRETER_INVALID_COMMAND;
//        }
//    }
