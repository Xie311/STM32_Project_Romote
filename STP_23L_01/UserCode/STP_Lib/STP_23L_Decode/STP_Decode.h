#ifndef __STP_DECODE_H__
#define __STP_DECODE_H__

#include "STP_Start.h"
#include "stm32f1xx_hal.h"
#include "STP_Receive.h"

/*STP_23L数据包命令码宏定义*/
#define PACK_GET_DISTANCE       0x02
#define PACK_RESET_SYSTEM       0x0D
#define PACK_STOP               0x0F
#define PACK_ACK                0x10
#define PACK_VERSIOM            0x14
void STP_23L_Decode();

#endif // __STP_DECODE_H__