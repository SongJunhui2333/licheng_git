#include "imageProcess.h"

// 图像处理函数
void imageProcess(uint8 image[MT9V03X_H][MT9V03X_W])
{
    uint16 i, j;
    // uint16 edgeL[MT9V03X_H];   // 左边线位置
    // uint16 edgeR[MT9V03X_H];   // 右边线位置
    // uint16 midline[MT9V03X_H]; // 中线位置

    // edgeL[MT9V03X_H - 1] = 0;               // 初始化底部左边线位置为图像最左侧
    // edgeR[MT9V03X_H - 1] = MT9V03X_W - 1;   // 初始化底部右边线位置为图像最右侧
    // midline[MT9V03X_H - 1] = MT9V03X_W / 2; // 初始化底部中线位置为图像中间

    //     // 从下往上寻找边界线位置
    // for (i = MT9V03X_H - 2;; i--)
    // {
    //     edgeL[i] = 0;
    //     edgeR[i] = MT9V03X_W - 1;

    //     for (j = midline[i + 1]; j > 0; j--)
    //     {
    //         if (image[i][j] == 0)
    //         {
    //             edgeL[i] = j;
    //             break;
    //         }
    //     }

    //     for (j = midline[i + 1]; j < MT9V03X_W - 1; j++)
    //     {
    //         if (image[i][j] == 0)
    //         {
    //             edgeR[i] = j;
    //             break;
    //         }
    //     }

    //     midline[i] = (edgeL[i] + edgeR[i]) / 2; // 考虑环岛补线后不能边判断边线边计算中线，这个中线只用来下一步寻找边界

    //     if (i == 0)
    //     {
    //         break;
    //     }
    // }

    // 寻找边线位置
    auxiliary_process((uint8_t *)image, MT9V03X_H, MT9V03X_W, threshold, left_line, mid_line, right_line);

    /*----环岛处理在这里开始----*/

    uint8 jumpFlagL = 0; // 左边线跳变标志位
    uint8 jumpFlagR = 0; // 右边线跳变标志位

    uint16 pointLX = 0; // 左边线跳变点X坐标
    uint16 pointLY = 0; // 左边线跳变点Y坐标
    uint16 pointRX = 0; // 右边线跳变点X坐标
    uint16 pointRY = 0; // 右边线跳变点Y坐标
    uint16 pointX = 0;  // 跳变点X坐标
    uint16 pointY = 0;  // 跳变点Y坐标

    uint8 pointSide = 0; // 跳变点方向，1表示左，2表示右
    uint8 pointType = 0; // 跳变点分类，1表示A字跳变点，2表示V字跳变点

    for (i = MT9V03X_H * 1 / 3; i < MT9V03X_H - 1; i++)
    { // 只判断下2/3部分图像的跳变点
        // 左侧A字跳变点
        if (left_line[i] - left_line[i - 1] > 20)
        {
            jumpFlagL = 1;          // 标记出现左侧跳变点
            pointLX = left_line[i]; // 记录左侧跳变点X坐标
            pointLY = i;            // 记录左侧跳变点Y坐标
            pointSide = 1;          // 跳变方向为左侧
            pointType = 1;          // 跳变类型为A字跳变点
        }
        // 左侧V字跳变点
        else if (left_line[i - 1] - left_line[i] > 20)
        {
            jumpFlagL = 1;              // 标记出现左侧跳变点
            pointLX = left_line[i - 1]; // 记录左侧跳变点X坐标
            pointLY = i - 1;            // 记录左侧跳变点Y坐标
            pointSide = 1;              // 跳变方向为左侧
            pointType = 2;              // 跳变类型为V字跳变点
        }
        // 右侧A字跳变点
        if (right_line[i - 1] - right_line[i] > 20)
        {
            jumpFlagR = 1;           // 标记出现右侧跳变点
            pointRX = right_line[i]; // 记录右侧跳变点X坐标
            pointRY = i;             // 记录右侧跳变点Y坐标
            pointSide = 2;           // 跳变方向为右侧
            pointType = 1;           // 跳变类型为A字跳变点
        }
        // 右侧V字跳变点
        else if (right_line[i] - right_line[i - 1] > 20)
        {
            jumpFlagR = 1;               // 标记出现右侧跳变点
            pointRX = right_line[i - 1]; // 记录右侧跳变点X坐标
            pointRY = i - 1;             // 记录右侧跳变点Y坐标
            pointSide = 2;               // 跳变方向为右侧
            pointType = 2;               // 跳变类型为V字跳变点
        }
    }

    // tft180_show_int(0, 130, pointSide, 1);
    // tft180_show_int(50, 130, pointType, 1);

    // 只在单侧跳变且跳变点较低时认为是有效跳变点
    uint8 jumpFlag = (jumpFlagL ^ jumpFlagR) && ((pointLY > MT9V03X_H / 2) || (pointRY > MT9V03X_H / 2)); // 防止十字误判

    if ((pointLX == MT9V03X_W / 2) && (pointRX == MT9V03X_W / 2) && (pointLY > MT9V03X_H / 2) && (pointRY > MT9V03X_H / 2))
    {
        jumpFlag = 1; // 入环V形特殊情况
    }

    if (jumpFlagL == 1)
    {
        pointX = pointLX;
        pointY = pointLY;
    }
    else if (jumpFlagR == 1)
    {
        pointX = pointRX;
        pointY = pointRY;
    }

    // 四步过环，遇环、入环、出环、过环
    static uint8 meetRingFlag = 0;  // 遇环标志位，第一个A字标志点——第一个V字标志点
    static uint8 enterRingFlag = 0; // 入环标志位，第一个V字标志点——第二个A字标志点
    static uint8 leaveRingFlag = 0; // 出环标志位，第二个A字标志点——第二个V字标志点
    static uint8 passRingFlag = 1;  // 过环标志位，第二个V字标志点——下一个A字标志点
    static uint8 ringSide = 0;      // 环岛类型，1表示左，2表示右

    // 图像左下角与右下角坐标（用于补线插值的起点）
    // pointLLC*: 左下角 (Left Lower Corner)
    // pointLRC*: 右下角 (Right Lower Corner)
    // 这些点作为补线时的基准点，和检测到的跳变点 (pointX, pointY) 连线，从而向上逐行补齐丢失的边线。
    uint16 pointLLCX = 0;             // 左下角 X 坐标，图像左边界
    uint16 pointLLCY = MT9V03X_H - 1; // 左下角 Y 坐标，图像底部（最后一行）
    uint16 pointLRCX = MT9V03X_W - 1; // 右下角 X 坐标，图像右边界
    uint16 pointLRCY = MT9V03X_H - 1; // 右下角 Y 坐标，图像底部（最后一行）

    float stepLength; // 横坐标插值步长

    if (jumpFlag)
    { // 若有有效跳变点
        if (pointType == 1)
        { // 若为A字跳变点，说明出环或遇环
            if (passRingFlag == 1)
            { // 遇环//遇环仅判断方向用
                meetRingFlag = 1;
                passRingFlag = 0;
            }
            else if (enterRingFlag == 1)
            { // 出环
                leaveRingFlag = 1;
                enterRingFlag = 0;
            }
        }
        else if (pointType == 2)
        { // 若为V字跳变点，说明入环或过环
            if (meetRingFlag == 1)
            { // 入环
                enterRingFlag = 1;
                meetRingFlag = 0;
            }
            else if (leaveRingFlag == 1)
            { // 过环
                passRingFlag = 1;
                leaveRingFlag = 0;
            }
        }
    }

    tft180_show_int(0, 100, meetRingFlag, 1);
    tft180_show_int(20, 100, enterRingFlag, 1);
    tft180_show_int(40, 100, leaveRingFlag, 1);
    tft180_show_int(60, 100, passRingFlag, 1);
    tft180_show_int(0, 130, ringSide, 1);

    if (meetRingFlag == 1) // 遇到环
    {
        if (jumpFlag == 1) // 出现跳变点
        {
            ringSide = pointSide; // 环岛方向与跳变点方向一致
        }
    }
    else if (enterRingFlag == 1)
    {
        if (jumpFlag == 1)
        {
            // 入环补线
            // 入环标志点（pointAX，pointAY），图像左下角（pointLLCX，pointLLCY），图像右下角（pointLRCX，pointLRCY）
            if (ringSide == 1)
            { // 入环标志点在左边，从右下角点到入环标志点连线，补右边线
                stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
                for (i = 0; i < pointLRCY - pointY; i++)
                {
                    right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
                    image[pointLRCY - i][right_line[pointLRCY - i]] = 0; // 显示补线
                }
            }
            else if (ringSide == 2)
            { // 入环标志点在右边，从左下角点到入环标志点连线，补左边线
                stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
                for (i = 0; i < pointLLCY - pointY; i++)
                {
                    left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
                    image[pointLLCY - i][left_line[pointLLCY - i]] = 0; // 显示补线
                }
            }
        }
    }
    else if (leaveRingFlag == 1)
    {
        if (ringSide == 1)
        { // 若左圆环
            for (i = MT9V03X_H * 3 / 4; i < MT9V03X_H - 1; i++)
            { // 判断右边线下部分
                if (left_line[i] == MT9V03X_W - 1)
                {           // 若丢线
                    return; // 不改变偏差
                }
            }
        }
        if (ringSide == 2)
        {
            for (i = MT9V03X_H * 3 / 4; i < MT9V03X_H - 1; i++)
            {
                if (left_line[i] == 0)
                {
                    return;
                }
            }
        }
    }

    else if (passRingFlag == 1)
    {
        if (jumpFlag == 1)
        {
            if (ringSide == 1)
            {
                stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
                for (i = 0; i < pointLLCY - pointY; i++)
                {
                    left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
                    image[pointLLCY - i][left_line[pointLLCY - i]] = 0;
                }
            }
            else if (ringSide == 2)
            {
                stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
                for (i = 0; i < pointLRCY - pointY; i++)
                {
                    left_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
                    image[pointLRCY - i][right_line[pointLRCY - i]] = 0;
                }
            }
        }
    }

    /*----环岛处理在这里结束----*/

    for (i = 0; i < MT9V03X_H; i++)
    { // 重新计算中线
        mid_line[i] = (left_line[i] + right_line[i]) / 2;
        image[i][mid_line[i]] = image[i][mid_line[i] - 1] = image[i][mid_line[i] + 1] = 0; // 粗线
    }
    // lcd_showint8(0, 5, jumpFlag);
    // lcd_showint16(0, 6, meetRingFlag * 1000 + enterRingFlag * 100 + leaveRingFlag * 10 + passRingFlag);

    // 取图像下1/4处的中线位置计算偏差
    offset = MT9V03X_W / 2 - mid_line[MT9V03X_H - MT9V03X_H / 4];
}