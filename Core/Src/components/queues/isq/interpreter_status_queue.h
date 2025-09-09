#include "interpreter.h"
#ifndef INTERPRETER_STATUS_QUEUE_H
#define INTERPRETER_STATUS_QUEUE_H
void isq_pop_element();
uint8_t isq_get_size();
typedef struct isq_container
{
    interpreter_status_t *head;
    interpreter_status_t *tail;
} isq_container_t;

#endif
