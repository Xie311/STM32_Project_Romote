/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 14:13:20
 * @LastEditors: x311 
 * @LastEditTime: 2024-04-14 22:59:32
 * @FilePath: \WTR_Omni_Chassis\UserCode\Chassis\Callback\Chassis_Callback.c
 * @Brief: 底盘用中断回调函数
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#include "Chassis_Callback.h"

/**
 * @brief: 串口接收中断回调函数
 * @param {UART_HandleTypeDef} *huart 中断串口
 * @return {*}
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == huart_RemoteCtl.Instance)
    {
        DjiRemoteCtl_Decode(); // 大疆遥控器解码
        
    }
    
    if (huart->Instance == OPS_UART) 
    {
        OPS_Decode(); // 当数据接收完成时调用OPS码盘解码函数
        HAL_UART_Receive_IT(&huart7, ops_buffer, sizeof(OPS_Data));
    }

    //（Mavlink从上位机）接收数据，保存在Target_Position结构体中
}

