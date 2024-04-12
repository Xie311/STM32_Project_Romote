/*
 * @Author: szf
 * @Date: 2023-02-22 12:04:21
 * @LastEditTime: 2023-10-06 14:48:00
 * @LastEditors: Chen Yitong 
 * @brief 运动学逆解算及PID计算函数
 * @FilePath: \MyChassisTry\UserCode\Lib\Calculate\wtr_calculate.c
 */

#include "wtr_calculate.h"
#include <math.h>
#include "Chassis_UserConfig.h"

double moter_speed[4];

// 全向轮底盘参数
#define wheel_radius  1.0       //全向轮半径
#define rotate_ratio    0.3615   // 正方形边长 ， 也即 (Width + Length)/2 
#define wheel_rpm_ratio 2387.324 // 减速比，换算线速度到rpm


/**
 * @brief 全向轮底盘逆解算
 * @param moter_speed 每个电机的速度数组
 * @param vx x轴速度
 * @param vy y轴速度
 * @param vw 旋转角速度
 * @return void
 */
void CalculateOmniWheel(double *moter_speed, double vx, double vy, double vw)
{
    // 计算每个轮子的速度
    moter_speed[0] = (vx * cos(M_PI/4) - vy * sin(M_PI/4) - vw * rotate_ratio * wheel_radius) * wheel_rpm_ratio;
    moter_speed[1] = (vx * cos(3*M_PI/4) - vy * sin(3*M_PI/4) - vw * rotate_ratio * wheel_radius) * wheel_rpm_ratio;
    moter_speed[2] = (vx * cos(5*M_PI/4) - vy * sin(5*M_PI/4) - vw * rotate_ratio * wheel_radius) * wheel_rpm_ratio;
    moter_speed[3] = (vx * cos(7*M_PI/4) - vy * sin(7*M_PI/4) - vw * rotate_ratio * wheel_radius) * wheel_rpm_ratio;
}

/**
 * @brief: 计算串级PID控制器的输出串级PID
 * @author：X311 2024/4/13
 * @param pid_params：指向 PIDParameters 结构体的指针，其中包含了PID控制器的所有参数和状态信息
 * @param target_position：目标位置，即期望达到的位置---来自大疆遥控/上位机
 * @param current_position：当前位置，即系统当前的位置--码盘反馈
 * @param current_velocity：当前速度，即系统当前的速度--电机通过CAN回传
 * @return {*}
 * @note:直接先位置再速度封装一下呢（？）
*/
void cascade_pid_control(PIDParameters *pid_params, double target_position, double current_position, double current_velocity) {
    // 外部PID控制器输出（目标速度）
    double error_outer = target_position - current_position;
    pid_params->integral_error_outer += error_outer;
    double derivative_error_outer = error_outer - pid_params->prev_error_outer;
    pid_params->prev_error_outer = error_outer;
    double target_velocity = pid_params->Kp_outer * error_outer + pid_params->Ki_outer * pid_params->integral_error_outer + pid_params->Kd_outer * derivative_error_outer;

    // 内部PID控制器输出（控制量）
    double error_inner = target_velocity - current_velocity;
    pid_params->integral_error_inner += error_inner;
    double derivative_error_inner = error_inner - pid_params->prev_error_inner;
    pid_params->prev_error_inner = error_inner;
    pid_params->control_output = pid_params->Kp_inner * error_inner + pid_params->Ki_inner * pid_params->integral_error_inner + pid_params->Kd_inner * derivative_error_inner;
}


/**
 * @brief: PID控制-增量式PID
 * @auther: Chen YiTong 3083697520
 * @param {__IO PID_t} *pid
 * @return {*}
 */

void PID_Calc(__IO PID_t *pid)
{
    pid->cur_error = pid->ref - pid->fdb;
    pid->output += pid->KP * (pid->cur_error - pid->error[1]) + pid->KI * pid->cur_error + pid->KD * (pid->cur_error - 2 * pid->error[1] + pid->error[0]);
    pid->error[0] = pid->error[1];
    pid->error[1] = pid->ref - pid->fdb;
    /*设定输出上限*/
    if (pid->output > pid->outputMax) pid->output = pid->outputMax;
    if (pid->output < -pid->outputMax) pid->output = -pid->outputMax;
}

/**
 * @brief: PID算法-P控制
 * @auther: Chen YiTong 3083697520
 * @param {__IO PID_t} *pid
 * @return {*}
 */
void P_Calc(__IO PID_t *pid)
{
    pid->cur_error = pid->ref - pid->fdb;
    pid->output    = pid->KP * pid->cur_error;
    /*设定输出上限*/
    if (pid->output > pid->outputMax) pid->output = pid->outputMax;
    if (pid->output < -pid->outputMax) pid->output = -pid->outputMax;

    if (fabs(pid->output) < pid->outputMin)
        pid->output = 0;
}

/**
 * @brief: 位置伺服
 * @auther: Chen YiTong 3083697520
 * @param {float} ref 目标位置
 * @param {DJI_t} *motor 电机结构体
 * @return {*}
 */
void positionServo(float ref, DJI_t *motor)
{

    motor->posPID.ref = ref;
    motor->posPID.fdb = motor->AxisData.AxisAngle_inDegree;

    motor->speedPID.ref = motor->posPID.output;
    motor->speedPID.fdb = motor->FdbData.rpm;
    PID_Calc(&(motor->speedPID));
}

/**
 * @brief: 速度伺服函数
 * @auther: Chen YiTong 3083697520
 * @param {float} ref 目标值
 * @param {DJI_t} *motor 电机结构体
 * @return {*}
 */
void speedServo(float ref, DJI_t *motor)
{
    motor->speedPID.ref = ref;
    motor->speedPID.fdb = motor->FdbData.rpm;
    PID_Calc(&(motor->speedPID));
}

/**
 * @brief: 圆周死区控制
 * @auther: szf
 * @param {double} x
 * @param {double} y
 * @param {double} *new_x
 * @param {double} *new_y
 * @param {double} threshould
 * @return {*}
 */
void DeadBand(double x, double y, double *new_x, double *new_y, double threshould)
{
    double length     = sqrt(x * x + y * y);
    double new_length = length - threshould;

    if (new_length <= 0) {
        *new_x = 0;
        *new_y = 0;
        return;
    }

    double k = new_length / length;

    *new_x = x * k;
    *new_y = y * k;
}

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
