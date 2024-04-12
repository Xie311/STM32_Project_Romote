#include "chassis_statemechine.h"

/*********************************************************************/
osThreadId_t State_mechine_TaskHandle; // 底盘总控制线程
const osThreadAttr_t State_mechine_Task_attributes = {
    .name       = "State_mechine_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

osThreadId_t Right_Grip_Seed_TaskHandle; // 右侧取苗控制线程
const osThreadAttr_t Right_Grip_Seed_Task_attributes = {
    .name       = "Right_Grip_Seed_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

osThreadId_t Right_Plant_Seed_TaskHandle; // 右侧放苗控制线程
const osThreadAttr_t Right_Plant_Seed_Task_attributes = {
    .name       = "Right_Plant_Seed_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

osThreadId_t Left_Grip_Seed_TaskHandle; // 左侧取苗控制线程
const osThreadAttr_t Left_Grip_Seed_Task_attributes = {
    .name       = "Left_Grip_Seed_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

osThreadId_t Left_Plant_Seed_TaskHandle; // 左侧放苗控制线程
const osThreadAttr_t Left_Plant_Seed_Task_attributes = {
    .name       = "Left_Plant_Seed_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

osMutexId_t Unitree_usart_mutex;
const osMutexAttr_t Unitree_usart_mutex_attributes = {
    .name    = "Unitree_USART_Mutex",
    .cb_size = 0U,
    .cb_mem  = NULL,
};

enum Chassis_State chassis_mode;              // 底盘总控制线程
enum Grip_Seed_State right_grip_seed_state;   // 右侧取苗状态
enum Plant_Seed_State right_plant_seed_state; // 右侧放苗状态
enum Grip_Seed_State left_grip_seed_state;    // 左侧取苗状态
enum Plant_Seed_State left_plant_seed_state;  // 左侧放苗状态

osThreadState_t seed_task_state; // 线程挂起状态定义

const int right_deposit_point[5] = {
    0,
    370,
    640,
    910,
    1120,
}; // 右侧挡板标记点
const int left_deposit_point[5] = {
    0,
    -370,
    -640,
    -910,
    -1120,
}; // 左侧挡板标记点

static int right_seed_count; // 右侧存苗数量
int right_land_angle;        // 右侧升降电机
int right_deposit_angle;     // 右侧挡板电机
static int left_seed_count;  // 左侧存苗数量
int left_land_angle;         // 左侧升降电机
int left_deposit_angle;      // 左侧挡板电机

/*********************************************************************/

/**
 * @brief   取苗状态机线程开启
 */
void Grip_Seed_Task_Start(void)
{
    right_seed_count            = 0;
    right_grip_seed_state       = Grip_Ready;
    right_plant_seed_state      = Plant_Ready;
    left_seed_count             = 0;
    left_grip_seed_state        = Grip_Ready;
    left_plant_seed_state       = Plant_Ready;
    Right_Plant_Seed_TaskHandle = osThreadNew(Right_Plant_Seed_Task, NULL, &Right_Plant_Seed_Task_attributes);
    Right_Grip_Seed_TaskHandle  = osThreadNew(Right_Grip_Seed_Task, NULL, &Right_Grip_Seed_Task_attributes);
    Left_Plant_Seed_TaskHandle  = osThreadNew(Left_Plant_Seed_Task, NULL, &Left_Plant_Seed_Task_attributes);
    Left_Grip_Seed_TaskHandle   = osThreadNew(Left_Grip_Seed_Task, NULL, &Left_Grip_Seed_Task_attributes);
}

/**
 * @brief   总状态机线程开启
 */
void Chassis_State_Mechine_Start(void)
{
    chassis_mode = Seed_Mode;
    // 创建串口互斥锁
    Unitree_usart_mutex = osMutexNew(&Unitree_usart_mutex_attributes);
    if (Unitree_usart_mutex == NULL) {
        for (;;) {
            HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_1);
            osDelay(1000);
        }
    }
    Grip_Seed_Task_Start();
    State_mechine_TaskHandle = osThreadNew(Chassis_State_mechine_Task, NULL, &State_mechine_Task_attributes);
}

/**
 * @brief   总控制状态机
 */
void Chassis_State_mechine_Task(void *argument)
{
    for (;;) {
        seed_task_state = osThreadGetState(Right_Grip_Seed_TaskHandle);
        if (chassis_mode == Seed_Mode) {
            if (seed_task_state == osThreadBlocked) {
                osThreadResume(Right_Grip_Seed_TaskHandle);  // 解挂取苗线程
                osThreadResume(Right_Plant_Seed_TaskHandle); // 解挂放苗线程
                osThreadResume(Left_Grip_Seed_TaskHandle);   // 解挂取苗线程
                osThreadResume(Left_Plant_Seed_TaskHandle);  // 解挂放苗线程
            }
        } else if (chassis_mode == Ball_Mode) {
            while (right_grip_seed_state != Grip_Ready || right_plant_seed_state != Plant_Ready) {
                osDelay(1);
            }
            Unitree_UART_tranANDrev(unitree_motor_right, 0, 1, 0, 0, unitree_offset_right - _PI / 2 - 0.1, 0.09, 0.05);
            osDelay(100);
            osThreadSuspend(Right_Grip_Seed_TaskHandle);  // 挂起取苗线程
            osThreadSuspend(Right_Plant_Seed_TaskHandle); // 挂起放苗线程
            osThreadSuspend(Left_Grip_Seed_TaskHandle);   // 挂起取苗线程
            osThreadSuspend(Left_Plant_Seed_TaskHandle);  // 挂起放苗线程
        }
        osDelay(2);
    }
}

/**
 * @brief   取苗状态机
 */
void Right_Grip_Seed_Task(void *argument)
{
    static osStatus_t unitree_usart_state;
    for (;;) {
        switch (right_grip_seed_state) {
            case Grip_Ready: // 1. 取苗准备
                unitree_usart_state = osMutexAcquire(Unitree_usart_mutex, osWaitForever);
                if (unitree_usart_state == osOK) {
                    Unitree_UART_tranANDrev(unitree_motor_right, 0, 1, 0, 0, unitree_offset_right - _PI - 0.1, 0.09, 0.05);
                }
                osMutexRelease(Unitree_usart_mutex);
                right_land_angle = -2;
                while (hDJI[0].AxisData.AxisAngle_inDegree < -3.0f) {
                    osDelay(1);
                }
                Right_Servo_Open();
                osDelay(2000);
                if (chassis_message.direction_message == 0xAA && chassis_message.grip_resolve_message == 0xAA) {
                    switch (chassis_message.grip_message) {
                        case 1:
                            right_grip_seed_state = Grip;
                            break;
                        case 2:
                            right_grip_seed_state = Grip_Ready;
                            break;
                        case 3:
                            right_grip_seed_state = Grip_Ready;
                            break;
                        default:
                            break;
                    }
                }
                break;
            case Grip: // 2. 取苗并准备存储
                Right_Servo_Grip();
                osDelay(100);
                right_land_angle = -695;
                while (hDJI[0].AxisData.AxisAngle_inDegree > -693.0f) {
                    osDelay(1);
                }
                osDelay(300);
                if (chassis_message.direction_message == 0xAA && chassis_message.grip_resolve_message == 0xAA) {
                    switch (chassis_message.grip_message) {
                        case 2:
                            right_grip_seed_state = Grip_Deposit;
                            break;
                        default:
                            right_grip_seed_state = Grip;
                            break;
                    }
                }
                break;
            case Grip_Deposit: // 3. 苗的存储
                if (right_seed_count < 4) {
                    unitree_usart_state = osMutexAcquire(Unitree_usart_mutex, osWaitForever);
                    if (unitree_usart_state == osOK) {
                        Unitree_UART_tranANDrev(unitree_motor_right, 0, 1, 0, 0, (unitree_offset_right + 0.3), 0.09, 0.05);
                    }
                    osMutexRelease(Unitree_usart_mutex);
                    Right_Servo_Buffle_Close();
                } else {
                    unitree_usart_state = osMutexAcquire(Unitree_usart_mutex, osWaitForever);
                    if (unitree_usart_state == osOK) {
                        Unitree_UART_tranANDrev(unitree_motor_right, 0, 1, 0, 0, (unitree_offset_right + 0.4), 0.09, 0.05);
                    }
                    osMutexRelease(Unitree_usart_mutex);
                }
                osDelay(2500);
                Right_Servo_Open();
                osDelay(1000);
                unitree_usart_state = osMutexAcquire(Unitree_usart_mutex, osWaitForever);
                if (unitree_usart_state == osOK) {
                    Unitree_UART_tranANDrev(unitree_motor_right, 0, 1, 0, 0, unitree_offset_right - _PI - 0.1, 0.09, 0.05);
                }
                osMutexRelease(Unitree_usart_mutex);
                osDelay(2000);
                if (right_seed_count < 4) {
                    right_deposit_angle = right_deposit_point[1];
                    while (hDJI[1].AxisData.AxisAngle_inDegree < (right_deposit_point[1] - 2.0f)) {
                        osDelay(1);
                    }
                    right_deposit_angle = right_deposit_point[0];
                    while (hDJI[1].AxisData.AxisAngle_inDegree > (right_deposit_point[0] + 2.0f)) {
                        osDelay(1);
                    }
                }
                Right_Servo_Buffle_Open();
                osDelay(100);
                right_seed_count++;
                right_grip_seed_state = Grip_Ready;
                break;
            default:
                break;
        }
        osDelay(2);
    }
}

/**
 * @brief   种苗状态机
 */
void Right_Plant_Seed_Task(void *argument)
{
    for (;;) {
        switch (right_plant_seed_state) {
            case Plant_Ready: // 1. 放苗准备
                if (chassis_message.direction_message == 0xAA && chassis_message.grip_resolve_message == 0xBB) {
                    switch (chassis_message.grip_message) {
                        case 1:
                            right_plant_seed_state = Plant;
                            break;
                        default:
                            right_plant_seed_state = Plant_Ready;
                            break;
                    }
                }
                break;
            case Plant:
                Right_Servo_Deposit_Open();
                osDelay(3000);
                Right_Servo_Deposit_Close();
                osDelay(1000);
                right_seed_count--;
                right_plant_seed_state = Plant_Deposit;
                break;
            case Plant_Deposit:
                if (right_seed_count >= 4) // 放下苗后，存储苗区有超过四个苗
                {
                    Right_Servo_Buffle_Close();
                    osDelay(500);
                    right_deposit_angle = right_deposit_point[1];
                    while (hDJI[1].AxisData.AxisAngle_inDegree < (right_deposit_point[1] - 2.0f)) {
                        osDelay(1);
                    }
                    right_deposit_angle = right_deposit_point[0];
                    while (hDJI[1].AxisData.AxisAngle_inDegree > (right_deposit_point[0] + 2.0f)) {
                        osDelay(1);
                    }
                    osDelay(100);
                } else if (right_seed_count == 3) {
                    Right_Servo_Buffle_Close();
                    osDelay(500);
                    right_deposit_angle = right_deposit_point[2];
                    while (hDJI[1].AxisData.AxisAngle_inDegree < (right_deposit_point[2] - 2.0f)) {
                        osDelay(1);
                    }
                    right_deposit_angle = right_deposit_point[0];
                    while (hDJI[1].AxisData.AxisAngle_inDegree > (right_deposit_point[0] + 2.0f)) {
                        osDelay(1);
                    }
                    osDelay(100);
                } else if (right_seed_count == 2) {
                    Right_Servo_Buffle_Close();
                    osDelay(500);
                    right_deposit_angle = right_deposit_point[3];
                    while (hDJI[1].AxisData.AxisAngle_inDegree < (right_deposit_point[3] - 2.0f)) {
                        osDelay(1);
                    }
                    right_deposit_angle = right_deposit_point[0];
                    while (hDJI[1].AxisData.AxisAngle_inDegree > (right_deposit_point[0] + 2.0f)) {
                        osDelay(1);
                    }
                    osDelay(100);
                } else if (right_seed_count == 1) {
                    Right_Servo_Buffle_Close();
                    osDelay(500);
                    right_deposit_angle = right_deposit_point[4];
                    while (hDJI[1].AxisData.AxisAngle_inDegree < (right_deposit_point[4] - 2.0f)) {
                        osDelay(1);
                    }
                    right_deposit_angle = right_deposit_point[0];
                    while (hDJI[1].AxisData.AxisAngle_inDegree > (right_deposit_point[0] + 2.0f)) {
                        osDelay(1);
                    }
                    osDelay(100);
                } else {
                    ;
                }
                right_plant_seed_state = Plant_Ready;
                break;
            default:
                break;
        }
        osDelay(2);
    }
}

/**
 * @brief   左侧取苗状态机
 */
void Left_Grip_Seed_Task(void *argument)
{
    static osStatus_t unitree_usart_state;
    for (;;) {
        switch (left_grip_seed_state) {
            case Grip_Ready: // 1. 取苗准备
                unitree_usart_state = osMutexAcquire(Unitree_usart_mutex, osWaitForever);
                if (unitree_usart_state == osOK) {
                    Unitree_UART_tranANDrev(unitree_motor_left, 1, 1, 0, 0, unitree_offset_left + _PI + 0.1, 0.09, 0.05);
                }
                osMutexRelease(Unitree_usart_mutex);
                left_land_angle = -2;
                while (hDJI[2].AxisData.AxisAngle_inDegree < -3.0f) {
                    osDelay(1);
                }
                Left_Servo_Open();
                osDelay(2000);
                if (chassis_message.direction_message == 0xBB && chassis_message.grip_resolve_message == 0xAA) {
                    switch (chassis_message.grip_message) {
                        case 1:
                            left_grip_seed_state = Grip;
                            break;
                        case 2:
                            left_grip_seed_state = Grip_Ready;
                            break;
                        case 3:
                            left_grip_seed_state = Grip_Ready;
                            break;
                        default:
                            break;
                    }
                }
                break;
            case Grip: // 2. 取苗并准备存储
                Left_Servo_Grip();
                osDelay(100);
                left_land_angle = -695;
                while (hDJI[2].AxisData.AxisAngle_inDegree > -693.0f) {
                    osDelay(1);
                }
                osDelay(300);
                if (chassis_message.direction_message == 0xBB && chassis_message.grip_resolve_message == 0xAA) {
                    switch (chassis_message.grip_message) {
                        case 2:
                            left_grip_seed_state = Grip_Deposit;
                            break;
                        default:
                            left_grip_seed_state = Grip;
                            break;
                    }
                }
                break;
            case Grip_Deposit: // 3. 苗的存储
                if (left_seed_count < 4) {
                    unitree_usart_state = osMutexAcquire(Unitree_usart_mutex, osWaitForever);
                    if (unitree_usart_state == osOK) {
                        Unitree_UART_tranANDrev(unitree_motor_left, 1, 1, 0, 0, (unitree_offset_left - 0.3), 0.09, 0.05);
                    }
                    osMutexRelease(Unitree_usart_mutex);
                    Left_Servo_Buffle_Close();
                } else {
                    unitree_usart_state = osMutexAcquire(Unitree_usart_mutex, osWaitForever);
                    if (unitree_usart_state == osOK) {
                        Unitree_UART_tranANDrev(unitree_motor_left, 1, 1, 0, 0, (unitree_offset_left - 0.4), 0.09, 0.05);
                    }
                    osMutexRelease(Unitree_usart_mutex);
                }
                osDelay(2500);
                Left_Servo_Open();
                osDelay(1000);
                unitree_usart_state = osMutexAcquire(Unitree_usart_mutex, osWaitForever);
                if (unitree_usart_state == osOK) {
                    Unitree_UART_tranANDrev(unitree_motor_left, 1`, 1, 0, 0, unitree_offset_left + _PI + 0.1, 0.09, 0.05);
                }
                osMutexRelease(Unitree_usart_mutex);
                osDelay(2000);
                if (left_seed_count < 4) {
                    left_deposit_angle = left_deposit_point[1];
                    while (hDJI[3].AxisData.AxisAngle_inDegree > (left_deposit_point[1] + 2.0f)) {
                        osDelay(1);
                    }
                    left_deposit_angle = left_deposit_point[0];
                    while (hDJI[3].AxisData.AxisAngle_inDegree < (left_deposit_point[0] - 2.0f)) {
                        osDelay(1);
                    }
                }
                Left_Servo_Buffle_Open();
                osDelay(100);
                left_seed_count++;
                left_grip_seed_state = Grip_Ready;
                break;
            default:
                break;
        }
        osDelay(2);
    }
}

/**
 * @brief   左侧种苗状态机
 */
void Left_Plant_Seed_Task(void *argument)
{
    for (;;) {
        switch (left_plant_seed_state) {
            case Plant_Ready: // 1. 放苗准备
                if (chassis_message.direction_message == 0xBB && chassis_message.grip_resolve_message == 0xBB) {
                    switch (chassis_message.grip_message) {
                        case 1:
                            left_plant_seed_state = Plant;
                            break;
                        default:
                            left_plant_seed_state = Plant_Ready;
                            break;
                    }
                }
                break;
            case Plant:
                Left_Servo_Deposit_Open();
                osDelay(3000);
                Left_Servo_Deposit_Close();
                osDelay(1000);
                left_seed_count--;
                left_plant_seed_state = Plant_Deposit;
                break;
            case Plant_Deposit:
                if (left_seed_count >= 4) // 放下苗后，存储苗区有超过四个苗
                {
                    Left_Servo_Buffle_Close();
                    osDelay(500);
                    left_deposit_angle = left_deposit_point[1];
                    while (hDJI[3].AxisData.AxisAngle_inDegree > (left_deposit_point[1] + 2.0f)) {
                        osDelay(1);
                    }
                    left_deposit_angle = left_deposit_point[0];
                    while (hDJI[3].AxisData.AxisAngle_inDegree < (left_deposit_point[0] - 2.0f)) {
                        osDelay(1);
                    }
                    osDelay(100);
                } else if (left_seed_count == 3) {
                    Left_Servo_Buffle_Close();
                    osDelay(500);
                    left_deposit_angle = left_deposit_point[2];
                    while (hDJI[3].AxisData.AxisAngle_inDegree > (left_deposit_point[2] + 2.0f)) {
                        osDelay(1);
                    }
                    left_deposit_angle = left_deposit_point[0];
                    while (hDJI[3].AxisData.AxisAngle_inDegree < (left_deposit_point[0] - 2.0f)) {
                        osDelay(1);
                    }
                    osDelay(100);
                } else if (left_seed_count == 2) {
                    Left_Servo_Buffle_Close();
                    osDelay(500);
                    left_deposit_angle = left_deposit_point[3];
                    while (hDJI[3].AxisData.AxisAngle_inDegree > (left_deposit_point[3] + 2.0f)) {
                        osDelay(1);
                    }
                    left_deposit_angle = left_deposit_point[0];
                    while (hDJI[3].AxisData.AxisAngle_inDegree < (left_deposit_point[0] - 2.0f)) {
                        osDelay(1);
                    }
                    osDelay(100);
                } else if (left_seed_count == 1) {
                    Left_Servo_Buffle_Close();
                    osDelay(500);
                    left_deposit_angle = left_deposit_point[4];
                    while (hDJI[3].AxisData.AxisAngle_inDegree > (left_deposit_point[4] + 2.0f)) {
                        osDelay(1);
                    }
                    left_deposit_angle = left_deposit_point[0];
                    while (hDJI[3].AxisData.AxisAngle_inDegree < (left_deposit_point[0] - 2.0f)) {
                        osDelay(1);
                    }
                    osDelay(100);
                } else {
                    ;
                }
                left_plant_seed_state = Plant_Ready;
                break;
            default:
                break;
        }
        osDelay(2);
    }
}