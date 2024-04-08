//
// Created by Quanquan on 2023/4/23.
//
#include "bsp_Mecanum.h"
float cal_matrix[4][3]={
        {1.0,1.0,-250.0},
        {1.0,-1.0,250.0},
        {1.0,1.0,250.0},
        {1.0,-1.0,-250.0} };
float motor_w[4]={0};
float car_speed[3]={0};
float MOTOR_R=37.5;

/*
 *
 */
void cal_motor_speed(float cal_matrix_[4][3],float* motor_w_,float* car_speed_,float MOTOR_R_)
{
    motor_w_[0]=(cal_matrix_[0][0]*car_speed_[0] + cal_matrix_[0][1]*car_speed_[1] + cal_matrix_[0][2]*car_speed_[2])/MOTOR_R_;
    motor_w_[1]=(cal_matrix_[1][0]*car_speed_[0] + cal_matrix_[1][1]*car_speed_[1] + cal_matrix_[1][2]*car_speed_[2])/MOTOR_R_;
    motor_w_[2]=(cal_matrix_[2][0]*car_speed_[0] + cal_matrix_[2][1]*car_speed_[1] + cal_matrix_[2][2]*car_speed_[2])/MOTOR_R_;
    motor_w_[3]=(cal_matrix_[3][0]*car_speed_[0] + cal_matrix_[3][1]*car_speed_[1] + cal_matrix_[3][2]*car_speed_[2])/MOTOR_R_;

    //printf("%f,%f,%f,%f\n",motor_w_[0]*2*3.1415/5.5*1000.0,motor_w_[1]*2*3.1415/5.5*1000.0,motor_w_[2]*2*3.1415/5.5*1000.0,motor_w_[3]*2*3.1415/5.5*1000.0);
}