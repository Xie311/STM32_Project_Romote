/*
 * @Author: Chen Yitong
 * @Date: 2023-10-10 11:06:04
 * @LastEditors: 15867773077
 * @LastEditTime: 2024-01-08 20:39:24
 * @FilePath: \MyChassisTry - 副本\UserCode\Chassis\Chassis\Chassis.c
 * @Brief: 底盘结构体初始化 底盘逆解算法 舵轮初始化
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */
#include "Chassis.h"
CHASSIS Chassis; // 全局底盘变量

/**
 * @brief 底盘结构体初始化
 * @auther: Chen YiTong 3083697520
 * @return
 */
void Chassis_Init()
{
    /*舵轮初始化*/
    /*大疆电机*/
    Chassis.Wheel.ChassisWheels[0].hDJI = &(hDJI[0]);
    Chassis.Wheel.ChassisWheels[1].hDJI = &(hDJI[1]);
    Chassis.Wheel.ChassisWheels[2].hDJI = &(hDJI[2]);
    Chassis.Wheel.ChassisWheels[3].hDJI = &(hDJI[3]);

    /*VESC初始化*/
    for (uint8_t i = 0; i < 4; i++) {
        Chassis.Wheel.ChassisWheels[i].hvesc.hcann = &hcan_VESC;
    }

    Chassis.Wheel.ChassisWheels[0].hvesc.controller_id = 101;
    Chassis.Wheel.ChassisWheels[1].hvesc.controller_id = 102;
    Chassis.Wheel.ChassisWheels[2].hvesc.controller_id = 103;
    Chassis.Wheel.ChassisWheels[3].hvesc.controller_id = 104;

    /*舵轮相对位置*/
    Chassis.Wheel.ChassisWheels[0].loc_x = -wheel2wheel_x / 2 - Offset_x2center;
    Chassis.Wheel.ChassisWheels[0].loc_y = wheel2wheel_y / 2 - Offset_y2center;
    Chassis.Wheel.ChassisWheels[1].loc_x = wheel2wheel_x / 2 - Offset_x2center;
    Chassis.Wheel.ChassisWheels[1].loc_y = wheel2wheel_y / 2 - Offset_y2center;
    Chassis.Wheel.ChassisWheels[2].loc_x = wheel2wheel_x / 2 - Offset_x2center;
    Chassis.Wheel.ChassisWheels[2].loc_y = -wheel2wheel_y / 2 - Offset_y2center;
    Chassis.Wheel.ChassisWheels[3].loc_x = -wheel2wheel_x / 2 - Offset_x2center;
    Chassis.Wheel.ChassisWheels[3].loc_y = -wheel2wheel_y / 2 - Offset_y2center;

    /*舵轮光电门安装位置*/
    Chassis.Wheel.ChassisWheels[0].Dimmer_angle = 90;
    Chassis.Wheel.ChassisWheels[1].Dimmer_angle = 90;
    Chassis.Wheel.ChassisWheels[2].Dimmer_angle = 90;
    Chassis.Wheel.ChassisWheels[3].Dimmer_angle = 90;

    /*舵轮光电门引脚*/
    Chassis.Wheel.ChassisWheels[0].Dimmer_GPIO_Port = GPIOD;
    Chassis.Wheel.ChassisWheels[1].Dimmer_GPIO_Port = GPIOD;
    Chassis.Wheel.ChassisWheels[2].Dimmer_GPIO_Port = GPIOD;
    Chassis.Wheel.ChassisWheels[3].Dimmer_GPIO_Port = GPIOD;
    Chassis.Wheel.ChassisWheels[0].Dimmer_GPIO_Pin  = GPIO_PIN_15;
    Chassis.Wheel.ChassisWheels[1].Dimmer_GPIO_Pin  = GPIO_PIN_14;
    Chassis.Wheel.ChassisWheels[2].Dimmer_GPIO_Pin  = GPIO_PIN_13;
    Chassis.Wheel.ChassisWheels[3].Dimmer_GPIO_Pin  = GPIO_PIN_12;

    /*舵轮光电门中断控制器*/
    Chassis.Wheel.ChassisWheels[0].Dimmer_hVNIC = EXTI15_10_IRQn;
    Chassis.Wheel.ChassisWheels[1].Dimmer_hVNIC = EXTI15_10_IRQn;
    Chassis.Wheel.ChassisWheels[2].Dimmer_hVNIC = EXTI15_10_IRQn;
    Chassis.Wheel.ChassisWheels[3].Dimmer_hVNIC = EXTI15_10_IRQn;

    /*参数初始化*/
    for (uint8_t i = 0; i < 4; i++) {
        Chassis.Wheel.ChassisWheels[i].Dimmer_detect_flag = false;
        Chassis.Wheel.ChassisWheels[i].rot_pos_offset     = 0;
        Chassis.Wheel.ChassisWheels[i].hDJI               = &(hDJI[i]);
    }

    /*驱动电机减速比初始化*/
    Chassis.Wheel.ChassisWheels[0].speed_ratio = vel_ratio / wheel_radius / 2 * wheel_motor_pole;
    Chassis.Wheel.ChassisWheels[1].speed_ratio = vel_ratio / wheel_radius / 2 * wheel_motor_pole;
    Chassis.Wheel.ChassisWheels[2].speed_ratio = vel_ratio / wheel_radius / 2 * wheel_motor_pole;
    Chassis.Wheel.ChassisWheels[3].speed_ratio = vel_ratio / wheel_radius / 2 * wheel_motor_pole;

    /*舵电机减速比初始化*/
    for (uint8_t i = 0; i < 4; i++) {
        Chassis.Wheel.ChassisWheels[i].rot_pos_ratio = pos_ratio;
    }

    /*状态初始化*/
    Chassis.State.ChassisState = Stop;

    /*互斥量初始化*/
    Chassis.Control.xMutex_Control   = xSemaphoreCreateRecursiveMutex();
    Chassis.Position.xMutex_Position = xSemaphoreCreateRecursiveMutex();
    Chassis.State.xMutex_State       = xSemaphoreCreateRecursiveMutex();
    Chassis.Wheel.xMutex_Wheel       = xSemaphoreCreateRecursiveMutex();
}

