/**
 * @brief   底盘相关状态机
 */
#ifndef __CHASSIS_STATEMECHINE_H
#define __CHASSIS_STATEMECHINE_H

#include "chassis_start.h"

/******************************类型定义**********************************/

enum Fire_State {
    Fire_Ready, // 发射准备状态
    Fire_Pick,  // 取球状态
    Fire_Shoot  // 射球状态
};

enum Chassis_State {
    Seed_Mode,
    Ball_Mode,
};

/******************************变量定义**********************************/

extern int arm_angle;  // 机械臂角度
extern int rail_angle; // 供弹轨道角度
extern int friction_speed;

extern enum Chassis_State chassis_mode;
extern enum Fire_State fire_state; // 发射状态

void Fire_Ball_Task(void *argument);
void Chassis_State_mechine_Task(void *argument);
void Chassis_State_Mechine_Start(void);

#endif