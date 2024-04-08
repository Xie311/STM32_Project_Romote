#include "tim.h"  //^_^
#include "servo.h"
#include <stdio.h>
TIM_HandleTypeDef* ServoPWMhandle[3]={&htim2,&htim2,&htim3};
uint16_t PWM_CHANNELHandle[3]={TIM_CHANNEL_2,TIM_CHANNEL_4,TIM_CHANNEL_2};

void Servo_SetAngle(int i,float angle)  
{
    float duty=angle/180*200+50;
    if(duty>=250) duty=250;
    __HAL_TIM_SetCompare(ServoPWMhandle[i],PWM_CHANNELHandle[i],duty);
}

/*
void Servo_ReSetAngle(int i,float angle)  
{
    float duty=250-angle/180*200;
    if(duty<=0) duty=0;
    __HAL_TIM_SetCompare(&htim1,PWM_CHANNELHandle[i],duty);
}
*/

