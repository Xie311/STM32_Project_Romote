/*
 * @Author: Chen Yitong 3083697520@qq.com
 * @Date: 2023-09-23 11:33:41
 * @LastEditors: Chen Yitong 
 * @LastEditTime: 2023-10-21 22:04:06
 * @FilePath: \WTR_Chassis\麦克纳姆轮\UserCode\Chassis\StateMachine\Chassis_StateMachine.c
 * @brief 底盘状态机
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */
#include "Chassis_StateMachine.h"
CHASSIS_MOVING_STATE ChassisControl;
CHASSIS_MOVING_STATE ChassisState;

/**
 * @brief: 状态机线程
 * @return {*}
 */
void Chassis_StateMachine_Task(void const *argument)
{
    for (;;) {
        vPortEnterCritical();
        Remote_t RemoteCtl_RawData_tmp = RemoteCtl_RawData;
        vPortExitCritical();
        switch (RemoteCtl_RawData_tmp.left) {
            case Stop:
                xSemaphoreTakeRecursive(ChassisControl.xMutex_control, portMAX_DELAY);
                ChassisControl.velocity.x = 0;
                ChassisControl.velocity.y = 0;
                ChassisControl.velocity.w = 0;
                xSemaphoreGiveRecursive(ChassisControl.xMutex_control);
                break;
            case Run:
                xSemaphoreTakeRecursive(ChassisControl.xMutex_control, portMAX_DELAY);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch0 - 1024) * 0.001, &(ChassisControl.velocity.x), 0.05);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch1 - 1024) * 0.001, &(ChassisControl.velocity.y), 0.05);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch2 - 1024) * 0.001, &(ChassisControl.velocity.w), 0.05);
                xSemaphoreGiveRecursive(ChassisControl.xMutex_control);
                break;
            case Correcting:

                break;
            default:
                break;
        }
        vTaskDelay(10);
    }
}

/**
 * @brief: 状态机线程启动
 * @return {*}
 */
void Chassis_StateMachine_TaskStart()
{
    osThreadDef(Chassis_StateMachine, Chassis_StateMachine_Task, osPriorityAboveNormal, 0, 1024);
    osThreadCreate(osThread(Chassis_StateMachine), NULL);
}

/**
 * @brief 初始化状态机
 * @return {*}
 */

void Chassis_StateMachine_Init()
{
    ChassisControl.xMutex_control = xSemaphoreCreateRecursiveMutex();
    ChassisState.xMutex_control   = xSemaphoreCreateRecursiveMutex();
    WheelComponent.xMutex_wheel   = xSemaphoreCreateRecursiveMutex();
}

void Chassis_SteerinfWheelCorrect(){
}