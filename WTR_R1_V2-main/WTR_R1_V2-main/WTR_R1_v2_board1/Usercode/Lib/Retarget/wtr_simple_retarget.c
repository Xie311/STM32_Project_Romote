/**
 * @file uart_printf.c
 * @author TITH (1023515576@qq.com)
 * @brief printf()串口重定向
 * @version 1.0
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __UART_PRINTF_C__
#define __UART_PRINTF_C__
#include "usart.h"
#include "stm32f4xx.h"

#include <stdio.h>

#define UART_Printf_Config_huart huart6

__attribute__((weak)) int _write(int file, uint8_t *ptr, int len)
{
    if (HAL_UART_Transmit(&UART_Printf_Config_huart, ptr, len, 100) != HAL_OK) {
        Error_Handler();
    }
    return len;
}

#endif