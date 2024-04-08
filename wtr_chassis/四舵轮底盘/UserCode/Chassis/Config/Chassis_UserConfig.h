/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 12:53:53
 * @LastEditors: 15867773077 
 * @LastEditTime: 2024-01-08 20:34:19
 * @FilePath: \MyChassisTry - 副本\UserCode\Chassis\Config\Chassis_UserConfig.h
 * @Brief: 自定义宏
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#ifndef __CHASSIS_USERCONFIG_H__
#define __CHASSIS_USERCONFIG_H__

/**
 * @brief 自定义端口 *
 */

#define huart_RemoteCtl huart1
#define hcan_Dji        hcan1
#define hcan_VESC       hcan1

/**
 * 四舵轮底盘参数
 */
#define wheel2wheel_x    430      // 底盘垂直于前进方向的舵轮宽度 mm
#define wheel2wheel_y    430      // 底盘平行于前进方向的舵轮宽度 mm
#define wheel_radius     20       // 轮子半径 mm
#define wheel_motor_pole 14       // 电机极对数
#define wheel_rpm_ratio  2387.324 // 换算线速度到rpm 已放弃
#define Offset_x2center  0        // x方向中心偏移量
#define Offset_y2center  0        // y方向中心偏移量
#define pos_ratio        -3.3     // 舵方向减速比
#define vel_ratio        1.0      // 驱动电机减速比

/**
 * 舵轮旋转位置规划
 */
#define WheelCorrect_MaxVelocity     500 // 舵方向最大速度
#define WheelCorrecting_Acceleration 100 // 舵方向最大加速度

#endif // __CHASSIS_USERCONFIG_H__