/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_pid.h"
#include "motor_control.h"
#include "bsp_Mecanum.h"
#include "io_retargetToUart.h"
#include "stdio.h"
#include "servo.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


uint16_t cnt[4]={0,0,0,0};  //to mark the capture value
uint8_t Rx_Buffer[18]={0}; 
uint16_t flag[6]={0};
float servo_mode=0;
typedef union
{
    float data;
    uint8_t byte[4];
}byte_to_float;

 byte_to_float data_2;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	motor_init(0);
	motor_init(1);
	motor_init(2);
	motor_init(3);
  motor_start(0);
	motor_start(1);
	motor_start(2);
	motor_start(3);
	
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
	HAL_UART_Receive_IT(&huart1,Rx_Buffer,18);
	
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
	
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	
	__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,50);
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,50);
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_4,50);
	
	//flag[5]=1;
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
   
		if(flag[5]==1)
		{
		     //HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);

					data_2.byte[0]=Rx_Buffer[1];    
					data_2.byte[1]=Rx_Buffer[2];
					data_2.byte[2]=Rx_Buffer[3];
					data_2.byte[3]=Rx_Buffer[4];
			   if(fabs(data_2.data)<0.05)  data_2.data=0;
					car_speed[0]=data_2.data*300;  //x  

					data_2.byte[0]=Rx_Buffer[5];
					data_2.byte[1]=Rx_Buffer[6];
					data_2.byte[2]=Rx_Buffer[7];
					data_2.byte[3]=Rx_Buffer[8];
			   if(fabs(data_2.data)<0.05)  data_2.data=0;
					car_speed[1]=-data_2.data*00;  //y 
									
					data_2.byte[0]=Rx_Buffer[9];
					data_2.byte[1]=Rx_Buffer[10];
					data_2.byte[2]=Rx_Buffer[11];
					data_2.byte[3]=Rx_Buffer[12];
					car_speed[2]=-data_2.data/2;  //w   
			
			    data_2.byte[0]=Rx_Buffer[13];
					data_2.byte[1]=Rx_Buffer[14];
					data_2.byte[2]=Rx_Buffer[15];
					data_2.byte[3]=Rx_Buffer[16];
					servo_mode=data_2.data;
					
					flag[5]=0;
					
					//printf("%2f,%2f,%2f,%2f\n",car_speed [0],car_speed [1],car_speed [2],servo_mode);
					
					cal_motor_speed(cal_matrix,motor_w,car_speed,MOTOR_R);


					for(uint16_t i=0;i<4;i++)
					{
							if(motor_w[i]>0)
									set_motor_direction(&motor[i],0);
							else
							{
									motor_w[i]=-motor_w[i];		
									set_motor_direction(&motor[i],1);                      
							}
					}
					set_pid_target(&motor[0].motor_pid,motor_w[0]);   //set the speed as target; should be placed in "while" when ps2 is added
					set_pid_target(&motor[1].motor_pid,motor_w[1]);
					set_pid_target(&motor[2].motor_pid,motor_w[2]);
					set_pid_target(&motor[3].motor_pid,motor_w[3]);
		
		
		       /***********************SERVO CONTROL*********************************************/
		                if(servo_mode==1)//
                    {
                        //printf("A\n");
                        Servo_SetAngle(0,56.0);//adjust
                        Servo_SetAngle(1,180);
                        Servo_SetAngle(2,180); //open
                        HAL_Delay(1500);
                        Servo_SetAngle(2,0);//GRAB
                    }
										if(servo_mode==2)//??
                    {
                        //printf("B\n");
                        Servo_SetAngle(0,88);
                        Servo_SetAngle(1,0);
                        HAL_Delay(900);
                        Servo_SetAngle(2,180);//STORE

                        HAL_Delay(900);
                        Servo_SetAngle(0,56);//^_^
                        Servo_SetAngle(1,180);
                        HAL_Delay(1000);
                        Servo_SetAngle(2,10);//GRAB
                    }
                    if(servo_mode==3)//??
                    {
                        //printf("C\n");
                        Servo_SetAngle(0,88);
                        Servo_SetAngle(1,90);
                    }
                    if(servo_mode==4)//??
                    {
                       // printf("D\n");
                        Servo_SetAngle(1,110);
                        Servo_SetAngle(2,180);//shoot
                    }
										if(servo_mode==5)//??
                    {
                       // printf("E\n");
                        Servo_SetAngle(0,88);
                        Servo_SetAngle(1,5);
                        HAL_Delay(1000);
                        Servo_SetAngle(2,0);//grab

                        HAL_Delay(1500);
                        Servo_SetAngle(1,110);
                        HAL_Delay(800);
                        Servo_SetAngle(2,180);//shoot
                    }
										
										servo_mode=0;
						 /***********************SERVO CONTROL*********************************************/
			}
	   if(flag[0])
	   {
	    	__HAL_TIM_SET_CAPTUREPOLARITY(EncoderHandle[0], EncoderChannel[0][0], TIM_INPUTCHANNELPOLARITY_RISING); //?is it necessary?
		   cnt[0]++;
		   HAL_TIM_IC_Start_IT(EncoderHandle[0],EncoderChannel[0][0]);
			flag[0]=0;
	   }
		  if(flag[1])
	   {
	    	__HAL_TIM_SET_CAPTUREPOLARITY(EncoderHandle[1], EncoderChannel[1][0], TIM_INPUTCHANNELPOLARITY_RISING); //?is it necessary?
		   cnt[1]++;
		   HAL_TIM_IC_Start_IT(EncoderHandle[1],EncoderChannel[1][0]);
			flag[1]=0;
	   }
			if(flag[2])
	   {
	    	__HAL_TIM_SET_CAPTUREPOLARITY(EncoderHandle[2], EncoderChannel[2][0], TIM_INPUTCHANNELPOLARITY_RISING); //?is it necessary?
		   cnt[2]++;
		   HAL_TIM_IC_Start_IT(EncoderHandle[2],EncoderChannel[2][0]);
			flag[2]=0;
	   }
		 if(flag[3])
	   {
	    	__HAL_TIM_SET_CAPTUREPOLARITY(EncoderHandle[3], EncoderChannel[3][0], TIM_INPUTCHANNELPOLARITY_RISING); //?is it necessary?
		   cnt[3]++;
		   HAL_TIM_IC_Start_IT(EncoderHandle[3],EncoderChannel[3][0]);
			flag[3]=0;
	   }
    if(flag[4])
		{
			motor_pid_control(&motor[0],0,cnt[0]);
			
			cnt[0]=0;
			motor_pid_control(&motor[1],1,cnt[1]);
			
			cnt[1]=0;
			motor_pid_control(&motor[2],2,cnt[2]);
			
			cnt[2]=0;			
			motor_pid_control(&motor[3],3,cnt[3]);
		
			cnt[3]=0;
			flag[4]=0;
			
			//printf("%f,%f,%f,%f,%f,%f,%f,%f\n",compileactualval[0],compileactualval[1],compileactualval[2],compileactualval[3],
			//																	compileaduty[0],compileaduty[1],compileaduty[2],compileaduty[3]);
		}
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  
{
	if(huart->Instance==USART1)
	{
		if(Rx_Buffer[0]==0xff&&Rx_Buffer[17]==0xfe)
    {
			flag[5]=1;
			HAL_UART_Receive_IT(&huart1,Rx_Buffer,18);
		}
	}
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)  //inputcapture:collect the data of the difference of time
{
	if(htim->Instance==TIM2&&htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
	{
		flag[0]=1;
	}
	if(htim->Instance==TIM2&&htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
	{
		flag[1]=1;
	}
	if(htim->Instance==TIM3&&htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
	{
		flag[2]=1;
	}
	if(htim->Instance==TIM3&&htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
	{
		flag[3]=1;
	}


}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
		if(htim->Instance==TIM4) //10ms
		{
			flag[4]=1;
			HAL_TIM_Base_Start_IT(&htim4);
		}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
