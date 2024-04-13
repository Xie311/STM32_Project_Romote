# Omni_Chassis_Note

## 0. 使用方法

1. 使能FreeRTOS
2. 使默认线程弱定义

> 在`UserCode\Chassis\Start\Chassis_Start.c` 中重定义了以下函数，所以CubeMX中应将默认线程设置为弱定义

```c
/**
 * @brief 默认任务，启动其他线程
 * @return {*}
 */
void StartDefaultTask(void const *argument)
{
    //================初始化====================
    Chassis_RemoteCtl_Init();    // 遥控初始化
    Chassis_StateMachine_Init(); // 状态机初始化
    Chassis_Servo_Init();        // 底盘电机初始化
    //================启动线程==================
    Chassis_StateMachine_TaskStart(); // 状态机进程启动
    Chassis_Servo_TaskStart();        // 底盘伺服进程启动

    for (;;)
    {
    }
}
```

**对于全自动无大疆遥控小车**

`Callback`、`RemoteCtl`、`Decode`  都不再需要

数据不再来自大疆遥控，而是来自上位机

有关大疆遥控的变量都在`Chassis_StateMachine.c`中

## 一、文件介绍

```c
├─Chassis
│  ├─Callback                           底盘中断回调函数：包含大疆遥控器解码。
│  │      Chassis_Callback.c            
│  │      Chassis_Callback.h
│  ├─Config
│  │      Chassis_commen.h              底盘所有结构体
│  │      Chassis_UserConfig.h          底盘端口定义 部分数学宏定义 部分数学函数
│  │      Chassis_Variable.h            定义extern变量
│  ├─RemoteCtl                          遥控初始化
│  │      Chassis_RemoteCtl.c           
│  │      Chassis_RemoteCtl.h
│  ├─Servo                              底盘伺服函数-直接驱动电机
│  │      Chassis_Servo.c               带有速度规划函数（虽然不知道放着干啥）
│  │      Chassis_Servo.h
│  ├─Start                              FreeRTOS启动文件 
│  │      Chassis_Start.c             
│  │      Chassis_Start.h
│  └─StateMachine                       底盘状态机：列出所有底盘状态
│          Chassis_StateMachine.c
│          Chassis_StateMachine.h
└─Lib
    ├─Calculate                         PID以及麦克纳姆底盘解算
    │      wtr_calculate.c
    │      wtr_calculate.h
    └─...
```

## 二、食用思路

### 1.Config

**在结构体内使用CMSIS V1方法（？）定义了互斥锁，所以在此文件`UserCode\Chassis\Config\Chassis_commen.h`中需`#include "semphr.h"`**

```c
typedef enum {
    Stop       = 1, // 停止
    Run        = 3, // 启动
    Correcting = 2, // 底盘校准
} CHASSIS_STATE;    // 底盘状态

typedef __IO struct
{
    __IO struct {
        double x;
        double y;
        double w;
    } velocity; // 速度结构体

    __IO struct {
        double x;
        double y;
        double w;
    } position;
    SemaphoreHandle_t  xMutex_control; // 互斥锁

} CHASSIS_MOVING_STATE; // 底盘运动状态


typedef __IO struct {
    DJI_t *hDJI[4];
    SemaphoreHandle_t xMutex_wheel; // 互斥锁
} WHEEL_COMPONENT; //轮子控制，在Chssis_Variable中声明，在Servo.c中被调用
```

### 2.Start

**通过`UserCode\Chassis\Start\Chassis_Start.c`中的默认任务启动其他线程**

```c
void StartDefaultTask(void const *argument)
{
    //================初始化====================
    Chassis_RemoteCtl_Init();    // 遥控初始化
    Chassis_StateMachine_Init(); // 状态机初始化
    Chassis_Servo_Init();        // 底盘电机初始化
    //================启动线程==================
    Chassis_StateMachine_TaskStart(); // 状态机进程启动
    Chassis_Servo_TaskStart();        // 底盘伺服进程启动

    for (;;)
    {
    }
}
```

> 所以`CHASSIS_Start.h`里 include了一堆头文件

### 3.状态机

在`UserCode\Chassis\StateMachine\Chassis_StateMachine.c` 文件中

#### （1）状态机初始化

通过 ` Chassis_StateMachine_Init`函数进行状态机初始化

> 注释部分为trail for CMSIS V2，但寄了

