#include "wtr_as69.h"

Remote_t as69_data;      // 遥控器原始数据
uint8_t as69_buffer[18]; // 遥控器数据缓冲区
/************************************用户函数段*************************************/
/**
 * @brief   AS69 解码函数
 */
int AS69_Decode(void)
{
    as69_data.ch0 = ((int16_t)as69_buffer[0] | ((int16_t)as69_buffer[1] << 8)) & 0x07FF;
    as69_data.ch1 = (((int16_t)as69_buffer[1] >> 3) | ((int16_t)as69_buffer[2] << 5)) & 0x07FF;
    as69_data.ch2 = (((int16_t)as69_buffer[2] >> 6) | ((int16_t)as69_buffer[3] << 2) |
                     ((int16_t)as69_buffer[4] << 10)) &
                    0x07FF;
    as69_data.ch3   = (((int16_t)as69_buffer[4] >> 1) | ((int16_t)as69_buffer[5] << 7)) & 0x07FF;
    as69_data.left  = ((as69_buffer[5] >> 4) & 0x000C) >> 2;
    as69_data.right = ((as69_buffer[5] >> 4) & 0x0003);
    as69_data.wheel = ((int16_t)as69_buffer[16]) | ((int16_t)as69_buffer[17] << 8);
    return 0;
}

/**
 * @brief   串口回调函数
 */
__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        AS69_Decode();
    }
}