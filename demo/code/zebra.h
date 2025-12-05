#ifndef _ZEBRA_H_
#define _ZEBRA_H_

#include "main.h"
#include "zf_common_headfile.h"
#include "imageProcess.h"

extern uint8 Stop_Flag; // 停车标志位，若其变1则停车
// extern int count;             // 检测一行内黑白跳变数，用于判断斑马线
extern uint8 prev_zebra_flag; // 上一次斑马线标志位状态
extern uint8 zebra_flag;      // 斑马线标志位
extern uint8 zebra_NUM;       // 识别到斑马线的次数

extern uint8 count_Show; // 识别到跳变数的次数

void Zebra_Stripes_Detect(void);

#endif