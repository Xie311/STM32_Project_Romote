

#include "STP_Decode.h"

void STP_23L_Decode()           
{
    if((Rxbuffer[0]==Rxbuffer[1])&&(Rxbuffer[1]==Rxbuffer[2])&&(Rxbuffer[2]==Rxbuffer[3])&&(Rxbuffer[3]==0xAA))     //检测帧头
    {
        if (Rxbuffer[5] == PACK_GET_DISTANCE)                                                                       //检测命令码
         {   uint32_t CS_sum = 0;
            for (uint16_t i = 4; i < 194; i++) CS_sum += Rxbuffer[i];
            if (Rxbuffer[194] == CS_sum%256)                                                                        //检测校验码
            {
           
                for (uint16_t i = 0; i < 12; i++)
                {
                    // OLED_ShowNum(90, 3, Rxbuffer[5], 2, 8);
                    LidarData[i].distance   = Rxbuffer[10 + 15 * i] + (Rxbuffer[11 + 15 * i] * 256); // 只有distance是有用的数据 注意前面是低位后面是高位
                    LidarData[i].noise      = (Rxbuffer[13 + 15 * i] << 8) + Rxbuffer[12 + 15 * i];
                    LidarData[i].peak       = (Rxbuffer[17 + 15 * i] << 24) + (Rxbuffer[15 + 15 * i] << 16) + (Rxbuffer[16 + 15 * i] << 8) + Rxbuffer[15 + 15 * i];
                    LidarData[i].confidence = Rxbuffer[18 + 15 * i];
                    LidarData[i].intg       = (Rxbuffer[22 + 15 * i] << 24) + (Rxbuffer[21 + 15 * i] << 16) + (Rxbuffer[20 + 15 * i] << 8) + Rxbuffer[19 + 15 * i];
                    LidarData[i].reftof     = (Rxbuffer[24 + 15 * i] << 8) + Rxbuffer[23 + 15 * i];
                }
            }
         }
    }
}