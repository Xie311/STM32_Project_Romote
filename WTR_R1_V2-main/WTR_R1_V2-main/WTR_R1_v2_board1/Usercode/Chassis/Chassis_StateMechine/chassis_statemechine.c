#include "chassis_statemechine.h"

/***********************************************************/
osThreadId_t State_mechine_TaskHandle;
const osThreadAttr_t State_mechine_Task_attributes = {
    .name       = "State_mechine_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

osThreadId_t Fire_Ball_TaskHandle;
const osThreadAttr_t Fire_Ball_Task_attributes = {
    .name       = "Fire_Ball_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

enum Chassis_State chassis_mode; // 底盘总控制线程
enum Fire_State fire_state;

int arm_angle  = 0;
int rail_angle = 0;
int friction_speed = 0;

/***********************************************************/

/**
 * @brief   射球状态机线程开启
 */
void Fire_Ball_Task_Start(void)
{
    fire_state           = Fire_Ready;
    Fire_Ball_TaskHandle = osThreadNew(Fire_Ball_Task, NULL, &Fire_Ball_Task_attributes);
}

/**
 * @brief   总状态机线程开启
 */
void Chassis_State_Mechine_Start(void)
{
    chassis_mode = Seed_Mode;
    Fire_Ball_Task_Start();
    State_mechine_TaskHandle = osThreadNew(Chassis_State_mechine_Task, NULL, &State_mechine_Task_attributes);
}

/**
 * @brief   总控制状态机
 */
void Chassis_State_mechine_Task(void *argument)
{
    for (;;) {
        if (chassis_mode == Seed_Mode) {
            while(fire_state != Fire_Ready)
            {
                osDelay(1);
            }
            friction_speed = 0;
            osThreadSuspend(Fire_Ball_TaskHandle); // 挂起射球线程
        } else if (chassis_mode == Ball_Mode) {
            osThreadResume(Fire_Ball_TaskHandle); // 解挂射球线程
            friction_speed = 5000;
        }
        osDelay(2);
    }
}

/**
 * @brief   射球状态机
 */
void Fire_Ball_Task(void *argument)
{
    for (;;) {
        switch (fire_state) {
            case Fire_Ready: // 1. 发球准备状态
                arm_angle  = 5;
                rail_angle = 0;
                Servo_Reset();
                Servo_Out();
                osDelay(500);
                switch (as69_data.left) {
                    case 3:
                        fire_state = Fire_Ready;
                        break;
                    case 2:
                        fire_state = Fire_Ready;
                        break;
                    case 1:
                        fire_state = Fire_Pick;
                        break;
                    default:
                        break;
                }
                break;
            case Fire_Pick: // 2. 取球状态
                Servo_Grip();
                osDelay(1000);
                Servo_In();
                arm_angle = 180;
                while (hDJI[4].AxisData.AxisAngle_inDegree < 178.0f) {
                    osDelay(1);
                }
                fire_state = Fire_Shoot;
                break;
            case Fire_Shoot: // 3. 发射状态
                Servo_Reset();
                osDelay(2000);
                rail_angle = -630;
                arm_angle  = 5;
                Servo_Out();
                while (hDJI[5].AxisData.AxisAngle_inDegree > -628.0f) {
                    osDelay(1);
                }
                rail_angle = -2;
                while (hDJI[5].AxisData.AxisAngle_inDegree < -3.0f || hDJI[4].AxisData.AxisAngle_inDegree > 7.0f) {
                    osDelay(1);
                }
                fire_state = Fire_Ready;
                break;
            default:
                break;
        }
        osDelay(1);
    }
}