#include "chassis_pid.h"

PID chassis_pid; // 底盘方向控制PID

/**
 * @brief       pid初始化
 * @param       KP  比例常数
 * @param       KI  积分常数
 * @param       KD  微分常数
 * @retval      无
 */
void Chassis_PID_Init(PID *upid, float KP, float KI, float KD)
{
    upid->SetPoint    = 0;   /* 设定目标值 */
    upid->ActualValue = 0.0; /* 期望输出值 */
    upid->SumError    = 0.0; /* 积分值 */
    upid->Error       = 0.0; /* Error[1] */
    upid->LastError   = 0.0; /* Error[-1] */
    upid->PrevError   = 0.0; /* Error[-2] */
    upid->Proportion  = KP;  /* 比例常数 Proportional Const */
    upid->Integral    = KI;  /* 积分常数 Integral Const */
    upid->Derivative  = KD;  /* 微分常数 Derivative Const */
}

/**
 * @brief       pid闭环控制
 * @param       *PID：PID结构体变量地址
 * @param       Feedback_value：当前实际值
 * @retval      期望输出值
 */
float Chassis_PID_Calc(PID *upid, float Feedback_value)
{
    upid->Error = (float)(upid->SetPoint - Feedback_value); /* 计算偏差 */

    upid->SumError += upid->Error;
    upid->ActualValue = (upid->Proportion * upid->Error)                        /* 比例环节 */
                        + (upid->Integral * upid->SumError)                     /* 积分环节 */
                        + (upid->Derivative * (upid->Error - upid->LastError)); /* 微分环节 */
    upid->LastError = upid->Error;

    return (upid->ActualValue); /* 返回计算后输出的数值 */
}
