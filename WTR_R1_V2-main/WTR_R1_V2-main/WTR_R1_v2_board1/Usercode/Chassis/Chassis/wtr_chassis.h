/**
 * @brief   底盘控制
 */
#ifndef __WTR_CHASSIS_H
#define __WTR_CHASSIS_H

#include "chassis_start.h"

/**************************************************************/

extern int32_t v_1, v_2, v_3, v_4;

/**************************************************************/
void Inverse_kinematic_equation(float vx, float vy, float wc, int32_t *_v_1, int32_t *_v_2, int32_t *_v_3, int32_t *_v_4);

void Chassis_CAN_Message_TaskStart(void);
void Chassis_Init(void);

void CAN_Message_Task(void *argument);

#endif