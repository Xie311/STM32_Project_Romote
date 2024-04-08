/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 12:54:05
 * @LastEditors: 15867773077
 * @LastEditTime: 2024-01-08 20:31:52
 * @FilePath: \MyChassisTry - 副本\UserCode\Chassis\Config\Chassis_commen.h
 * @Brief: 底盘结构体定义
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#ifndef __CHASSIS_COMMEN_H__
#define __CHASSIS_COMMEN_H__

#include "Chassis.h"
typedef enum {
    Stop       = 1, // 停止
    Run        = 3, // 启动
    Correcting = 2, // 底盘校准
} CHASSIS_STATE;    // 底盘状态

typedef __IO struct
{
    double loc_x; // 舵轮在机器人坐标系中的位置
    double loc_y; // 舵轮在机器人坐标系中的位置

    double exp_rot_pos;   // 期望旋转角位置 degree
    double exp_rot_angle; // 期望旋转角总位置 degree
    double exp_speed;     // 期望速度 mm/s
    double now_rot_pos;   // 当前旋转角度 degree
    double now_rot_angle; // 当前旋转总角度 degree

    double rot_pos_offset;      // 角位置偏置，(rot_pos + rot_pos_offset) * rot_pos_ratio = PID 的值
    double rot_pos_ratio;       // 角位置比率，(rot_pos + rot_pos_offset) * rot_pos_ratio = PID 的值
    double speed_ratio;         // 速度比率，speed * speed_ratio = ERPM
    double Dimmer_on_pos;       // 光电门检测到时的 now_rot_pos
    double Dimmer_off_pos;      // 光电门失去时的 now_rot_pos
    double Dimmer_angle;        // 光电门所在的角度
    uint8_t Dimmer_detect_flag; // 光电门检测标志位
    uint16_t Dimmer_GPIO_Port;  // 光电门GPIO引脚
    uint16_t Dimmer_GPIO_Pin;   // 光电门GPIO引脚
    IRQn_Type Dimmer_hVNIC;     // 光电门GPIO中断
    __IO DJI_t *hDJI;           // 舵方向电机结构体
    VESC_t hvesc;               // 驱动电机结构体
} WHEEL_COMPONENT;              // 舵轮结构体

typedef __IO struct {
    __IO struct {
        __IO struct
        {
            double x; // mm/s
            double y; // mm/s
            double w; // degree/2
            // 位置
        } ChassisPosition;                 // 底盘运动状态
        SemaphoreHandle_t xMutex_Position; // 互斥锁
    } Position;

    __IO struct {
        CHASSIS_STATE ChassisState;     // 底盘状态控制
        SemaphoreHandle_t xMutex_State; // 互斥锁
    } State;

    __IO struct {
        __IO struct
        {
            __IO struct {
                double x; // mm/s
                double y; // mm/s
                double w; // degree/s
            } velocity;   // 速度

            __IO struct {
                double x;
                double y;
                double w;
            } position; // 位置

        } ChassisControl;                 // 底盘运动状态
        SemaphoreHandle_t xMutex_Control; // 互斥锁
    } Control;

    __IO struct {
        WHEEL_COMPONENT ChassisWheels[4]; // 舵轮控制
        SemaphoreHandle_t xMutex_Wheel;   // 互斥锁
    } Wheel;

} CHASSIS; // 底盘结构体

#endif // __CHASSIS_COMMEN_H__