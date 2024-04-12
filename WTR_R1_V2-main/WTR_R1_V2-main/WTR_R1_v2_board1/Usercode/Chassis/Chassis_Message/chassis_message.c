#include "chassis_message.h"

/****************************变量定义*******************************/

osThreadId_t chassis_message_TaskHandle;
const osThreadAttr_t chassis_message_Task_attributes = {
    .name       = "chassis_message_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

uint8_t chassis_message_buffer[8] = {0};

/**
 * @brief   板间通讯线程开启
 */
void Chassis_Message_TaskStart(void)
{
    chassis_message_buffer[0]  = 0x0B;
    chassis_message_buffer[1]  = 0x0C;
    chassis_message_buffer[6]  = 0x0C;
    chassis_message_buffer[7]  = 0x0B;
    chassis_message_TaskHandle = osThreadNew(Chassis_Message_Task, NULL, &chassis_message_Task_attributes);
}

/**
 * @brief   板间通讯线程
 */
void Chassis_Message_Task(void *argument)
{
    for (;;) {
        chassis_message_buffer[2] = as69_data.right;
        if (as69_data.ch2 > 1354) {
            chassis_message_buffer[3] = 0xAA; // 左侧
        } else if (as69_data.ch2 > 694 && as69_data.ch2 < 1354) {
            chassis_message_buffer[3] = 0x00;
        } else if (as69_data.ch2 < 694) {
            chassis_message_buffer[3] = 0xBB; // 右侧
        }
        if (as69_data.ch3 > 1354) {
            chassis_message_buffer[4] = 0xAA; // 取苗
        } else if (as69_data.ch3 > 694 && as69_data.ch3 < 1354) {
            chassis_message_buffer[4] = 0x00;
        } else if (as69_data.ch3 < 694) {
            chassis_message_buffer[4] = 0xBB; // 放苗
        }
        if (as69_data.wheel > 1354) {
            chassis_message_buffer[5] = 0xAA; // 苗
            chassis_mode = Seed_Mode;
        } else if (as69_data.wheel > 694 && as69_data.wheel < 1354) {
            chassis_message_buffer[5] = 0x00;
        } else if (as69_data.wheel < 694) {
            chassis_message_buffer[5] = 0xBB; // 球
            chassis_mode = Ball_Mode;
        }
        HAL_UART_Transmit(&huart7, chassis_message_buffer, 8, 0xFF);
        osDelay(5);
    }
}