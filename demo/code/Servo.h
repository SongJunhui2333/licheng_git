#ifndef _Servo_h
#define _Servo_h
#include "main.h"
#include "zf_common_headfile.h"

//舵机初始化
void Servo_Init();

//控制舵机打角
void Servo_Ctrl(float duty);

#endif