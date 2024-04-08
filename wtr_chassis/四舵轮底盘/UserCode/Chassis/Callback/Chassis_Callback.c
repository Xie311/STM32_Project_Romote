/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 14:13:20
 * @LastEditors: 15867773077
 * @LastEditTime: 2024-01-07 15:37:43
 * @FilePath: \MyChassisTry\UserCode\Chassis\Callback\Chassis_Callback.c
 * @Brief: 底盘用中断回调函数
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#include "Chassis_Callback.h"

/**
 * @brief 光电门中断回调
 * @auther: Chen YiTong 3083697520
 * @param *wheel
 * @return
 */
void Wheel_Diammer_Callback(WHEEL_COMPONENT *wheel)
{
    // HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
    if (!wheel->Dimmer_detect_flag) {
        Wheel_ReadNowRotPos(wheel);
        wheel->Dimmer_on_pos      = wheel->now_rot_pos;
        wheel->Dimmer_detect_flag = true;
    } else {
        Wheel_ReadNowRotPos(wheel);
        wheel->Dimmer_detect_flag = false;
        wheel->Dimmer_off_pos     = wheel->now_rot_pos;
    }
    // if (HAL_GPIO_ReadPin(wheel->Dimmer_GPIO_Port, wheel->Dimmer_GPIO_Pin) == GPIO_PIN_SET) {
    //     Wheel_ReadNowRotPos(wheel);
    //     wheel->Dimmer_on_pos      = wheel->now_rot_angle;
    //     wheel->Dimmer_detect_flag = true;
    // } else {
    //     if (wheel->Dimmer_detect_flag) {
    //         wheel->Dimmer_detect_flag = false;
    //         Wheel_ReadNowRotPos(wheel);
    //         wheel->Dimmer_off_pos = wheel->now_rot_angle;
    //     }
    // }
}

/**
 * @brief: 串口接收中断回调函数
 * @param {UART_HandleTypeDef} *huart 中断串口
 * @return {*}
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == huart_RemoteCtl.Instance) {
        DjiRemoteCtl_Decode(); // 大疆遥控器解码
    }
}

/**
 * @brief gpio外部中断回调函数
 * @auther: Chen YiTong 3083697520
 * @param GPIO_Pin
 * @return
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == Chassis.Wheel.ChassisWheels[0].Dimmer_GPIO_Pin) {
        Wheel_Diammer_Callback(&(Chassis.Wheel.ChassisWheels[0]));
    } else if (GPIO_Pin == Chassis.Wheel.ChassisWheels[1].Dimmer_GPIO_Pin) {
        Wheel_Diammer_Callback(&(Chassis.Wheel.ChassisWheels[1]));
    } else if (GPIO_Pin == Chassis.Wheel.ChassisWheels[2].Dimmer_GPIO_Pin) {
        Wheel_Diammer_Callback(&(Chassis.Wheel.ChassisWheels[2]));
    } else if (GPIO_Pin == Chassis.Wheel.ChassisWheels[3].Dimmer_GPIO_Pin) {
        Wheel_Diammer_Callback(&(Chassis.Wheel.ChassisWheels[3]));
    }
}
