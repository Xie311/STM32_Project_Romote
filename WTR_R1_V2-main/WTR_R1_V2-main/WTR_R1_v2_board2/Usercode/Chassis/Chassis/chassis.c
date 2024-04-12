#include "chassis.h"

/************************************************************************/
osThreadId_t can_message_TaskHandle;
const osThreadAttr_t can_message_Task_attributes = {
    .name       = "can_message_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal1,
};
/************************************************************************/
/**
 * @brief   底盘初始化
 */
void Chassis_Init(void)
{
    can1.CAN_Rx_Filter_Init();
    can1.CAN_Start();
    can1.CAN_Interrupt_Init();
    hDJI[0].motorType = M2006; // 右侧取苗爪升降电机
    hDJI[1].motorType = M2006; // 右侧存储机构运动电机
    hDJI[2].motorType = M2006; // 左侧取苗爪升降电机
    hDJI[3].motorType = M2006; // 左侧存储机构运动电机
    DJI_Init();
}

/**
 * @brief   底盘电机CAN消息发送线程创建
 */
void Chassis_CAN_Message_TaskStart(void)
{
    can_message_TaskHandle = osThreadNew(CAN_Message_Task, NULL, &can_message_Task_attributes);
}

/**
 * @brief   底盘电机CAN消息发送线程
 */
void CAN_Message_Task(void *argument)
{
    for (;;) {
        positionServo(right_land_angle, &hDJI[0]);
        positionServo(right_deposit_angle, &hDJI[1]);
        positionServo(left_land_angle, &hDJI[2]);
        positionServo(left_deposit_angle, &hDJI[3]);
        CanTransmit_DJI_1234(hDJI[0].speedPID.output, hDJI[1].speedPID.output, hDJI[2].speedPID.output, hDJI[3].speedPID.output);
        osDelay(5);
    }
}