/**
 * @brief 底盘逆解
 * @auther: Chen YiTong 3083697520
 * @return
 */
void FourSteeringInverseCalculate()
{
    /*拷贝数据*/
    xSemaphoreTakeRecursive(Chassis.Control.xMutex_Control, (TickType_t)10);
    double vx = Chassis.Control.ChassisControl.velocity.x;
    double vy = Chassis.Control.ChassisControl.velocity.y;
    double vw = Chassis.Control.ChassisControl.velocity.w;
    xSemaphoreGiveRecursive(Chassis.Control.xMutex_Control);

    xSemaphoreTakeRecursive(Chassis.Wheel.xMutex_Wheel, (TickType_t)10);
    WHEEL_COMPONENT Wheel_tmp[4];
    memcpy(Wheel_tmp, Chassis.Wheel.ChassisWheels, sizeof(Wheel_tmp));
    xSemaphoreGiveRecursive(Chassis.Wheel.xMutex_Wheel);

    /*运动学解算*/
    for (uint8_t i = 0; i < 4; i++) {
        double Wheel_vx, Wheel_vy;
        Wheel_vx               = vx - Wheel_tmp[i].loc_y * vw / 180 * M_PI;
        Wheel_vy               = vy + Wheel_tmp[i].loc_x * vw / 180 * M_PI;
        Wheel_tmp[i].exp_speed = sqrt(Wheel_vx * Wheel_vx + Wheel_vy * Wheel_vy);
        if (Wheel_tmp[i].exp_speed > 500) {
            Wheel_tmp[i].exp_rot_pos = atan2(Wheel_vx, Wheel_vy) / M_PI * 180;
        }
    }

    /*拷贝数据*/
    xSemaphoreTakeRecursive(Chassis.Wheel.xMutex_Wheel, (TickType_t)10);
    memcpy(Chassis.Wheel.ChassisWheels, Wheel_tmp, sizeof(Wheel_tmp));
    xSemaphoreGiveRecursive(Chassis.Wheel.xMutex_Wheel);
}

/**
 * @brief 舵轮初始化
 * @auther: Chen YiTong 3083697520
 * @return
 */
