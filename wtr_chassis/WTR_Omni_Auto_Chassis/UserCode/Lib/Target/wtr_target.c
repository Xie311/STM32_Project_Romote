/**
 * @file wtr_target.c
 * @author X311
 * @brief 接受上位机数据
 * @version 0.0
 * @date 2024-04-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "wtr_target.h"
#include <stdio.h>

Tar_t Tar_Data = {0};
uint8_t tar_buffer[30];

// 串口中断使能
//__HAL_UART_ENABLE_IT(&huart8, UART_IT_RXNE);

/**
 * @brief 对上位机传来数据进行解码
 * 
 */
uint8_t Tar_Decode(void){
    //To be continued
    return 0;
}
