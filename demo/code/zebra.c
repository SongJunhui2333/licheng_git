#include "zebra.h"
#include "Motor.h" // 用于调用 Motor_Setspeed 停车
/*@brief 斑马线检测
@param null
@ return null

        Zebra_Stripes_Detect(void)
           // @note 边界起始靠下，最长白列较长，赛道宽度过窄，且附近大量跳变-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -*/

uint8 Stop_Flag = 0; // 停车标志位，我们有吗？

uint8 zebra_flag = 0;      // 斑马线标志位
uint8 prev_zebra_flag = 0; // 上一次斑马线标志位状态
uint8 zebra_NUM = 0;       // 识别到斑马线的次数
uint8 count_Show = 0;      // 识别到跳变数的次数

void Zebra_Stripes_Detect(void)
{
    int count = 0; // 跳变计数
    if (Stop_Flag)
        return; // 如果已经停车就不再检测

    // 取图像中间一行，检测黑白跳变数
    int line = MT9V03X_H * 5 / 6;
    for (int i = 2; i < MT9V03X_W - 1; i++)
    {
        if (image[line][i - 1] == 0 && image[line][i - 2] == 0 && image[line][i] == 255 && image[line][i + 1] == 255) // 黑到白跳变
        {
            count++;
        }
        else if (image[line][i - 1] == 255 && image[line][i - 2] == 255 && image[line][i] == 0 && image[line][i + 1] == 0) // 白到黑跳变
        {
            count++;
        }
    }

    count_Show = count; // 用于显示跳变数

    if (count >= 7) // 跳变大于某一阈值，认为找到了斑马线
    {
        // 直接设置停车标志并发送停止命令给电机
        // 注意：更稳健的方式是做多帧确认（防抖）并/或使用计时器，这里为最小改动直接停车
        // Stop_Flag = 1;
        zebra_flag = 1; // 设置斑马线标志位
        // Motor_Setspeed(0); // 将电机 PWM 设为 0，立即停车
    }
    else
    {
        zebra_flag = 0; // 未检测到斑马线，清除标志位
    }
    // 相关参数显示。debug使用
    //     ips200_show_uint(0*16,50,narrow_road_count,5);
    //     ips200_show_uint(1*16,50,change_count,5);

    if (prev_zebra_flag - zebra_flag >= 1)
    {
        zebra_NUM++; // 识别到斑马线次数加1
    }

    if (zebra_NUM >= 2)
    {
        Stop_Flag = 1; // 如果识别到两次斑马线则停车
    }

    // 识别一帧后
    // count = 0; // 清除跳变计数

    prev_zebra_flag = zebra_flag; // 更新上一次斑马线标志位状态
}