void WheelCorrect()
{
    float Wheel_StartPos[4]     = {0};
    float Wheel_TargetPos[4]    = {0};
    float Wheel_CurrentAngle[4] = {0};
    float Wheel_Different[4]    = {0};
    for (uint8_t i = 0; i < 4; i++) {
        Wheel_ReadNowRotPos(&(Chassis.Wheel.ChassisWheels[i]));
        Wheel_StartPos[i]  = Chassis.Wheel.ChassisWheels[i].now_rot_pos;
        Wheel_TargetPos[i] = Wheel_StartPos[i] + 380;
    }
    for (uint8_t i = 0; i < 4; i++) {
        HAL_NVIC_EnableIRQ(Chassis.Wheel.ChassisWheels[i].Dimmer_hVNIC);
    }
    vTaskDelay(10);

    TickType_t WheelCorrect_StartTick = xTaskGetTickCount();
    bool isArray                      = false;
    do {
        TickType_t WheelCorrect_NowTick     = xTaskGetTickCount();
        TickType_t WheelCorrect_ElapsedTick = WheelCorrect_NowTick - WheelCorrect_StartTick;
        float timeSec                       = (WheelCorrect_ElapsedTick / (1000.0)); // 获取当前时间/s
        for (uint8_t i = 0; i < 4; i++) {

            VelocityPlanning(Wheel_StartPos[i],
                             WheelCorrect_MaxVelocity,
                             WheelCorrecting_Acceleration,
                             Wheel_TargetPos[i], timeSec,
                             &(Wheel_CurrentAngle[i]));
            xSemaphoreTakeRecursive(Chassis.Wheel.xMutex_Wheel, 10);
            Chassis.Wheel.ChassisWheels[i].exp_speed   = 0;
            Chassis.Wheel.ChassisWheels[i].exp_rot_pos = Wheel_CurrentAngle[i];
            xSemaphoreGiveRecursive(Chassis.Wheel.xMutex_Wheel);
            Wheel_Different[i] = fabs(Wheel_CurrentAngle[i] - Wheel_TargetPos[i]);
        }
        if (Wheel_Different[0] < 1 && Wheel_Different[1] < 1 && Wheel_Different[2] < 1 && Wheel_Different[3] < 1) {
            isArray = true;
        }
    } while (!isArray);
    // for (uint8_t i = 0; i < 4; i++) {
    //     bool isArray = false;
    //     TickType_t WheelCorrect_StartTick = xTaskGetTickCount();
    //     do {
    //         TickType_t WheelCorrect_NowTick     = xTaskGetTickCount();
    //         TickType_t WheelCorrect_ElapsedTick = WheelCorrect_NowTick - WheelCorrect_StartTick;
    //         float timeSec                       = (WheelCorrect_ElapsedTick / (1000.0)); // 获取当前时间/s
    //         VelocityPlanning(Wheel_StartPos[i],
    //                          WheelCorrect_MaxVelocity,
    //                          WheelCorrecting_Acceleration,
    //                          Wheel_TargetPos[i],
    //                          timeSec,
    //                          &(Wheel_CurrentAngle[i]));
    //         xSemaphoreTakeRecursive(Chassis.Wheel.xMutex_Wheel, 10);
    //         Chassis.Wheel.ChassisWheels[i].exp_speed   = 0;
    //         Chassis.Wheel.ChassisWheels[i].exp_rot_pos = Wheel_CurrentAngle[i];
    //         xSemaphoreGiveRecursive(Chassis.Wheel.xMutex_Wheel);
    //         Wheel_Different[i] = fabs(Wheel_CurrentAngle[i] - Wheel_TargetPos[i]);
    //         if (Wheel_Different[i]<1) {
    //             isArray = true;
    //         }
    //     } while (!isArray);
    // }

    for (uint8_t i = 0; i < 4; i++) {
        HAL_NVIC_DisableIRQ(Chassis.Wheel.ChassisWheels[i].Dimmer_hVNIC);
    }

    xSemaphoreTakeRecursive(Chassis.Wheel.xMutex_Wheel, 10);
    for (uint8_t i = 0; i < 4; i++) {
        double Dimmer_avg_pos = (Chassis.Wheel.ChassisWheels[i].Dimmer_on_pos + Chassis.Wheel.ChassisWheels[i].Dimmer_off_pos) / 2;
        Chassis.Wheel.ChassisWheels[i].rot_pos_offset += (Dimmer_avg_pos - Chassis.Wheel.ChassisWheels[i].Dimmer_angle);
        Chassis.Wheel.ChassisWheels[i].exp_rot_pos = 0;
    }
    xSemaphoreGiveRecursive(Chassis.Wheel.xMutex_Wheel);
    vTaskDelay(10);
}