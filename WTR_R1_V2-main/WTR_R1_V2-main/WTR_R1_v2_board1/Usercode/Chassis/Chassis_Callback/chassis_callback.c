#include "chassis_start.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == OPS_UART) {
        OPS_Decode();
    }
    if (huart->Instance == USART1) {
        AS69_Decode();
    }
}