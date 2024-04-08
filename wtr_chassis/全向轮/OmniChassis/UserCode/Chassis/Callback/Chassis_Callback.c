/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 14:13:20
 * @LastEditors: Chen Yitong
 * @LastEditTime: 2023-10-21 21:56:54
 * @FilePath: \WTR_Chassis\麦克纳姆轮\UserCode\Chassis\Callback\Chassis_Callback.c
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
}
