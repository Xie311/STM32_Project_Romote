/*
 * @Author: Chen Yitong 3083697520@qq.com
 * @Date: 2023-09-23 11:33:41
 * @LastEditors: 15867773077
 * @LastEditTime: 2024-01-07 23:34:15
 * @FilePath: \MyChassisTry\UserCode\Chassis\StateMachine\Chassis_StateMachine.c
 * @brief 底盘状态机
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */
#include "Chassis_StateMachine.h"

/**
 * @brief: 状态机线程
 * @return {*}
 */
void Chassis_StateMachine_Task(void const *argument)
{
    WheelCorrect();
    for (;;) {
        TickType_t StartTime = xTaskGetTickCount();
        vPortEnterCritical();
        Remote_t RemoteCtl_RawData_tmp = RemoteCtl_RawData;
        vPortExitCritical();
        switch (RemoteCtl_RawData_tmp.left) {
            case Stop:
                xSemaphoreTakeRecursive(Chassis.Control.xMutex_Control, 10);
                Chassis.Control.ChassisControl.velocity.x = 0;
                Chassis.Control.ChassisControl.velocity.y = 0;
                Chassis.Control.ChassisControl.velocity.w = 0;
                xSemaphoreGiveRecursive(Chassis.Control.xMutex_Control);
                FourSteeringInverseCalculate();
                break;
            case Run:
                xSemaphoreTakeRecursive(Chassis.Control.xMutex_Control, 10);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch0 - 1024) * 20, &(Chassis.Control.ChassisControl.velocity.x), 500);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch1 - 1024) * 20, &(Chassis.Control.ChassisControl.velocity.y), 500);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch2 - 1024) * -3, &(Chassis.Control.ChassisControl.velocity.w), 100);
                printf("%f,%f,%f\n",
                       Chassis.Control.ChassisControl.velocity.x,
                       Chassis.Control.ChassisControl.velocity.y,
                       (Chassis.Control.ChassisControl.velocity.w));
                xSemaphoreGiveRecursive(Chassis.Control.xMutex_Control);
                FourSteeringInverseCalculate();
                break;
            case Correcting:
                WheelCorrect();
                break;
        }
        vTaskDelayUntil(&StartTime, 5);
    }
}

/**
 * @brief: 状态机线程启动
 * @return {*}
 */
void Chassis_StateMachine_TaskStart()
{
    osThreadDef(Chassis_StateMachine, Chassis_StateMachine_Task, osPriorityNormal, 0, 2048);
    osThreadCreate(osThread(Chassis_StateMachine), NULL);
}

/**
 * @brief 初始化状态机
 * @return {*}
 */

void Chassis_StateMachine_Init()
{
    for (uint8_t i = 0; i < 4; i++) {
        HAL_NVIC_DisableIRQ(Chassis.Wheel.ChassisWheels[i].Dimmer_hVNIC); // 停止中断
    }
}
