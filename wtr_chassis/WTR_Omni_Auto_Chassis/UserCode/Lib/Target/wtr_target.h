/**
 * @file wtr_target.h
 * @author X311
 * @brief 
 * @version 0.0
 * @date 2024-04-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __WTR_TARGET_H
#define __WTR_TARGET_H

#include "stm32f4xx.h"
#include "usart.h"
/************************ 用户定义 ***************************/
#define Tar_UART_HANDLE huart6 // 串口选项
#define Tar_UART        USART6  // 串口选项

/************************ 结构体定义 ***************************/

typedef struct Tar_t {
    float pos_x;   // x坐标
    float pos_y;   // y坐标

    float vx;      // x轴速度
    float vy;      // y轴速度
    float vw;      // 角速度w

    int state;     //状态
} Tar_t; 

extern Tar_t Tar_Data;
extern uint8_t tar_buffer[30];

uint8_t Tar_Decode(void);

#endif