
#include "STP_Start.h"

void STP_32L_Start()
{
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); // 串口中断使能
    HAL_UART_Receive_IT(&huart1, (uint8_t *)Rxbuffer,195);

    OLED_Init();  // 屏幕初始化
    OLED_Clear(); // 先清屏
}