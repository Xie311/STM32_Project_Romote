/**
 * @brief   底盘启动线程
 */
#include "chassis_start.h"

/**
 * @brief   底盘总初始化段
 */
void StartDefaultTask(void *argument)
{
    /**********************外设及硬件初始化***********************/
    Servo_Init();         // 舵机初始化
    Chassis_Init();       // DJI 电机初始化
    Board_Message_Init(); // 板间通讯初始化
    osDelay(2000);
    Unitree_User_Init(); // Unitree电机初始化
    osDelay(5000);
    /***************************线程开启*************************/
    Debug_TaskStart();               // 调试线程
    Chassis_CAN_Message_TaskStart(); // CAN消息发送线程
    Chassis_State_Mechine_Start();   // 状态机线程
    for (;;) {
        osDelay(1000);
    }
}