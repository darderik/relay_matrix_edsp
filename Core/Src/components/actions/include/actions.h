/**
 * @file      actions.h
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      04-09-2024
 * @copyright 2024, your company / association / school
 */

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
void action_return_constructor(action_return_t *instance);
#endif // ACTIONS_H_

