# 麦克纳姆轮-四轮底盘
0. 计划更新
- [ ] 添加状态修改线程
- [ ] 将所有结构体统一到一个Chassis结构体中
- [ ] 优化文件结构，将所有用户配置放到`UserConfig.h`。

1. 使用方法
	1. 使能FreeRTOS
	2. 是默认线程弱定义
2. 文件介绍
```WTR_CHASSIS\麦克纳姆轮\USERCODE
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
3. 控制方式-大疆遥控
