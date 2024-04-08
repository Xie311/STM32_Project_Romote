# 各文件作用
```
UserCode
    └─Chassis
       ├─Callback                   中断回调函数    包含串口中断回调函数
       ├─ChangeState                状态修改线程    包含底盘状态修改
       ├─Chassis                    底盘           包含底盘电机初始化、底盘运动学解算、舵轮初始化
       ├─Config                     用户参数设定    
       │    ├─Chassis_commen.h                     包含结构体定义
       │    ├─Chassis_UserConfig.h                 包含车初始参数定义
       │    └─Chassis_Variable.h                   包含extern变量声明
       ├─RemoteCtl                  遥控器初始化    包含大疆遥控器初始化 
       ├─Servo                      伺服线程        包含电机初始化、底盘电机伺服
       ├─Start                      默认启动线程    包含初始化各个部件、启动线程 整个工程的入口 freertos默认任务定义
       └─StateMachine               状态机线程      包含初始化各个锁、启动状态机线程
```

# 各个线程作用
1. Chassis_Init();                      底盘结构体Chassis初始化
2. Chassis_RemoteCtl_Init();            遥控用串口初始化
3. Chassis_StateMachine_Init();         状态机初始化
4. Chassis_Servo_Init();                底盘电机（包括大疆电机2006）初始化
5. Chassis_StateMachine_TaskStart();    状态机线程：罗列各个状态底盘的运动状态
                                            1. Stop： 停止底盘，将Chassis.Contorl vx、xy、vw设定为0 逆解为每个舵轮的行动
                                            2. Run： 读取遥控ch0123，计算死区，设定vx、xy、vw 逆解为每个舵轮的行动
                                            3. Correct： 通过光电门校准舵轮，方式为旋转舵轮一周，记录光电门上升沿下降沿位置，可以考虑常驻
6. Chassis_Servo_TaskStart();           底盘伺服进程启动，读取Chassis.Wheel的数值，发送can数据帧
