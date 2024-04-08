/*
 * @Author Chen Yitong
 * @Date 2023-09-22 22:19:27
 * @LastEditors: 15867773077
 * @LastEditTime: 2024-01-08 20:27:10
 * @FilePath: \MyChassisTry - 副本\UserCode\Chassis\Start\Chassis_Start.c
 * @brief 底盘启动文件
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */
#include "Chassis_Start.h"

/**
 * @brief 默认任务，启动其他线程
 * @return {*}
 */
void StartDefaultTask(void const *argument)
{

    //================初始化====================
    Chassis_Init();              // 底盘结构体初始化
    Chassis_RemoteCtl_Init();    // 遥控初始化
    Chassis_StateMachine_Init(); // 状态机初始化
    Chassis_Servo_Init();        // 底盘伺服初始化
    vTaskDelay(3000);
    //================启动线程==================
    Chassis_Servo_TaskStart();        // 底盘伺服进程启动
    Chassis_StateMachine_TaskStart(); // 状态机进程启动

    for (;;) {
        HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin); // 程序运行指示灯
        osDelay(1000);
    }
}