/*
 * @Author: Chen Yitong
 * @Date: 2024-01-08 13:46:49
 * @LastEditors: FurryFox2004 
 * @LastEditTime: 2024-01-14 14:26:38
 * @FilePath: \MyChassisTry\UserCode\Chassis\Chassis\Chassis.h
 * @Brief: 底盘结构体初始化 底盘逆解算法 舵轮初始化
 *
 * Copyright (c) 2024 by ChenYiTong, All Rights Reserved.
 */
#ifndef __CHASSIS_H__
#define __CHASSIS_H__

/*引用库文件*/
#include "main.h"
#include "can.h"
#include "gpio.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "wtr_calculate.h"
#include "wtr_can.h"
#include "wtr_dji.h"
#include "wtr_uart.h"
#include "wtr_vesc.h"
#include "wtr_math.h"
#include "Chassis.h"
#include "Chassis_Callback.h"
#include "Chassis_commen.h"
#include "Chassis_UserConfig.h"
#include "Chassis_RemoteCtl.h"
#include "Chassis_Servo.h"
#include "Chassis_StateMachine.h"
#include "Chassis_Variable.h"
#include "Chassis.h"
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "uart_printf.c"

void Chassis_Init();

void FourSteeringInverseCalculate();

void WheelCorrect();

#endif // __CHASSIS_H__