#include "wtr_fire_ball.h"

// /**
//  * @brief   发射
//  */
// void Fire_Action(void)
// {
//     if (fire_state == Fire) {
//         // 1. 滑轨前移进行发射
//         rail_angle = -630;
//         osDelay(3000);
//         // 2. 滑轨后移复位
//         rail_angle = 0;
//         osDelay(3000);
//         fire_state = Fire_Ready;
//     }
// }

// /**
//  * @brief 机械臂抓取并进行供弹
//  */
// void Arm_Grip_Action(void)
// {
//     // 1. 抓取
//     Servo_Grip();
//     osDelay(1000);
//     // 2. 收缩夹爪
//     Servo_In();
//     osDelay(1000);
//     // 3. 机械臂进行供弹
//     arm_angle = 180;
//     osDelay(3000);
//     // 4. 松开,假设此时滑轨已复位
//     while (fire_state != Fire_Ready) {
//         osDelay(1);
//     }
//     Servo_Reset();
//     osDelay(2000);
//     // 5. 执行发射动作
//     arm_angle = 5;
//     osDelay(1000);
//     fire_state = Fire;
//     Fire_Action();
//     // 6. 机械臂回缩
//     Servo_Out();
// }
