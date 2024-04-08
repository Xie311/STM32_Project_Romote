/*
 * @Author: szf
 * @Date: 2023-02-22 12:06:17
 * @LastEditors: Chen Yitong 
 * @LastEditTime: 2023-10-17 16:58:12
 * @FilePath: \MyChassisTry\UserCode\Lib\Calculate\wtr_calculate.h
 * @brief 大疆电机PID 遥控死区控制
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */
#ifndef __WTR_CALCULATE_H__
#define __WTR_CALCULATE_H__
#include "Chassis.h"

#include "wtr_dji.h"

#define r_underpan_3 0.1934
#define r_underpan_4 0.25
#define r_wheel      0.076

// 位置伺服
void positionServo(float ref, DJI_t *motor);

// 速度伺服
void speedServo(float ref, DJI_t *motor);

// 圆周死区控制
void DeadBand(double x, double y, double *new_x, double *new_y, double threshould);

// 单维度死区控制
void DeadBandOneDimensional(double x, double *new_x, double threshould);

void VelocityPlanning(float initialAngle, float maxAngularVelocity, float AngularAcceleration, float targetAngle, float currentTime, volatile float *currentAngle);

#endif // __WTR_CALCULATE_H__
