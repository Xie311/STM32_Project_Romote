#ifndef __CHASSIS_STATEMECHINE_H
#define __CHASSIS_STATEMECHINE_H

#include "chassis.h"

enum Grip_Seed_State {
    Grip_Ready,
    Grip,
    Grip_Deposit,
};

enum Plant_Seed_State {
    Plant_Deposit,
    Plant,
    Plant_Ready,
};

enum Chassis_State {
    Seed_Mode,
    Ball_Mode,
};

extern int right_land_angle;
extern int right_deposit_angle;
extern int left_land_angle;
extern int left_deposit_angle;
extern enum Chassis_State chassis_mode;

void Grip_Seed_Task_Start(void);
void Chassis_State_Mechine_Start(void);

void Chassis_State_mechine_Task(void *argument);
void Right_Grip_Seed_Task(void *argument);
void Right_Plant_Seed_Task(void *argument);
void Left_Grip_Seed_Task(void *argument);
void Left_Plant_Seed_Task(void *argument);

#endif