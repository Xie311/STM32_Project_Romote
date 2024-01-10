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
#include "BSP_PS2.h"
#include <stdio.h>
#include <stdlib.h>
#include "retarget.h"
#include "string.h"
//#include "servo.h"
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
float car_speed[8]={0};//定义包含x（前后）、y（左右）、z（角速度）三个方向，以及�?个控制上层状态的参数
uint8_t dataByte[18]={0};//存储四个数据包的数组
typedef union
{
    float data;
    uint8_t byte[4];
}float_to_byte;//用联合体将浮点数拆分为可收发的字�?
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start(&htim1);
    //HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);//�??????下方舵机
    //HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);//中间位置舵机
    //HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);//控制爪子舵机
    RetargetInit(&huart1);//初始化串口重定向
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);//主控板自带LED灯亮
    //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);//为CH340模块提供GND（用CH340连电脑测试时用）
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        delay_ms(20);//【重要�?�与PS2通信后延�??????20ms，防止两次�?�信间时间间隔过短，PS2接收器反应不过来而超时响�?????
        PS2_Read_Data();//实时获取PS2传回的原始数�??????

        //Servo_SetAngle(0,60);
        //Servo_SetAngle(1,60);
        //Servo_SetAngle(2,90);

        //PS2测试
        //HAL_UART_Transmit(&huart1,"A",sizeof("A"),0xff);
        //printf("[0]0x%2x,[1]0x%2x,[2]0x%2x,[3]0x%2x,[4]0x%2x,[5]0x%2x,[6]0x%2x,[7]0x%2x,[8]0x%2x\n",PS2_RawData[0],PS2_RawData[1],PS2_RawData[2],PS2_RawData[3],PS2_RawData[4],PS2_RawData[5],PS2_RawData[6],PS2_RawData[7],PS2_RawData[8]);
        //printf("%2x\n",PS2_RawData[0]);

        //控制小车运动方向
        //PS2使用模拟模式，主控接收PS2传回的摇杆�?�与按键值，对其处理后发送给从机

        if (PS2_Data.A_D == 1)
        {
            /************************************************************************/
            //由于在BSP_PS2.c文件中，PS2_Data.Rocker_LY和PS2_Data.Rocker_RX这两个变�??????
            //分别被decode成了-1 - (PS2_RawData[8] - 0x80)和PS2_RawData[5] - 0x80
            //导致这段代码算出的car_speed[0]和[1]的�?�会�??????-0.039063的误�??????
            //但是由于直接在这里把误差加回去会导致PS2异常断连（莫名其妙！），�??????以只能由接收方加以修正啦^_^
            car_speed[0] = 5.0 / 128.0 * PS2_Data.Rocker_LY;//左摇杆取y方向值，设定麦轮x方向速度
            car_speed[1] = 5.0 / 128.0 * PS2_Data.Rocker_RX;//右摇杆取x方向值，设定麦轮y方向速度
            /************************************************************************/
            if (PS2_RawData[4] == 0xfb)//若按下L1，车逆时针转（反转）
            {
                car_speed[2] = -10.0;
            }
            else if (PS2_RawData[4] == 0xf7)//若按下R1，车顺时针转（正转）
            {
                car_speed[2] = 10.0;
            }
            else
            {
                car_speed[2] = 0.0;
            }
            /************************************************************************
            //switch-case 也寄啦~
            //稳定断联
            switch(PS2_RawData[3])
            {  case 0x7f:
                    car_speed[3] = 1.0;
                    break;
                    case 0xbf:
                car_speed[4] = 1.0;}

            switch(PS2_RawData[4])
            {  case 0xdf:
                    car_speed[5] = 1.0;
                    break;
                case 0xbf:
                    car_speed[6] = 1.0;}
            ************************************************************************/

            /************************************************************************
            //当按键未被按下时，对应位置零（PS2稳定断联 寄寄寄寄寄寄寄寄寄寄�??? �???20ms？）???
             if (PS2_RawData[3] == 0x7f)//LEFT  抓球->储球->抓球
            {
                car_speed[3] = 1.0;
            }
            else car_speed[3]=0.0;

            if (PS2_RawData[3] == 0xbf)//DOWN  复位
            {
                car_speed[4] = 1.0;
            }
            else car_speed[4] = 0.0;

            if (PS2_RawData[4] == 0xdf)//O  投球
            {
                car_speed[5] = 1.0;
            }
            else car_speed[5] = 0.0;

            if (PS2_RawData[4] == 0xbf)//X
            {
                car_speed[6] = 1.0;
            }
            else car_speed[6] = 0.0;
            ************************************************************************/

            /************************************************************************/
           if (PS2_RawData[3] == 0xbf)//DOWN  抓球
           {
               car_speed[3] = 1.0;
            }
           else if (PS2_RawData[3] == 0x7f)//LEFT 储球->抓球
           {
               car_speed[3] = 2.0;
           }
            else if (PS2_RawData[3] == 0xef)//UP 复位
            {
                car_speed[3] = 3.0;
            }
            else if (PS2_RawData[4] == 0xbf)//x  投球
            {
                car_speed[3] = 4.0;
            }
            else if (PS2_RawData[4] == 0xdf)//o  取球->再次投球
            {
                car_speed[3] = 5.0;
            }
            else car_speed[3]=0.0;
            /************************************************************************/
            //a1b11a7

           }

            /**********************************板间通信**********************************/
            float_to_byte data_1;

            //帧头
            dataByte[0] = 0xff;

            //将三个浮点数据分别拆分为4个字节，存入dataByte数组�??????1~12�??????
            data_1.data = car_speed[0];
            dataByte[1] = data_1.byte[0];
            dataByte[2] = data_1.byte[1];
            dataByte[3] = data_1.byte[2];
            dataByte[4] = data_1.byte[3];

            data_1.data = car_speed[1];
            dataByte[5] = data_1.byte[0];
            dataByte[6] = data_1.byte[1];
            dataByte[7] = data_1.byte[2];
            dataByte[8] = data_1.byte[3];
            data_1.data = car_speed[2];
            dataByte[9] = data_1.byte[0];
            dataByte[10] = data_1.byte[1];
            dataByte[11] = data_1.byte[2];
            dataByte[12] = data_1.byte[3];

            data_1.data = car_speed[3];
            dataByte[13] = data_1.byte[0];
            dataByte[14] = data_1.byte[1];
            dataByte[15] = data_1.byte[2];
            dataByte[16] = data_1.byte[3];

            //帧尾
            dataByte[17] = 0xfe;


