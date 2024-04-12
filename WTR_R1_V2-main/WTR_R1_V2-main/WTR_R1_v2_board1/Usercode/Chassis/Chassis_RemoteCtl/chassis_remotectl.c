#include "chassis_remotectl.h"

/****************************变量定义*******************************/

osThreadId_t remotectl_TaskHandle;
const osThreadAttr_t remotectl_Task_attributes = {
    .name       = "remotectl_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/******************************************************************/
/**
 * @brief   遥控器初始化
 */
void Chassis_Joytick_Init(void)
{
    HAL_UART_Receive_DMA(&huart1, as69_buffer, 18);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

/**
 * @brief   遥控器线程开启
 */
void Chassis_Remotectl_TaskStart(void)
{
    remotectl_TaskHandle = osThreadNew(Chassis_RemoteCtl_Task, NULL, &remotectl_Task_attributes);
}

/**
 * @brief   遥控器线程
 */
void Chassis_RemoteCtl_Task(void *argument)
{
    for (;;) {
        float mvx, mvy, mwc;
        mvx = (float)((as69_data.ch1 - 1024) * 200) / 4000.0;
        mvy = -(float)((as69_data.ch0 - 1024) * 200) / 4000.0;
        chassis_pid.SetPoint = 0.0;
        mwc = Chassis_PID_Calc(&chassis_pid, OPS_Data.z_angle);
        Inverse_kinematic_equation(mvx, mvy, mwc, &v_1, &v_2, &v_3, &v_4);
        osDelay(5);
    }
}