/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 13:47:29
 * @LastEditors: Chen Yitong
 * @LastEditTime: 2023-09-24 18:52:45
 * @FilePath: \WTR_Chassis\麦克纳姆轮\UserCode\Chassis\RemoteCtl\Chassis_RemoteCtl.c
 * @Brief: 底盘遥控代码
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#include "Chassis_RemoteCtl.h"


/**
 * @brief 底盘遥控初始化
 * @return {*}
 */
void Chassis_RemoteCtl_Init()
{
    HAL_UART_Receive_DMA(&huart_RemoteCtl, JoyStickReceiveData, 18);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); // IDLE 中断使能
}
