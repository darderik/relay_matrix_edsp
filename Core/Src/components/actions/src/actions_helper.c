#include "actions.h"

void action_return_constructor(action_return_t *instance)
{
    instance->status = ACTION_OK;
    for (int i = 0; i < ACTION_DATA_MAXLENGTH; i++)
    {
        instance->data[i] = '\0';
    }
}