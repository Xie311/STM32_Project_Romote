#include "wtr_chassis.h"

/*****************************************************************************/

int32_t v_1, v_2, v_3, v_4; // 底盘速度
osThreadId_t can_message_TaskHandle;
const osThreadAttr_t can_message_Task_attributes = {
    .name       = "can_message_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityHigh1,
};

/*****************************************************************************/
/**
 * @brief   底盘初始化
 */
void Chassis_Init(void)
{
    can1.CAN_Rx_Filter_Init();
    can1.CAN_Start();
    can1.CAN_Interrupt_Init();
    hDJI[0].motorType = M3508;
    hDJI[1].motorType = M3508;
    hDJI[2].motorType = M3508;
    hDJI[3].motorType = M3508;
    hDJI[4].motorType = M3508;
    hDJI[5].motorType = M2006; // 5号电机，用于控制球的发射
    hDJI[6].motorType = M3508; // 6号电机，发射左摩擦轮
    hDJI[7].motorType = M3508; // 7号电机，发射右摩擦轮
    DJI_Init();
    Chassis_PID_Init(&chassis_pid, 0.4, 0.0, 0.1);
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
        speedServo(v_1, &hDJI[0]);
        speedServo(v_2, &hDJI[1]);
        speedServo(v_3, &hDJI[2]);
        speedServo(v_4, &hDJI[3]);
        positionServo(arm_angle, &hDJI[4]);
        positionServo(rail_angle, &hDJI[5]);
        speedServo(friction_speed, &hDJI[6]);
        speedServo(- friction_speed, &hDJI[7]);
        CanTransmit_DJI_1234(hDJI[0].speedPID.output, hDJI[1].speedPID.output, hDJI[2].speedPID.output, hDJI[3].speedPID.output);
        CanTransmit_DJI_5678(hDJI[4].speedPID.output, hDJI[5].speedPID.output, hDJI[6].speedPID.output, hDJI[7].speedPID.output);
        osDelay(5);
    }
}

/**
 * @brief   逆运动学方程，输入速度求解电机转速
 * @param   vx          底盘前向速度，单位m/s
 * @param   vy          底盘左横向速度，单位m/s
 * @param   wc          转向角速度，使用弧度，单位rad/s
 */
void Inverse_kinematic_equation(float vx, float vy, float wc, int32_t *_v_1, int32_t *_v_2, int32_t *_v_3, int32_t *_v_4)
{
    float v1, v2, v3, v4;
    v1 = (float)((sqrt(2) * vx / 2 - sqrt(2) * vy / 2 - wc * r) / R);
    v2 = (float)((-sqrt(2) * vx / 2 - sqrt(2) * vy / 2 - wc * r) / R);
    v3 = (float)((-sqrt(2) * vx / 2 + sqrt(2) * vy / 2 - wc * r) / R);
    v4 = (float)((sqrt(2) * vx / 2 + sqrt(2) * vy / 2 - wc * r) / R);

    *_v_1 = (int32_t)(v1 * 60.0) / (2 * PI);
    *_v_2 = (int32_t)(v2 * 60.0) / (2 * PI);
    *_v_3 = (int32_t)(v3 * 60.0) / (2 * PI);
    *_v_4 = (int32_t)(v4 * 60.0) / (2 * PI);
}
