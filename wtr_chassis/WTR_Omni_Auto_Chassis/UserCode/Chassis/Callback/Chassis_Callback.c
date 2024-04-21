/*
 * @Author: X311
 * @Date: 2024-04-16
 * @LastEditors: x311 
 * @LastEditTime: 2024-04-14 22:59:32
 * @FilePath: \WTR_Omni_Chassis\UserCode\Chassis\Callback\Chassis_Callback.c
 * @Brief: 底盘用中断回调函数
 */

#include "Chassis_Callback.h"

/**
 * @brief: 串口接收中断回调函数
 * @param {UART_HandleTypeDef} *huart 中断串口
 * @return {*}
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == OPS_UART) 
    {
        OPS_Decode(); // 当数据接收完成时调用OPS码盘解码函数
        HAL_UART_Receive_IT(&huart7, ops_buffer, sizeof(OPS_Data));
    }

    if (huart->Instance == Tar_UART) {

        //接收上位机数据后进行解码
        //To be continued
        HAL_UART_Receive_IT(&huart6, tar_buffer, sizeof(Tar_Data));
    }
}