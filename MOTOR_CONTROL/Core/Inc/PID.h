#ifndef PID_h
#define PID_h
#include "main.h"
#include "tim.h"

#define OUTPUT_MAX 2000.0


typedef struct
{
		float Kp;  
    float Ki;  
    float Kd;  
    float prepreError;  
    float preError;  
    float curError;
		float output;
		float target;
}PID_t;


void PIDController_init(PID_t* pid, float Kp, float Ki, float Kd, float target);
float PID_Algorithm(PID_t *pid,float measuredVal) ;




#endif

