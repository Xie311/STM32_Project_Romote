/*
 * @Author: Chen Yitong 
 * @Date: 2023-10-09 22:36:04
 * @LastEditors: 15867773077 
 * @LastEditTime: 2024-01-06 12:06:45
 * @FilePath: \MyChassisTry\UserCode\Lib\math\wtr_math.c
 * @Brief: 自用数学库
 * 
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved. 
 */
#include"wtr_math.h"
/**
 * @brief 循环变量化简
 * @param cycle 周期
 * @param value
 * @return double 化简后的值，在[- cycle / 2, cycle / 2] 之间
 */
double LoopSimplify(double cycle, double value)
{
    double mod_value = fmod(value, cycle);

    if (mod_value > cycle / 2) {
        mod_value -= cycle;
    }

    if (mod_value < -cycle / 2) {
        mod_value += cycle;
    }

    return mod_value;
}