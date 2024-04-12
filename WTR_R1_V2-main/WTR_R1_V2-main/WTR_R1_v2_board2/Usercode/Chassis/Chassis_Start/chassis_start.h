#ifndef __CHASSIS_START_H
#define __CHASSIS_START_H

/*********************基本驱动库**********************/
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "usart.h"
#include "math.h"
#include <stdio.h>
/*********************硬件驱动库**********************/
#include "Caculate.h"
#include "DJI.h"
#include "Unitree_user.h"
#include "wtr_can.h"
#include "wtr_servo.h"
/*********************底盘控制库**********************/
#include "chassis.h"
#include "chassis_debug.h"
#include "chassis_message.h"
#include "chassis_start.h"
/*******************状态机和应用***********************/
#include "chassis_statemechine.h"

#endif 