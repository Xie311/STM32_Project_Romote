#include "chassis_debug.h"

osThreadId_t debug_TaskHandle;
const osThreadAttr_t debug_Task_attributes = {
    .name       = "debug_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/**
 * @brief   遥控器线程开启
 */
void Debug_TaskStart(void)
{
    debug_TaskHandle = osThreadNew(Debug_Task, NULL, &debug_Task_attributes);
}

/**
 * @brief   遥控器线程
 */
void Debug_Task(void *argument)
{
    for (;;) {
        printf("%d\r\n", (int)(OPS_Data.pos_x));
        osDelay(1000);
    }
}