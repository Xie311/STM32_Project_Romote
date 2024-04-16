
#ifndef STP_RECEIVE
#define STP_RECEIVE

#include "STP_Start.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
extern uint16_t DataLens;
extern uint8_t Rxbuffer[195];
extern uint16_t flag;

typedef struct{
    int16_t distance;               //测量目标距离（单位：mm）
    uint16_t noise;                 //环境噪声
    uint32_t peak;                  //接收强度信息
    uint8_t confidence;             //置信度    
    uint32_t intg;                  //积分次数
    int16_t reftof;                 //温度表征值
} LidarPointTypedef;                //获取的测量数据结构体

/*定义接收到的雷达测试数据*/
extern LidarPointTypedef LidarData[12];

/*串口宏定义*/
#define USART_RX_HANDLE &huart1



#endif // !1