```c
/**
 * @brief 初始化状态机
 * @return {*}
 */

void Chassis_StateMachine_Init()
{
    ChassisControl.xMutex_control = xSemaphoreCreateRecursiveMutex();
    ChassisState.xMutex_control   = xSemaphoreCreateRecursiveMutex();
    WheelComponent.xMutex_wheel   = xSemaphoreCreateRecursiveMutex();
    // const osMutexAttr_t mutex_attr = 
    // {
    //     .name = "Chassis_Mutex"
    // };
    
    // ChassisControl.xMutex_control = osMutexNew(&mutex_attr);
    // ChassisState.xMutex_control = osMutexNew(&mutex_attr);
    // WheelComponent.xMutex_wheel = osMutexNew(&mutex_attr);
 }
```

**在这里创建了三个互斥锁**

#### （2）状态机线程启动

> 注释部分为原CMSIS V1代码

```c
/**
 * @brief: 状态机线程启动
 * @return {*}
 */
void Chassis_StateMachine_TaskStart()
{
    // osThreadDef(Chassis_StateMachine, Chassis_StateMachine_Task, osPriorityAboveNormal, 0, 1024);
    // osThreadCreate(osThread(Chassis_StateMachine), NULL);
    
    // 定义线程属性
    const osThreadAttr_t Chassis_StateMachine_attr = 
    {
        .name = "Chassis_StateMachine",
        .priority = osPriorityAboveNormal,
        .stack_size = 1024
    };
    osThreadNew(Chassis_StateMachine_Task, NULL, &Chassis_StateMachine_attr);

}
```

此线程调用的函数为

```c
/**
 * @brief: 状态机线程
 * @return {*}
 */
void Chassis_StateMachine_Task(void const *argument)
{
    for (;;) {
        osKernelLock();                                       // 锁住内核，防止多个任务同时访问 RemoteCtl_RawData
        Remote_t RemoteCtl_RawData_tmp = RemoteCtl_RawData;   // 复制遥控器数据到临时变量
        osKernelUnlock();                                     // 解锁内核
        switch (RemoteCtl_RawData_tmp.left) {
            case Stop:
                // 获取底盘控制的互斥锁，防止多任务同时修改底盘控制数据
                xSemaphoreTakeRecursive(ChassisControl.xMutex_control, portMAX_DELAY);
                ChassisControl.velocity.x = 0;
                ChassisControl.velocity.y = 0;
                ChassisControl.velocity.w = 0;
                // 释放底盘控制的互斥锁
                xSemaphoreGiveRecursive(ChassisControl.xMutex_control);
                break;
            case Run:
                // 获取底盘控制的互斥锁，防止多任务同时修改底盘控制数据
                xSemaphoreTakeRecursive(ChassisControl.xMutex_control, portMAX_DELAY);
                // 根据遥控器输入设置底盘速度，同时进行死区处理
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch0 - 1024) * 0.001, &(ChassisControl.velocity.x), 0.05);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch1 - 1024) * 0.001, &(ChassisControl.velocity.y), 0.05);
                DeadBandOneDimensional((RemoteCtl_RawData_tmp.ch2 - 1024) * 0.001, &(ChassisControl.velocity.w), 0.05);
                // 释放底盘控制的互斥锁
                xSemaphoreGiveRecursive(ChassisControl.xMutex_control);
                break;
            case Correcting:

                break;
            default:
                break;
        }
        osDelay(10);
    }
}
```

其中的死区处理：

```c
/**
 * @brief: 单轴死区控制
 * @auther: szf
 * @param {double} x
 * @param {double} *new_x
 * @param {double} threshould
 * @return {*}
 * @note   对输入值进行死区处理，防止过小的输入值对系统产生影响
 */
void DeadBandOneDimensional(double x, double *new_x, double threshould)
{
    // 计算输入值的绝对值与死区阈值的差值
    double difference_x = fabs(x) - threshould;
    
    // 如果差值小于0，即输入值在死区范围内
    if (difference_x < 0) {
        *new_x = 0;
        return;
    }

    // 计算缩放因子k，用于对输入值进行缩放
    double k = difference_x / fabs(x);
    *new_x   = k * x;
}
```

### 4.Servo

#### (1）底盘电机初始化

