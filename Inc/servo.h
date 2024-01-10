#ifndef __SERVO_H
#define __SERVO_H

//#include "stm32f1xx_hal.h "

extern uint16_t PWM_CHANNELHandle[3];
void Servo_SetAngle(int i,float Angle);

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
