#include <inttypes.h>
#include "main.h"
#ifndef CONFIG_H_
#define CONFIG_H_
// System settings
#define MAX_COMMAND_LENGTH 64
#define MAX_TIMEOUT_MINUTES 3
#define PSU_TURNON_TIMEOUT 600
// Flags
#define DMA 0
#define CPU 1

#define QUEUE_MODE CPU

// Queues
#define MAX_COMMAND_QUEUE_SIZE 8
#define MAX_STATUS_QUEUE_SIZE 8
#define MAX_SYSLOG_BUFFER_SIZE 1024
#define SYSLOG_SINGLE_MESSAGE_LENGTH 200
#define ACTION_DATA_MAXLENGTH 32
// Init
#define PS_ON_GPIO GPIOC
#define PS_ON_PIN GPIO_PIN_9 


 // Switch commute
#define SWITCH_EMR 0
#define SWITCH_SSR 1
#define SWITCH_COMMUTE_MODE SWITCH_EMR
typedef struct
{
    char letter;
    GPIO_TypeDef *gpio_port_ncs;
    GPIO_TypeDef *gpio_port_nrst;
    uint16_t ncs_pin;
    uint16_t nrst_pin;
} relay_group_t;
extern const relay_group_t relayGroups[4];

void config_set_uart_mode(uint8_t mode);
#endif // CONFIG_H_