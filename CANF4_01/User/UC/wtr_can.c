/*过滤器配置，can的fifo0接收*/

#include "wtr_can.h"

uint8_t CanReceiveData[8];

/**
 * @brief CAN过滤器配置函数
 * ps.此处的很多配置都是因为使用的电机需要
*/
HAL_StatusTypeDef CANFilterInit(CAN_HandleTypeDef *hcan)    //“邮箱配置初始化”
{
  CAN_FilterTypeDef sFilterConfig;                         //CAN_FilterTypeDef是HAL定义的结构体类型

  sFilterConfig.FilterBank = 0;                            //选择验收筛选器（此处为第一个）
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;        //筛选模式使用掩码模式
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;       //筛选器尺度
  sFilterConfig.FilterIdHigh = 0x0000;                     //CAN_FxR1的高16位
  sFilterConfig.FilterIdLow = 0x0000;                      //CAN_FxR1的低16位
  sFilterConfig.FilterMaskIdHigh = 0x0000;                 //CAN_FxR2的高16位，这里掩码全部设置为0是因为不需要筛电机的ID
  sFilterConfig.FilterMaskIdLow = 0x0000;                  //CAN_FxR2的高16位
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;       //设置经过筛选后的数据放入哪个fifo，可0可1
  sFilterConfig.FilterActivation = ENABLE;                 //是否使能本筛选器
  sFilterConfig.SlaveStartFilterBank = 14;                 //从筛选器，用不到，随便填个大于等于14小于28的

  if (HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_Start(hcan) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }

  return HAL_OK;
}

/**
 * @brief CAN消息解码函数
*/
void CanDataDecode(CAN_RxHeaderTypeDef RxHeader)
{
  /* Can message Decode */
  if (RxHeader.IDE == CAN_ID_STD)  //STD表示标准，也就是标准模式（不扩展）
  {
    DJI_CanMsgDecode(RxHeader.StdId, CanReceiveData);
  }
  if (RxHeader.IDE == CAN_ID_EXT)
  {
    // vesc反馈关掉这里就不会有消息
    ;
    ;
  }
}

/**
 * @brief CAN RX0接收中断回调函数
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef RxHeader;     //一个结构体
  if (hcan->Instance == hcan1.Instance)
  {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, CanReceiveData) != HAL_OK)
    {
      //GetMessage不成功就进入硬件错误处理
      Error_Handler();  //一种比较暴力的处理方式，程序一旦跑飞只能重启
    }
    CanDataDecode(RxHeader);//接受成功则开始解码
  }
}
