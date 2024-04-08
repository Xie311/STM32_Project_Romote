/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 13:49:46
 * @LastEditors: Chen Yitong
 * @LastEditTime: 2023-10-17 00:11:36
 * @FilePath: \MyChassisTry\UserCode\Chassis\Servo\Chassis_Servo.h
 * @Brief: 底盘伺服函数
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#ifndef __CHASSIS_SERVO_H__
#define __CHASSIS_SERVO_H__
#include "Chassis.h"
void Wheel_ReadNowRotPos(WHEEL_COMPONENT *wheel);
void Chassis_Servo_Init();
void Chassis_Servo_TaskStart();

#endif // __CHASSIS_SERVO_H__