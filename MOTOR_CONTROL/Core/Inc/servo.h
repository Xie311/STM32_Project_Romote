#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f1xx_hal.h "
extern TIM_HandleTypeDef* ServoPWMhandle[3];
extern uint16_t PWM_CHANNELHandle[3];
void Servo_SetAngle(int i,float Angle);
//void Servo_ReSetAngle(int i,float angle);
#endif

/*
void Servo_Init(uint8_t i)
{
    servo[i].TIM_PWMHandle=htim1;
    servo[i].PWM_CHANNELHanle=CHANNEL[i];
}

void Servo_SetAngle(float Angle)
{
    float duty=Angle/180*2000+500;
    if(duty>2500)  duty=2500;
    __HAL_TIM_SetCompare(TIM_PWMHandle,PWM_CHANNELHanle,duty);
}
*/
