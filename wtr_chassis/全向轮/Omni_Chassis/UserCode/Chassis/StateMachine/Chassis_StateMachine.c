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
        vPortEnterCritical();                                   // 进入临界区，防止多个任务同时访问 RemoteCtl_RawData
        Remote_t RemoteCtl_RawData_tmp = RemoteCtl_RawData;     // 复制遥控器数据到临时变量
        vPortExitCritical();                                    // 退出临界区
        switch (RemoteCtl_RawData_tmp.left) {
            case Stop:
                // 获取底盘控制的互斥锁，防止多任务同时修改底盘控制数据
                xSemaphoreTakeRecursive(ChassisControl.xMutex_control, portMAX_DELAY);
                ChassisControl.velocity.x = 0;
                ChassisControl.velocity.y = 0;
                ChassisControl.velocity.w = 0;
                // 释放底盘控制的互斥锁
                xSemaphoreGiveRecursive(ChassisControl.xMutex_control);
                break;
            case Run:
                // 获取底盘控制的互斥锁，防止多任务同时修改底盘控制数据
                xSemaphoreTakeRecursive(ChassisControl.xMutex_control, portMAX_DELAY);
                // 根据遥控器输入设置底盘速度，同时进行死区处理
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch0 - 1024) * 0.001, &(ChassisControl.velocity.x), 0.05);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch1 - 1024) * 0.001, &(ChassisControl.velocity.y), 0.05);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch2 - 1024) * 0.001, &(ChassisControl.velocity.w), 0.05);
                // 释放底盘控制的互斥锁
                xSemaphoreGiveRecursive(ChassisControl.xMutex_control);
                break;
            case Correcting:

                break;
            default:
                break;
        }
        osDelay(10);
    }
}

/**
 * @brief: 状态机线程启动
 * @return {*}
 */
void Chassis_StateMachine_TaskStart()
{
    const osThreadAttr_t Chassis_StateMachine_attr = 
    {
        .name = "Chassis_StateMachine",
        .priority = osPriorityAboveNormal,
        .stack_size = 1024
    };
    osThreadNew(Chassis_StateMachine_Task, NULL, &Chassis_StateMachine_attr);

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
    // const osMutexAttr_t mutex_attr = 
    // {
    //     .name = "Chassis_Mutex"
    // };
    
    // ChassisControl.xMutex_control = osMutexNew(&mutex_attr);
    // ChassisState.xMutex_control = osMutexNew(&mutex_attr);
    // WheelComponent.xMutex_wheel = osMutexNew(&mutex_attr);
 }

void Chassis_SteerinfWheelCorrect(){
}