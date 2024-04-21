#include "wtr_ops.h"
#include <stdio.h>

OPS_t OPS_Data = {0};   // OPS_Data 结构体，用于存储解码后的数据，初始值为 0
uint8_t ops_buffer[28]; // 接收缓冲区，用于存储接收到的数据，大小为 28 字节

/**
 * @brief 码盘解码函数
 *
 * @return uint8_t
 */
uint8_t OPS_Decode(void)
{
    float offset_x = 0;  // 码盘坐标系零点距底盘坐标系零点x方向偏移量
    float offset_y = 0;  // 码盘坐标系零点距底盘坐标系零点y方向偏移量

    static union {
        uint8_t data[24]; // 用于存储解码后的姿态数据的数据缓冲区
        float ActVal[6]; // 用于存储解码后的值的数组
    } posture;

    // 检查起始和结束字符以确保数据完整性
    if (ops_buffer[0] != 0x0D || ops_buffer[1] != 0x0A || ops_buffer[26] != 0x0A || ops_buffer[27] != 0x0D) {
        // 如果未找到起始和结束字符，则清空缓冲区并返回错误
        for (int i = 0; i < 28; i++) {
            ops_buffer[i] = 0;
        }
        return 1; // 返回错误代码
    }

    // 从缓冲区提取数据并分配到姿态数据缓冲区
    for (int i = 0; i < 24; i++) {
        posture.data[i] = ops_buffer[i + 2];
    }

    // 将解码后的值分配给 OPS_Data 结构体的字段
    OPS_Data.z_angle = posture.ActVal[0];
    OPS_Data.x_angle = posture.ActVal[1];
    OPS_Data.y_angle = posture.ActVal[2];
    OPS_Data.pos_x   = posture.ActVal[3] + offset_x;
    OPS_Data.pos_y   = posture.ActVal[4] + offset_y;
    OPS_Data.w_z     = posture.ActVal[5];

    return 0; // 返回成功代码
}