/*
            data_1.data = car_speed[4];
            dataByte[17] = data_1.byte[0];
            dataByte[18] = data_1.byte[1];
            dataByte[19] = data_1.byte[2];
            dataByte[20] = data_1.byte[3];

            data_1.data = car_speed[5];
            dataByte[21] = data_1.byte[0];
            dataByte[22] = data_1.byte[1];
            dataByte[23] = data_1.byte[2];
            dataByte[24] = data_1.byte[3];

            data_1.data = car_speed[6];
            dataByte[25] = data_1.byte[0];
            dataByte[26] = data_1.byte[1];
            dataByte[27] = data_1.byte[2];
            dataByte[28] = data_1.byte[3];

            //帧尾
            dataByte[29] = 0xfe;*/
/*
      data_1.data = car_speed[7];
      dataByte[29]=data_1.byte[0];
      dataByte[30]=data_1.byte[1];
      dataByte[31]=data_1.byte[2];
      dataByte[32]=data_1.byte[3];

       //dataByte[33]=0xfe;
*/
            //仅调用�?�一次�?�串口轮询发送函数，发�?�dataByte数组（不要使用串口DMA发�?�，比轮询卡很多�??????
            HAL_UART_Transmit(&huart1, dataByte, sizeof(dataByte), 0xff);
            /*************************************************************************/
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