```c
/**
 * @brief: 伺服初始化
 * @return {*}
 */
void Chassis_Servo_Init()
{
    Chassis_Servo_DjiMotorInit();
}
```
**若选用其他电机型号），则需在这里修改motorType**

```c
/**
 * @brief 大疆电机初始化
 * @return {*}
 */
void Chassis_Servo_DjiMotorInit()
{
    CANFilterInit(&hcan1);
    WheelComponent.hDJI[0] = &hDJI[0];
    WheelComponent.hDJI[1] = &hDJI[1];
    WheelComponent.hDJI[2] = &hDJI[2];
    WheelComponent.hDJI[3] = &hDJI[3];
    hDJI[0].motorType      = M3508;
    hDJI[1].motorType      = M3508;
    hDJI[2].motorType      = M3508;
    hDJI[3].motorType      = M3508;
    DJI_Init();
}
```

定义指路：

```c
typedef __IO struct {
    DJI_t *hDJI[4];
    SemaphoreHandle_t xMutex_wheel; // 互斥锁
} WHEEL_COMPONENT; //轮子控制，在Chssis_Variable中声明，在Servo.c中被调用

--- 
typedef __IO struct {

    DJI_e motorType;

    struct {
        int msg_cnt;            // 收到消息数
        float RotorAngle_0_360; // 电机转子角度 单位 度° 范围0~360
        float rpm;              // 电机转子速度，单位 rpm
        float current;          // 电机转矩电流
    } FdbData;

    struct {
        float RotorAngle_all;          // 电机总角度 单位 度° 范围-inf~inf
        float RotorAngle_0_360_Log[2]; // NOW->电机当前位置0~360 LAST->电机上一次更新位置0~360
        float RotorAngle_0_360_OffSet; // 电机机械角度偏移量 单位 度°
        int RotorRound;                // 电机转的圈数
    } Calculate;
    struct {
        float AxisAngle_inDegree; // 电机轴输出角度 单位 度°
        float AxisVelocity;       // 电机轴输出速度 单位 rpm
    } AxisData;

    float reductionRate;      // 减速比
    float encoder_resolution; // 编码器分辨率

    PID_t speedPID;
    PID_t posPID;
} DJI_t;
```

#### （2）启动底盘伺服线程

> 注释部分为原CMSIS V1代码

```c
void Chassis_Servo_TaskStart()
 {
    //     osThreadDef(Chassis_Servo, Chassis_Servo_Task, osPriorityHigh, 0, 1024);
    //     osThreadCreate(osThread(Chassis_Servo), NULL);
    osThreadAttr_t Chassis_Servo_attributes = {
    .name = "Chassis_Servo",
    .priority = osPriorityHigh,
    .stack_size = 1024
    };
    osThreadNew(Chassis_Servo_Task,&Chassis_Servo_attributes,NULL);
}
```

此线程调用的函数为

```c
/**
 * @brief: 伺服线程
 * @return {*}
 */
void Chassis_Servo_Task(void const *argument)
{
    // 延时1秒，等待系统稳定（？）
    osDelay(1000);
    for (;;) {
        xSemaphoreTakeRecursive(ChassisControl.xMutex_control, portMAX_DELAY);
        // 复制底盘控制数据到临时变量
        CHASSIS_MOVING_STATE ChassisControl_tmp = ChassisControl;
        xSemaphoreGiveRecursive(ChassisControl.xMutex_control);
        
        //创建一个数组存储四个电机的速度
        double motor_velocity[4] = {0};
        CalculateOmniWheel(motor_velocity,
                                   ChassisControl_tmp.velocity.x,
                                   ChassisControl_tmp.velocity.y,
                                   ChassisControl_tmp.velocity.w);
        //创建一个数组存储四个电机的状态                           
        DJI_t hDJI_tmp[4];

        vPortEnterCritical();
        // 将电机状态复制到临时数组中
        for (int i = 0; i < 4; i++) { memcpy(&(hDJI_tmp[i]), WheelComponent.hDJI[i], sizeof(DJI_t)); }
        vPortExitCritical();

        // 遍历四个电机，根据计算得到的速度调整电机状态
        for (int i = 0; i < 4; i++) { speedServo(motor_velocity[i], &(hDJI_tmp[i])); }
        // 将调整后的电机状态通过CAN总线发送出去
        CanTransmit_DJI_1234(&hcan_Dji,
                             hDJI_tmp[0].speedPID.output,
                             hDJI_tmp[1].speedPID.output,
                             hDJI_tmp[2].speedPID.output,
                             hDJI_tmp[3].speedPID.output);

        vPortEnterCritical();
        // 将调整后的电机状态复制回原始数组中
        for (int i = 0; i < 4; i++) { memcpy(WheelComponent.hDJI[i], &(hDJI_tmp[i]), sizeof(DJI_t)); }
        vPortExitCritical();

        //控制任务执行频率
        osDelay(10);
    }
}
```

