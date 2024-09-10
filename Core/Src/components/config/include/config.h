#include <inttypes.h>
#ifndef CONFIG_H_
#define CONFIG_H_
// System settings
#define MAX_COMMAND_LENGTH 64
#define MAX_TIMEOUT_MINUTES 3
// Flags
#define DMA 0
#define CPU 1

#define QUEUE_MODE CPU
void config_set_uart_mode(uint8_t mode);
#endif // CONFIG_H_