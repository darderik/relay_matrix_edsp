#include "command.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#ifndef ACTIONS_H_
#define ACTIONS_H_

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
    char message[ACTION_MSG_LENGTH];
    uint8_t toTransmit;
} action_return_t;

void action_return_constructor(action_return_t *instance);
void action_return_addMessage(action_return_t *instance, char *message, uint8_t toTransmit);
#endif