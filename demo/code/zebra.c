#include "zebra.h"
/*@brief 斑马线检测
@param null
@ return null

        Zebra_Stripes_Detect(void)
           // @note 边界起始靠下，最长白列较长，赛道宽度过窄，且附近大量跳变-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -*/

int Zebra_Stripes_Flag = 0;
uint8 Stop_Flag = 0;                // 停车标志位，我们有吗？
volatile int Search_Stop_Line;      // 搜索截止行,只记录长度，想要坐标需要用视野高度减去该值
volatile int Boundry_Start_Left;    // 左边界起始行
volatile int Boundry_Start_Right;   // 右边界起始行
volatile int Road_Wide[MT9V03X_H];  // 赛宽数组
volatile int Left_Line[MT9V03X_H];  // 左边线数组
volatile int Right_Line[MT9V03X_H]; // 右边线数组

int Longest_White_Column_Left[2];  // 最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
int Longest_White_Column_Right[2]; // 最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列

// 标准赛宽，将车子放在长直道上面实测，以下数值仅供参考
const int Standard_Road_Wide[MT9V03X_H] =
    {10, 12, 14, 16, 18, 20, 22, 24, 26, 28,
     30, 32, 34, 36, 38, 40, 42, 44, 46, 48,
     50, 52, 54, 56, 58, 60, 62, 64, 66, 68,
     70, 72, 74, 76, 78, 80, 82, 84, 86, 88,
     90, 92, 94, 96, 98, 100, 102, 104, 106, 108,
     110, 112, 114, 116, 118, 120, 122, 124, 126, 128,
     130, 132, 134, 136, 138, 140, 142, 144, 146, 148};

uint8 image_two_value[MT9V03X_H][MT9V03X_W]; // 二值化后的原数组

void Zebra_Stripes_Detect(void)
{

    int i = 0, j = 0;
    int change_count = 0; // 跳变计数
    int start_line = 0;
    int endl_ine = 0;
    int narrow_road_count = 0;
    if (Zebra_Stripes_Flag != 0 || Stop_Flag != 0) // 元素互斥，不是十字，不是，不是坡道，不是停车
    {
        return;
    }

    ////赛宽变化判斑马线
    if (Search_Stop_Line >= 60 &&
        30 <= Longest_White_Column_Left[1] && Longest_White_Column_Left[1] <= MT9V03X_W - 30 &&
        30 <= Longest_White_Column_Right[1] && Longest_White_Column_Right[1] <= MT9V03X_W - 30 &&
        Boundry_Start_Left >= MT9V03X_H - 15 && Boundry_Start_Right >= MT9V03X_H - 15)
    {                              // 截止行长，.最长白列的位置在中心附近，边界起始点靠下
        for (i = 65; i >= 20; i--) // 在靠下的区域进行寻找赛道宽度过窄的地方
        {
            if ((Standard_Road_Wide[i] - Road_Wide[i]) > 10)
            {
                narrow_road_count++; // 多组赛宽变窄，才认为是斑马线
                if (narrow_road_count >= 5)
                {
                    start_line = i; // 记录赛道宽度很窄的位置
                    break;
                }
            }
        }
    }
    if (start_line != 0) // 多组赛宽变窄，，以赛道过窄的位置为中心，划定一个范围，进行跳变计数
    {
        start_line = start_line + 8;
        endl_ine = start_line - 15;
        if (start_line >= MT9V03X_H - 1) // 限幅保护，防止数组越界
        {
            start_line = MT9V03X_H - 1;
        }
        if (endl_ine <= 0) // 限幅保护，防止数组越界
        {
            endl_ine = 0;
        }
        for (i = start_line; i >= endl_ine; i--) // 区域内跳变计数
        {
            for (j = Left_Line[i]; j <= Right_Line[i]; j++)
            {
                if (image_two_value[i][j + 1] - image_two_value[i][j] != 0)
                {
                    change_count++;
                }
            }
        }
        //        ips200_show_uint(0*16,100,change_count,5);//debug使用，查看跳变数，便于适应赛道
    }
    // 画出区域，便于找bug，debug使用
    //        Draw_Line( Left_Line[start_line], start_line, Left_Line[endl_ine], endl_ine);
    //        Draw_Line( Left_Line[start_line], start_line, Right_Line[start_line], start_line);
    //        Draw_Line(Right_Line[endl_ine], endl_ine, Right_Line[start_line], start_line);
    //        Draw_Line(Right_Line[endl_ine], endl_ine, Left_Line[endl_ine], endl_ine);
    //        ips200_draw_line ( Left_Line[start_line], start_line, Left_Line[endl_ine], endl_ine, RGB565_RED);
    //        ips200_draw_line ( Left_Line[start_line], start_line, Right_Line[start_line], start_line, RGB565_RED);
    //        ips200_draw_line (Right_Line[endl_ine], endl_ine, Right_Line[start_line], start_line, RGB565_RED);
    //        ips200_draw_line (Right_Line[endl_ine], endl_ine, Left_Line[endl_ine], endl_ine, RGB565_RED);

    if (change_count > 30) // 跳变大于某一阈值，认为找到了斑马线
    {
        Zebra_Stripes_Flag = 1;
    }
    // 相关参数显示。debug使用
    //     ips200_show_uint(0*16,50,narrow_road_count,5);
    //     ips200_show_uint(1*16,50,change_count,5);
}