#ifndef __CHASSIS_MESSAGE_H
#define __CHASSIS_MESSAGE_H

#include "chassis_start.h"

extern uint8_t chassis_message_buffer[8];

void Chassis_Message_TaskStart(void);
void Chassis_Message_Task(void *argument);

#endif