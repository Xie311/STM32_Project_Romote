//
// Created by budazhushideQuanquan on 2023/4/5.
//
#include "motor_control.h"
#include "io_retargetToUart.h"
#include "stdio.h"
#include "bsp_Mecanum.h"

motor_data motor[4];
uint16_t CHANNEL[4]={TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4};
GPIO_TypeDef *GPIOX[4][2]={{GPIOA,GPIOA},{GPIOA,GPIOA},{GPIOB,GPIOB},{GPIOB,GPIOB}};
uint16_t GPIOPIN[4][2]={{GPIO_PIN_2,GPIO_PIN_3},{GPIO_PIN_4,GPIO_PIN_5},{GPIO_PIN_12,GPIO_PIN_13},{GPIO_PIN_14,GPIO_PIN_15}};
TIM_HandleTypeDef* EncoderHandle[4]={&htim2,&htim2,&htim3,&htim3};
uint16_t EncoderChannel[4][2]={{TIM_CHANNEL_1,TIM_CHANNEL_2},{TIM_CHANNEL_3,TIM_CHANNEL_4},{TIM_CHANNEL_1,TIM_CHANNEL_2},{TIM_CHANNEL_3,TIM_CHANNEL_4}};


//for compile
 double compileactualval[4]={0};
 double compileaduty[4]={0};


void set_motor_speed(motor_data *motor_,int i,float v)    //i:第i个电机(0->3)
{
    motor_->dutyfactor=v;
	 if(motor_->MOTOR_DIE == 1)			//通过dir设置gpio口，真正实现转向
    {
        HAL_GPIO_WritePin(motor_->GPIO1, motor_->GPIO_Pin1,0);
			  HAL_GPIO_WritePin(motor_->GPIO2, motor_->GPIO_Pin2,1);
    }
    else
    {
        HAL_GPIO_WritePin(motor_->GPIO1, motor_->GPIO_Pin1,1);
			  HAL_GPIO_WritePin(motor_->GPIO2, motor_->GPIO_Pin2,0);
    }
		
	__HAL_TIM_SET_COMPARE(&PWM_TIM,CHANNEL[i],motor_->dutyfactor);
	//printf("duty%d=%d\n",i,motor_->dutyfactor);
		compileaduty[i]=motor_->dutyfactor;
}

void set_motor_direction(motor_data *motor_, uint8_t dir)
{
    motor_->MOTOR_DIE=dir;
}

void set_motor_enable(motor_data *motor_)
{
    motor_->is_motor_enable=1;
}

void set_motor_disable(motor_data *motor_)
{
    motor_->is_motor_enable=0;
}

void motor_init(uint8_t i)
{
    motor[i].MOTOR_DIE=0;
    motor[i].dutyfactor=0;
    motor[i].is_motor_enable=0;
    motor[i].actual_speed=0;
    motor[i].TIM_EncoderHandle=EncoderHandle[i];
	  motor[i].ENCODERA_CHANNELHanle=EncoderChannel[i][0];
	  motor[i].ENCODERB_CHANNELHanle=EncoderChannel[i][1];
    motor[i].TIM_PWMHandle=&htim1;
   
    motor[i].PWM_CHANNELHanle=CHANNEL[i];
    motor[i].GPIO1=GPIOX[i][0];
	  motor[i].GPIO2=GPIOX[i][1];
	  motor[i].GPIO_Pin1=GPIOPIN[i][0];
	  motor[i].GPIO_Pin2=GPIOPIN[i][1];
	  motor[i].motor_overflow_counter.Capture_Count=0;//输入捕获计数值（此处为时间差值）
    PID_Param_init(&motor[i].motor_pid);
	
//11.24  初始化就需要设置引脚电平不同，否则“前馈”转不起来
//	  HAL_GPIO_WritePin(GPIOX[i][0], GPIOPIN[i][0],0);
//		HAL_GPIO_WritePin(GPIOX[i][1], GPIOPIN[i][1],1);
}

void motor_pid_control(motor_data *motor_,uint16_t i,uint16_t actualval)
{
    if(motor_->is_motor_enable==1)   //电机使能
    {
			  float current_val = 0;      //PID 计算值（初始化为0）
				motor_->motor_overflow_counter.Capture_Count=actualval;  //将所得时间差值difference传给Capture_Count
        motor_->actual_speed=motor_->motor_overflow_counter.Capture_Count*10;
        
//		  printf("actual_speed%d=%f\n",i,motor_->actual_speed);
        compileactualval[i]=motor_->actual_speed;
			  if(current_val<0) current_val=0;											//防止将负数传入pwm导致意外
			
//        if(motor_w[i]>0)
//            set_motor_direction(motor_,0);
//        else
//				{
//					motor_->motor_pid.target=-motor_->motor_pid.target;		//若解算出的速度为负数则转向并且让target等于相反数
//					set_motor_direction(motor_,1);                      //注：此处判断条件为motor[i]的正负  因为target的正负需要被改变
//        }																											//11.25 放主函数去了
				current_val= PID_realize(&(motor_->motor_pid),motor_->actual_speed);  //丢进pid里，吐出应输出的占空比
//			printf("dir%d,%d\n",i,motor_->MOTOR_DIE);
        set_motor_speed(motor_,i,current_val);                //计算出pid的输出值并传入set_speed函数控制占空比
    }
}

void motor_start(uint8_t i)
{
    motor[i].is_motor_enable=1;
    //输入捕获启动
    //HAL_TIM_IC_Start (EncoderHandle[i],EncoderChannel[i][0]);  //这一句加了会让程序开摆
	  HAL_TIM_IC_Start_IT (EncoderHandle[i],EncoderChannel[i][0]);
    __HAL_TIM_SET_COUNTER(EncoderHandle[i],0);
    //PWM启动
    HAL_TIM_PWM_Start(motor[i].TIM_PWMHandle,motor[i].PWM_CHANNELHanle);

}

void motor_stop(uint8_t i)
{
    motor[i].is_motor_enable=0;
    //输入捕获关闭
    HAL_TIM_IC_Stop(EncoderHandle[i],EncoderChannel[i][0]);
	  HAL_TIM_IC_Stop_IT(EncoderHandle[i],EncoderChannel[i][0]);
    //PWM关闭
    HAL_TIM_PWM_Stop(motor[i].TIM_PWMHandle,motor[i].PWM_CHANNELHanle);

}


