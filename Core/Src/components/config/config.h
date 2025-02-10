#include <inttypes.h>
#include "main.h"
#ifndef CONFIG_H_
#define CONFIG_H_

// Flags
#define DMA 0
#define CPU 1
#define PSU_PWROK 0
#define PSU_TIMEOUT 1
// System settings
#define MAX_COMMAND_LENGTH 256
#define MAX_TIMEOUT_MINUTES 3
#define PSU_TURNON_TIMEOUT 600
#define PSU_CHECK_MODE PSU_TIMEOUT
#define QUEUE_MODE DMA
#define HANDSHAKE_SCPI 1
#define TERM_CHAR "\n"
// Newline dummy character that in reception may get replaced by proper newline. Multiline msg
#define NEWLINE_STR "\\|" // pyVisa doesn't allow multichar term char so using \n or \r for multiline message may break.

// Queues
#define MAX_COMMAND_QUEUE_SIZE 32
#define MAX_STATUS_QUEUE_SIZE 64
#define SYSLOG_SINGLE_MESSAGE_LENGTH 512
#define SYSLOG_MAX_MESSAGES 6
#define ACTION_DATA_MAXLENGTH 128

// Init
#define PS_ON_GPIO GPIOC
#define PS_ON_PIN GPIO_PIN_9
#define PWR_OK_GPIO GPIOB
#define PWR_OK_PIN GPIO_PIN_5

// Switch commute
#define SWITCH_EMR 0
#define SWITCH_SSR 1
#define SWITCH_COMMUTE_MODE SWITCH_EMR

/**
 * @struct relay_group_t
 *
 * @brief Structure used to store relay groups
 *
 * @param letter         Letter of relay group
 * @param gpio_port_ncs  GPIO port for NCS
 * @param gpio_port_nrst GPIO port for nRST
 * @param ncs_pin        Pin number for NCS
 * @param nrst_pin       Pin number for nRST
 */
typedef struct
{
    char letter;
    GPIO_TypeDef *gpio_port_ncs;
    GPIO_TypeDef *gpio_port_nrst;
    uint16_t ncs_pin;
    uint16_t nrst_pin;
    uint8_t oldByte;
} relay_group_t;
extern relay_group_t relayGroups[4];

void config_set_uart_mode(uint8_t mode);
#endif // CONFIG_H_