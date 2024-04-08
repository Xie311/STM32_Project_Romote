//
// Created by Quanquan on 2023/4/5.
//

#ifndef STM32F103_DEMO1_MOTOR_CONTROL_H
#define STM32F103_DEMO1_MOTOR_CONTROL_H

//#include "stm32f4xx.h"
//#include "bsp_GPIO_ENABLE.h"
#include "bsp_pid.h"
#include "tim.h"
#include <stdio.h>




#define REDUCTION_RATIO                        30.0         //减速比
#define ENCODER_RESOLUTION                     6.0
#define ENCODER_TOTAL_RESOLUTION             (ENCODER_RESOLUTION * 4)

#define PWM_TIM htim1							//pwm：定时器1


extern uint16_t CHANNEL[4];
extern GPIO_TypeDef* GPIOX[4][2];
extern uint16_t GPIOPIN[4][2];
extern TIM_HandleTypeDef* EncoderHandle[4];
extern uint16_t EncoderChannel[4][2];
//11.24 调试用数组
extern double compileactualval[4];
extern double compileaduty[4];
typedef struct
{
    __IO int32_t Capture_Count;
}encoder_data;

typedef struct
{
    uint8_t MOTOR_DIE;
    uint8_t is_motor_enable;
    uint16_t dutyfactor;
    TIM_HandleTypeDef* TIM_EncoderHandle;
		TIM_HandleTypeDef* TIM_PWMHandle;
    uint16_t PWM_CHANNELHanle;
    uint16_t ENCODERA_CHANNELHanle;
	  uint16_t ENCODERB_CHANNELHanle;
		GPIO_TypeDef* GPIO1;
	  GPIO_TypeDef* GPIO2;
		uint16_t GPIO_Pin1;
	   uint16_t GPIO_Pin2;
    _PID_t motor_pid;
    encoder_data motor_overflow_counter;
    float actual_speed;
}motor_data;

extern motor_data motor[4];

void motor_init(uint8_t i);
void set_motor_speed(motor_data *motor_,int i,float v);

void set_motor_direction(motor_data *motor_,uint8_t dir);

void motor_pid_control(motor_data *motor_,uint16_t i,uint16_t actualval);

void set_motor_enable(motor_data *motor_);

void set_motor_disable(motor_data *motor_);

void motor_start(uint8_t i);

void motor_stop(uint8_t i);

#endif //STM32F103_DEMO1_MOTOR_CONTROL_H
