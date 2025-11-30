#ifndef img_process_h
#define img_process_h

#include "zf_common_headfile.h"
#include "main.h"

extern uint8 pointSide; // 跳变点方向，1表示左，2表示右
extern uint8 pointType; // 跳变点分类，1表示A字跳变点，2表示V字跳变点

void imageProcess(uint8 image[MT9V03X_H][MT9V03X_W]);

#endif