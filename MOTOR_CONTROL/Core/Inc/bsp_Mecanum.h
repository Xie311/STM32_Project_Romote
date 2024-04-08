//
// Created by Quan2 on 2023/4/23.
//

#ifndef STM32F407_DEMO2_BSP_MECANUM_H
#define STM32F407_DEMO2_BSP_MECANUM_H

#include "motor_control.h"
extern float cal_matrix[4][3];
extern float motor_w[4];
extern float car_speed[3];
extern float MOTOR_R;

void cal_motor_speed(float cal_matrix_[4][3],float* motor_w_,float* car_speed_,float MOTOR_R_);
#endif //STM32F407_DEMO2_BSP_MECANUM_H