### 5.OPS

**采用PID串级控制**

`[到底什么是串级PID? - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/352407769)`

1.大疆遥控/上位机传来的**（速度？位置？）期望值**与码盘的**（位置）反馈值**进行PID，计算出**PID反馈值**

2.**PID反馈值**与**CAN总线传回的电机速度值**再进行PID，将计算值通过CAN总线发送

---

**！！!  使用OPS解码函数需在 `UserCode\Lib\OPS\wtr_ops.h` 中用户定义段修改串口选项**

---
chatGPT给出的串级PID demo：

```c
#include <stdio.h>

// 定义PID参数结构体
typedef struct {
    double Kp_outer;            // 外层PID参数P
    double Ki_outer;            // 外层PID参数I
    double Kd_outer;            // 外层PID参数D
    double Kp_inner;            // 内层PID参数P
    double Ki_inner;            // 内层PID参数I
    double Kd_inner;            // 内层PID参数D
    double prev_error_inner;    // 内层PID上一次误差
    double prev_error_outer;    // 外层PID上一次误差
    double prev_output;         // 上一次控制量
} PIDParameters;

// 初始化PID参数
void initialize_pid_parameters(PIDParameters *pid_params, double Kp_outer, double Ki_outer, double Kd_outer, double Kp_inner, double Ki_inner, double Kd_inner) {
    pid_params->Kp_outer = Kp_outer;
    pid_params->Ki_outer = Ki_outer;
    pid_params->Kd_outer = Kd_outer;
    pid_params->Kp_inner = Kp_inner;
    pid_params->Ki_inner = Ki_inner;
    pid_params->Kd_inner = Kd_inner;
    pid_params->prev_error_inner = 0.0;
    pid_params->prev_error_outer = 0.0;
    pid_params->prev_output = 0.0; // 初始化上一次控制量
}

// 计算串级增量式PID控制器的输出
void cascade_incremental_pid_control(PIDParameters *pid_params, double target_position, double current_position, double current_velocity) {
    // 外部PID控制器增量（目标速度）
    double error_outer = target_position - current_position;
    double delta_error_outer = error_outer - pid_params->prev_error_outer;
    pid_params->prev_error_outer = error_outer;
    double delta_output_outer = pid_params->Kp_outer * delta_error_outer + pid_params->Ki_outer * error_outer + pid_params->Kd_outer * (delta_error_outer - pid_params->prev_error_inner);

    // 内部PID控制器增量（控制量）
    double error_inner = delta_output_outer - current_velocity;
    double delta_error_inner = error_inner - pid_params->prev_error_inner;
    pid_params->prev_error_inner = error_inner;
    pid_params->prev_output += pid_params->Kp_inner * delta_error_inner + pid_params->Ki_inner * error_inner + pid_params->Kd_inner * (delta_error_inner - (pid_params->prev_output - pid_params->prev_error_inner));
}

// 封装成一个函数
void cascade_pid_control(PIDParameters *pid_params, double target_position, double current_position, double current_velocity) {
    // 调用串级增量式PID控制函数，更新控制量
    cascade_incremental_pid_control(pid_params, target_position, current_position, current_velocity);
}

int main() {
    // 初始化PID参数
    PIDParameters pid_params;
    initialize_pid_parameters(&pid_params, 1.0, 0.1, 0.01, 0.5, 0.05, 0.005);

    // 使用示例
    double target_position = 100.0; // 目标位置
    double current_position = 80.0;  // 当前位置
    double current_velocity = 0.0;   // 当前速度

    // 调用串级PID控制函数，更新控制量
    cascade_pid_control(&pid_params, target_position, current_position, current_velocity);
    printf("Control Output: %lf\n", pid_params.prev_output);

    return 0;
}
```

OPS_Data 返回pos_x、pos_y 、theta
