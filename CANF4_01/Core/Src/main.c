/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "can.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DJI.h"
#include "wtr_can.h"
#include "wtr_uart.h"
#include "string.h"
#include "Caculate.h"
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
  MX_CAN1_Init();
  MX_USART1_UART_Init();
  MX_CAN2_Init();
  /* USER CODE BEGIN 2 */
  can1.CAN_Start();
  can2.CAN_Start();
  can1.CAN_Rx_Filter_Init();
  can2.CAN_Rx_Filter_Init();
  can1.CAN_Interrupt_Init();
  can2.CAN_Interrupt_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    int j=0;
    for(j=0;j<8;j++)
    {
      speedServo(500,&hDJI[j][0]);
    }
    
    CanTransmit_DJI_1234(can1, hDJI[0][0].speedPID.output, hDJI[1][0].speedPID.output, hDJI[2][0].speedPID.output, hDJI[3][0].speedPID.output);
    CanTransmit_DJI_5678(can1, hDJI[4][0].speedPID.output, hDJI[5][0].speedPID.output, hDJI[6][0].speedPID.output, hDJI[7][0].speedPID.output);
    CanTransmit_DJI_1234(can2, hDJI[0][1].speedPID.output, hDJI[1][1].speedPID.output, hDJI[2][1].speedPID.output, hDJI[3][1].speedPID.output);
    CanTransmit_DJI_5678(can2, hDJI[4][1].speedPID.output, hDJI[5][1].speedPID.output, hDJI[6][1].speedPID.output, hDJI[7][1].speedPID.output);
    HAL_Delay(2);
  /* USER CODE END 3 */
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance == hcan1.Instance) {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hcan1_rx, can1_rxdata) != HAL_OK) {
            Error_Handler();
        }
        memcpy((uint8_t *)can1.rx_MSG.buffer, can1_rxdata, 8); // 保存CAN报文的内�?         �?&hcan1_rx接收到的信息都传给CAN_HANDLER结构�?
        can1.rx_MSG.id  = hcan1_rx.StdId;                      // 保存CAN报文的ID
        can1.rx_MSG.len = hcan1_rx.DLC;                        // 保存CAN报文的内容长�?
        /* Can message Decode */
        if (hcan1_rx.IDE == CAN_ID_STD) {
            DJI_CanMsgDecode(1, can1.rx_MSG.id, (uint8_t *)can1.rx_MSG.buffer);  //把can1.rx_MSG.buffer的内容每五十次更新到电机结构体中；Decode--->update
        }
        if (hcan1_rx.IDE == CAN_ID_EXT) {
            // vesc反馈关掉这里就不会有消息
            ;
            ;
        }
    }
#if (USE_CAN2 == 1)
    else if (hcan->Instance == hcan2.Instance) {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hcan2_rx, can2_rxdata) != HAL_OK) {
            Error_Handler();
        }
        memcpy((uint8_t *)can2.rx_MSG.buffer, can2_rxdata, 8); // 保存CAN报文的内�?
        can2.rx_MSG.id  = hcan2_rx.StdId;                      // 保存CAN报文的ID
        can2.rx_MSG.len = hcan2_rx.DLC;                        // 保存CAN报文的内容长�?
        /* Can message Decode */
        if (hcan2_rx.IDE == CAN_ID_STD) {
            DJI_CanMsgDecode(2, can2.rx_MSG.id, (uint8_t *)can2.rx_MSG.buffer);

        }
        if (hcan2_rx.IDE == CAN_ID_EXT) {
            // vesc反馈关掉这里就不会有消息
            ;
            ;
        }
    }
#endif
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
