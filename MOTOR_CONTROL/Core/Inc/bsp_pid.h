//
// Created by Quanquan on 2023/4/5.
//

#ifndef STM32F103_DEMO1_BSP_PID_H
#define STM32F103_DEMO1_BSP_PID_H

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#define PWM_MAX 900
typedef struct{
    float KP;											//PID参数P
    float KI;											//PID参数I
    float KD;											//PID参数D

    float fdb;											//PID反馈值
    float target;									    //PID目标值
    float cur_error;									//当前误差
    float error[2];										//前两次误差
//    float output;										//输出值
    float outputMax;									//最大输出值的绝对值
    //float outputMin;                  //最小输出值的绝对值用于防抖
}_PID_t;


void PID_Param_init(_PID_t *pid);
void set_pid_target(_PID_t *pid,float temp_target);
void set_p_i_d(_PID_t *pid,float p,float i,float d);
float PID_realize(_PID_t *pid,float fdb);


#endif //STM32F103_DEMO1_BSP_PID_H
