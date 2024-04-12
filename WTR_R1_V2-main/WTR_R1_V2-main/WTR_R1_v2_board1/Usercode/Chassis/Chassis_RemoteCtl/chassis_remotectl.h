#ifndef __CHASSIS_REMOTECTL_H
#define __CHASSIS_REMOTECTL_H

#include "chassis_start.h"

void Chassis_Joytick_Init(void);
void Chassis_Remotectl_TaskStart(void);
void Chassis_RemoteCtl_Task(void *argument);

#endif