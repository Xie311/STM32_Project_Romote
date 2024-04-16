#include "STP_Receive.h"
uint16_t DataLens;
LidarPointTypedef LidarData[12];
uint8_t Rxbuffer[195]={0};
uint16_t flag = 0; //test
uint16_t cnt  = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        STP_23L_Decode();               //Decode_Function
        cnt++;
        HAL_UART_Receive_IT(USART_RX_HANDLE, (uint8_t *)Rxbuffer, 195);
    }
}