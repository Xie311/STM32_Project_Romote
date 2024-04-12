#include "chassis_callback.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == BOARD_MESSAGE_UART) {
        Board_Message_Decode();
    }
}