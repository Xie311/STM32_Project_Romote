#ifndef __WTR_SERVO_H
#define __WTR_SERVO_H

#include "stm32f4xx.h"
#include "tim.h"

/**
 * 夹爪定义：
 * 右侧夹取臂夹爪： PA0 ----- TIM2_CH1
 * 右侧放苗板：     PA1 ----- TIM2_CH2
 * 右侧挡板:        PA2 ----- TIM2_CH3
 * 左侧夹取臂夹爪： PA3 ----- TIM2_CH4
 * 左侧放苗板：     PI5 ----- TIM8_CH1
 * 左侧挡板：       PI6 ----- TIM8_CH2
 */

#define Right_Left_Gripper_Servo_Timer_Handle  &htim2
#define Right_Left_Gripper_Servo_Timer_Channel TIM_CHANNEL_1

#define Right_Deposit_Servo_Timer_Handle       &htim2
#define Right_Deposit_Servo_Timer_Channel      TIM_CHANNEL_2

#define Right_Baffle_Servo_Timer_Handle        &htim2
#define Right_Baffle_Servo_Timer_Channel       TIM_CHANNEL_3

#define Left_Left_Gripper_Servo_Timer_Handle   &htim2
#define Left_Left_Gripper_Servo_Timer_Channel  TIM_CHANNEL_4

#define Left_Deposit_Servo_Timer_Handle        &htim8
#define Left_Deposit_Servo_Timer_Channel       TIM_CHANNEL_1

#define Left_Baffle_Servo_Timer_Handle         &htim8
#define Left_Baffle_Servo_Timer_Channel        TIM_CHANNEL_2

void Servo_Init(void);
void Right_Servo_Open(void);
void Right_Servo_Grip(void);
void Right_Servo_Buffle_Open(void);
void Right_Servo_Buffle_Close(void);
void Right_Servo_Deposit_Open(void);
void Right_Servo_Deposit_Close(void);
void Left_Servo_Open(void);
void Left_Servo_Grip(void);
void Left_Servo_Buffle_Open(void);
void Left_Servo_Buffle_Close(void);
void Left_Servo_Deposit_Open(void);
void Left_Servo_Deposit_Close(void);

#endif