#include "fsm.h"
#include "config.h"
#include "string.h"
#include <stdlib.h>
#include "queues.h"
#include "callbacks.h"

uint8_t inProgress = 0;
uint32_t lastTick = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        if (QUEUE_MODE == CPU)
        {
            unsigned char curChar = rx_data_ptr;
            if (curChar != '\n' && curChar != '\r' && curChar != 127) // Delimiter
            {
                // Byte received
                rx_buffer[inProgress++] = curChar;
                // Verify timeout
                uint32_t curTick = HAL_GetTick();
                if (curTick - lastTick > 1000 * 60 * MAX_TIMEOUT_MINUTES)
                {
                    // Flush everything
                    inProgress = 0;
                    lastTick = 0;
                    // Set all rx_buffer to null
                    memset(rx_buffer, '\0', sizeof(rx_buffer[0]) * MAX_COMMAND_LENGTH);
                }
                else
                {
                    lastTick = HAL_GetTick();
                }
                HAL_UART_Transmit(huart, &rx_data_ptr, 1, HAL_MAX_DELAY);
            }
            else if (curChar == 127) //Backspace deletes previous character
            {
                if (inProgress > 0)
                {
                    // Delete last character
                    rx_buffer[--inProgress] = '\0';
                    HAL_UART_Transmit(huart, (unsigned char *)"\033[2K\r", 6, HAL_MAX_DELAY);
                    HAL_UART_Transmit(huart, rx_buffer, strlen((char*)rx_buffer), HAL_MAX_DELAY);
                }
            }
            else
            {
                // Append terminator
                rx_buffer[inProgress] = '\0';
                // End of command
                inProgress = 0;
                lastTick = 0;
                // Move buffer to queue
                ucq_addElement(rx_buffer);
                HAL_UART_Transmit(huart, (unsigned char *)"\r\n", 2, HAL_MAX_DELAY);
            }
        }
    }
    HAL_UART_Receive_IT(huart, &rx_data_ptr, 1);
}