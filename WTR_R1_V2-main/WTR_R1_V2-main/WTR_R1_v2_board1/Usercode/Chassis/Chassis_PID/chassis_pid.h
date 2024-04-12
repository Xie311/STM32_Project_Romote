#ifndef __CHASSIS_PID_H
#define __CHASSIS_PID_H

#include "chassis_start.h"

/************************类型定义*****************************/

typedef struct PID
{
    __IO float SetPoint;    /* PID目标值 */
    __IO float ActualValue; /* PID输出值 */
    __IO float SumError;    /* PID误差 */
    __IO float Proportion;  /* P */
    __IO float Integral;    /* I */
    __IO float Derivative;  /* D */
    __IO float Error;       /* Error[1] */
    __IO float LastError;   /* Error[-1] */
    __IO float PrevError;   /* Error[-2] */
} PID;

extern PID chassis_pid;

/**************************用户函数****************************/
void Chassis_PID_Init(PID *upid, float KP, float KI, float KD);
float Chassis_PID_Calc(PID *upid, float Feedback_value);

#endif