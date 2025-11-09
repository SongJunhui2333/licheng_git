#ifndef _Motor_h
#define _Motor_h

#include "main.h"
#include "zf_common_headfile.h"

// 电机初始化
void Motor_Init();

// 设定电机转速
void Motor_Setspeed(int pwm);

#endif