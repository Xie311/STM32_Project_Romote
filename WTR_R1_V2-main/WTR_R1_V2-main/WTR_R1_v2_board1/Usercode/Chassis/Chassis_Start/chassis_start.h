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
#include "wtr_as69.h"
#include "wtr_can.h"
#include "wtr_ops.h"
#include "wtr_servo.h"
/*********************底盘控制库**********************/
#include "chassis_start.h"
#include "chassis_config.h"
#include "chassis_remotectl.h"
#include "chassis_perception.h"
#include "chassis_statemechine.h"
#include "chassis_callback.h"
#include "chassis_message.h"
#include "chassis_debug.h"
#include "chassis_pid.h"
#include "wtr_chassis.h"
/*******************状态机和应用***********************/
#include "wtr_fire_ball.h"

#endif