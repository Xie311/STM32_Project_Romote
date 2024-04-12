#ifndef __CHASSIS_MESSAGE_H
#define __CHASSIS_MESSAGE_H

#include "chassis_start.h"

typedef struct Chassis_Message {
    uint8_t grip_message;         // 夹取信息
    uint8_t direction_message;    // 左右信息
    uint8_t grip_resolve_message; // 抓取/释放模式选择
    uint8_t ball_seed_mode;       // 球/苗模式选择
} Chassis_Message;

/************************ 用户定义 ***************************/
#define BOARD_MESSAGE_UART_HANDLE huart7 // 串口选项
#define BOARD_MESSAGE_UART        UART7  // 串口选项

extern uint8_t board_message_buffer[8];
extern Chassis_Message chassis_message;

int Board_Message_Decode(void);
void Board_Message_Init(void);

#endif