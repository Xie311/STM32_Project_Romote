#include "wtr_servo.h"
#include "tim.h"

/**
 * @brief 左右夹爪动作-----向内收缩
 */
void Servo_In(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1750 - 750);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 750);
}

/**
 * @brief 左右夹爪动作-----向外伸张
 */
void Servo_Out(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1750 - 1250);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 1250);
}

/**
 * @brief 夹爪抓取动作
 */
void Servo_Grip(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1250);
}

/**
 * @brief 夹爪放松动作
 */
void Servo_Reset(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 700);
}

/**
 * @brief 夹爪舵机初始化
 */
void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // 夹爪中央舵机,CCR 700 张开,CCR 1250 夹取
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // 夹爪右舵机
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // 夹爪左舵机,CCR 之和为 1750
    Servo_Reset();
    Servo_Out();
}