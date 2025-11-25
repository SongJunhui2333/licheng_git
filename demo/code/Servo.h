#ifndef _Servo_h
#define _Servo_h
#include "main.h"
#include "zf_common_headfile.h"

// 舵机初始化
void Servo_Init();

// 控制舵机开环打角
void Servo_Ctrl(float duty);

// 根据中线误差控制舵机闭环打角度
void Servo_Ctrl_Loop(float mid_err);

#endif