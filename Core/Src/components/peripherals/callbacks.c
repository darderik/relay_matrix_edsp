#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fsm.h"
#include "config.h"
#include "queues.h"
#include "callbacks.h"
#include "main.h"
#include "stm32f4xx_hal_uart.h"
#include "callbacks.h"

static uint16_t inProgress;
uint8_t is_query(unsigned char *command)
{
    // Contains '?'
    return strchr((char *)command, (uint8_t)'?') != NULL;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    char msg[32] = {'\0'};

    if (huart->Instance == USART2)
    {
        // 1. Immediate copy to avoid DMA buffer overwrite
        char local_buf[MAX_COMMAND_LENGTH + 1];
        memcpy(local_buf, rx_buffer, Size);
        local_buf[Size] = '\0'; // Ensures null termination

        // 2. Reactivate DMA reception IMMEDIATELY
        HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, MAX_COMMAND_LENGTH);

        // 3. Check if the terminator is present
        int term_char_present = (strstr(local_buf, TERM_CHAR) != NULL);

        if (term_char_present)
        {
            // 4. Command completed, assemble any previous part
            if (inProgress > 0)
            {
                if (inProgress + Size < MAX_COMMAND_LENGTH)
                {
                    memcpy(rx_buffer_temp + inProgress, local_buf, Size);
                    rx_buffer_temp[inProgress + Size] = '\0';
                }
                else
                {
                    // Overflow, reset temp buffer
                    inProgress = 0;
                    memset(rx_buffer_temp, 0, MAX_COMMAND_LENGTH);
                    return;
                }
                inProgress = 0;
            }
            else
            {
                memcpy(rx_buffer_temp, local_buf, Size);
                rx_buffer_temp[Size] = '\0';
            }

            // 5. Send to the prequeue
            uint8_t res = prequeue_add((unsigned char *)rx_buffer_temp);

            // 6. Handshake response
            if (HANDSHAKE_SCPI && !is_query(rx_buffer_temp))
            {
                snprintf(msg, sizeof(msg), "OK%s", TERM_CHAR);
            }
            else if (HANDSHAKE_SCPI && res == 0 && is_query(rx_buffer_temp))
            {
                snprintf(msg, sizeof(msg), "ERR:FULL_QUEUE%sOK%s", TERM_CHAR, TERM_CHAR);
            }
            else if (!HANDSHAKE_SCPI && res == 0 && is_query(rx_buffer_temp))
            {
                snprintf(msg, sizeof(msg), "ERR:FULL_QUEUE%s", TERM_CHAR);
            }

            // 7. Reset the temporary buffer
            memset(rx_buffer_temp, 0, MAX_COMMAND_LENGTH);
        }
        else
        {
            // 8. Incomplete command, accumulate
            if (inProgress + Size < MAX_COMMAND_LENGTH)
            {
                memcpy(rx_buffer_temp + inProgress, local_buf, Size);
                inProgress += Size;
                rx_buffer_temp[inProgress] = '\0';
            }
            else
            {
                // Overflow, reset buffer
                inProgress = 0;
                memset(rx_buffer_temp, 0, MAX_COMMAND_LENGTH);
            }
        }

        // 9. Transmit any response message
        if (msg[0] != '\0')
        {
            HAL_UART_Transmit(huart, (unsigned char *)msg, strlen(msg), HAL_MAX_DELAY);
        }
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    uint32_t errorCode;

    if (huart->Instance == USART2)
    {
        errorCode = HAL_UART_GetError(huart);
        if (errorCode != HAL_UART_ERROR_NONE)
        {

            // Dma disabled, restart
            HAL_UART_DMAStop(huart);
            HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, MAX_COMMAND_LENGTH);
        }
    }
}