#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fsm.h"
#include "config.h"
#include "queues.h"
#include "callbacks.h"

static uint16_t inProgress;
static uint32_t lastTick;

uint8_t is_query(unsigned char *command)
{
    // Contains '?'
    return strchr((char *)command, '?') != NULL;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        if (QUEUE_MODE == CPU)
        {
            unsigned char curChar = rx_data_ptr;
            switch (curChar)
            {
            case '\n':
            case '\r':
                // End of command
                // Move buffer to queue
                rx_buffer[inProgress] = '\0';
                ucq_addElement(rx_buffer);
                if (HANDSHAKE_SCPI && !is_query(rx_buffer))
                {
                    unsigned char handshake[8];
                    snprintf((char *)handshake, sizeof(handshake), "OK%s", TERM_CHAR);
                    HAL_UART_Transmit(huart, (uint8_t *)handshake, strlen((char *)handshake), HAL_MAX_DELAY);
                }
                inProgress = 0;
                lastTick = 0;
                break;
            case 127:
                // Backspace
                if (inProgress > 0)
                {
                    // Delete last character
                    rx_buffer[--inProgress] = '\0';
                    HAL_UART_Transmit(huart, (unsigned char *)"\033[2K\r", 6, HAL_MAX_DELAY);
                    HAL_UART_Transmit(huart, rx_buffer, strlen((char *)rx_buffer), HAL_MAX_DELAY);
                }
                break;
            default:
                // Byte received
                rx_buffer[inProgress++] = curChar;
                // Verify timeout
                uint32_t curTick = HAL_GetTick();
                if (curTick - lastTick > 1000 * 60 * MAX_TIMEOUT_MINUTES && lastTick)
                {
                    // Flush everything
                    inProgress = 0;
                    lastTick = 0;
                    // Set all rx_buffer to null, this memset is not actually needed for sake of performance
                    // memset(rx_buffer, '\0', sizeof(rx_buffer[0]) * MAX_COMMAND_LENGTH);
                }
                else
                {
                    lastTick = HAL_GetTick();
                }
                HAL_UART_Transmit(huart, &rx_data_ptr, 1, HAL_MAX_DELAY);
                break;
            }
        }
        HAL_UART_Receive_IT(huart, &rx_data_ptr, 1);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        // WE Have received a string,add to ucq (if there is space)
        rx_buffer[Size] = '\0';
        ucq_addElement(rx_buffer);
        if (HANDSHAKE_SCPI && !is_query(rx_buffer))
        {
            char message[8];
            snprintf(message, sizeof(message), "OK%s", TERM_CHAR);
            HAL_UART_Transmit(huart, (unsigned char *)message, strlen(message), HAL_MAX_DELAY);
        }
    }
    HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, MAX_COMMAND_LENGTH);
}