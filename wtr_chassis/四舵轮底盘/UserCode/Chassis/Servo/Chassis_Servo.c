/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 13:49:36
 * @LastEditors: 15867773077
 * @LastEditTime: 2024-01-08 20:37:59
 * @FilePath: \MyChassisTry - 副本\UserCode\Chassis\Servo\Chassis_Servo.c
 * @Brief: 底盘伺服函数,根据状态机输出的底盘速度驱动电机
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#include "Chassis_Servo.h"

/**
 * @brief 读取舵轮位置
 * @auther: Chen YiTong 3083697520
 * @param *wheel
 * @return
 */
void Wheel_ReadNowRotPos(WHEEL_COMPONENT *wheel)
{
    wheel->now_rot_angle = (wheel->hDJI->AxisData.AxisAngle_inDegree / wheel->rot_pos_ratio) - wheel->rot_pos_offset;
    wheel->now_rot_pos   = LoopSimplify(360.0, wheel->now_rot_angle);
}

/**
 * @brief: 底盘电机初始化
 * @return {*}
 */
void Chassis_Servo_Init()
{
    CANFilterInit(&hcan1);
    for (uint8_t i = 0; i < 4; i++) {
        Chassis.Wheel.ChassisWheels[i].hDJI->motorType = M2006; // 电机类型初始化
    }
    DJI_Init(); // 大疆电机初始化
    CanTransmit_DJI_1234(&hcan1, 0, 0, 0, 0);
}

/**
 * @brief: 伺服线程
 * @return {*}
 */
void Chassis_Servo_Task(void const *argument)
{
    for (;;) {
        TickType_t StartTime = xTaskGetTickCount();

        /*拷贝舵轮数据*/
        xSemaphoreTakeRecursive(Chassis.Wheel.xMutex_Wheel, portMAX_DELAY);
        for (uint8_t i = 0; i < 4; i++) {
            Wheel_ReadNowRotPos(&(Chassis.Wheel.ChassisWheels[i]));
        }
        WHEEL_COMPONENT ChassisWheel_tmp[4];
        memcpy(ChassisWheel_tmp, Chassis.Wheel.ChassisWheels, sizeof(ChassisWheel_tmp));
        xSemaphoreGiveRecursive(Chassis.Wheel.xMutex_Wheel);

        /*计算预期总角度exp_rot_angle，计算最小旋转角*/
        for (uint8_t i = 0; i < 4; i++) {
            if (ChassisWheel_tmp[i].exp_speed < 0) {
                ChassisWheel_tmp[i].exp_rot_pos = LoopSimplify(360.0, 180.0 + ChassisWheel_tmp[i].exp_rot_pos);
                ChassisWheel_tmp[i].exp_speed   = -ChassisWheel_tmp[i].exp_speed;
            }
            double delta_angle = LoopSimplify(360.0, ChassisWheel_tmp[i].exp_rot_pos - ChassisWheel_tmp[i].now_rot_pos);
            if (delta_angle >= 90.0) {
                delta_angle -= 180.0;
                ChassisWheel_tmp[i].exp_speed = -ChassisWheel_tmp[i].exp_speed;
            } else if (delta_angle <= (-90.0)) {
                delta_angle += 180.0;
                ChassisWheel_tmp[i].exp_speed = -ChassisWheel_tmp[i].exp_speed;
            }
            ChassisWheel_tmp[i].exp_rot_angle = ChassisWheel_tmp[i].now_rot_angle + delta_angle;
        }

        /*速度设定-无中断保护*/
        for (int i = 0; i < 4; i++) {
            positionServo((ChassisWheel_tmp[i].exp_rot_angle + ChassisWheel_tmp[i].rot_pos_offset) * ChassisWheel_tmp[i].rot_pos_ratio,
                          &(hDJI[i]));
            VESC_CAN_SET_ERPM(&(ChassisWheel_tmp[i].hvesc),
                              ChassisWheel_tmp[i].exp_speed * ChassisWheel_tmp[i].speed_ratio);
        }
        CanTransmit_DJI_1234(&hcan_Dji,
                             hDJI[0].speedPID.output,
                             hDJI[1].speedPID.output,
                             hDJI[2].speedPID.output,
                             hDJI[3].speedPID.output);

        // /*速度设定-有中断保护*/
        // /* 拷贝电机状态*/
        // vPortEnterCritical();
        // DJI_t hDJI_tmp[4];
        // for (int i = 0; i < 4; i++) { memcpy(&(hDJI_tmp[i]), Chassis.Wheel.ChassisWheels[i].hDJI, sizeof(DJI_t)); }
        // vPortExitCritical();
        // /*速度设定*/
        // for (int i = 1; i < 2; i++) {
        //     positionServo((ChassisWheel_tmp[i].exp_rot_angle + ChassisWheel_tmp[i].rot_pos_offset) * ChassisWheel_tmp[i].rot_pos_ratio,
        //                   &(hDJI_tmp[i]));
        //     VESC_CAN_SET_ERPM(&(ChassisWheel_tmp[i].hvesc),
        //                       ChassisWheel_tmp[i].exp_speed * ChassisWheel_tmp[i].speed_ratio);
        // }
        // CanTransmit_DJI_1234(&hcan_Dji,
        //                      hDJI_tmp[0].speedPID.output,
        //                      hDJI_tmp[1].speedPID.output,
        //                      hDJI_tmp[2].speedPID.output,
        //                      hDJI_tmp[3].speedPID.output);
        // /*反馈数据*/
        // vPortEnterCritical();
        // for (int i = 0; i < 4; i++) { memcpy(Chassis.Wheel.ChassisWheels[i].hDJI, &(hDJI_tmp[i]), sizeof(DJI_t)); }
        // vPortExitCritical();
        
        /*拷贝数据*/
        xSemaphoreTakeRecursive(Chassis.Wheel.xMutex_Wheel, portMAX_DELAY);
        memcpy(Chassis.Wheel.ChassisWheels, ChassisWheel_tmp, sizeof(ChassisWheel_tmp));
        xSemaphoreGiveRecursive(Chassis.Wheel.xMutex_Wheel);

        vTaskDelayUntil(&StartTime, 5);
    }
}

/**
 * @brief: 启动底盘伺服线程
 * @return {*}
 */
void Chassis_Servo_TaskStart()
{
    osThreadDef(Chassis_Servo, Chassis_Servo_Task, osPriorityHigh, 0, 1024);
    osThreadCreate(osThread(Chassis_Servo), NULL);
}
