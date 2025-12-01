#include "imageProcess.h"

// 四步过环，遇环、入环、出环、过环
uint8 meetRingFlag = 0;    // 遇环标志位，第一个A字标志点——第一个V字标志点
uint8 enterRingFlag_1 = 0; // 入环标志位，第一个V字标志点——第二个A字标志点
uint8 enterRingFlag_2 = 0; // 上一帧跳变标志位
uint8 leaveRingFlag = 0;   // 出环标志位，第二个A字标志点——第二个V字标志点
uint8 passRingFlag = 1;    // 过环标志位，第二个V字标志点——下一个A字标志点
uint8 ringSide = 0;        // 环岛类型，1表示左，2表示右

// 0表示无环状态
// 1表示车辆遇环状态
// 2表示车辆入环状态
// 3表示车辆环内状态
//  4表示车辆出环状态
uint8 carType = 0;

// 图像处理函数
void imageProcess(uint8 image[MT9V03X_H][MT9V03X_W])
{
    uint16 i, j;

    // 寻找边线位置
    auxiliary_process((uint8_t *)image, MT9V03X_H, MT9V03X_W, threshold, left_line, mid_line, right_line);

    // /*----环岛处理在这里开始----*/

    uint8 jumpFlagL = 0; // 左边线跳变标志位
    uint8 jumpFlagR = 0; // 右边线跳变标志位

    uint16 pointLX = 0; // 左边线跳变点X坐标
    uint16 pointLY = 0; // 左边线跳变点Y坐标
    // 左跳变点的坐标就为 （pointLX, pointLY）

    // 左跳变点的坐标就为 （pointLX, pointLY）

    uint16 pointRX = 0; // 右边线跳变点X坐标
    uint16 pointRY = 0; // 右边线跳变点Y坐标
    // 右跳变点的坐标就为 （pointRX, pointRY）

    uint16 pointX = 0; // 跳变点X坐标
    uint16 pointY = 0; // 跳变点Y坐标
    // 最终确认的跳变点坐标就为 （pointX, pointY）

    uint8 pointSide = 0; // 跳变点方向，1表示左，2表示右
    uint8 pointType = 0; // 跳变点分类，1表示A字跳变点，2表示V字跳变点

    // 寻找跳变点，以及判断跳变点类型
    for (i = MT9V03X_H * 1 / 4; i < MT9V03X_H - 1; i++)
    { // 只判断下2/3部分图像的跳变点
        // 左侧A字跳变点
        if (left_line[i] - left_line[i - 1] > 15)
        {
            jumpFlagL = 1;          // 标记出现左侧跳变点
            pointLX = left_line[i]; // 记录左侧跳变点X坐标
            pointLY = i;            // 记录左侧跳变点Y坐标
            pointSide = 1;          // 跳变方向为左侧
            pointType = 1;          // 跳变类型为A字跳变点
        }
        // 左侧V字跳变点
        else if (left_line[i - 1] - left_line[i] > 30)
        {
            jumpFlagL = 1;              // 标记出现左侧跳变点
            pointLX = left_line[i - 1]; // 记录左侧跳变点X坐标
            pointLY = i - 1;            // 记录左侧跳变点Y坐标
            pointSide = 1;              // 跳变方向为左侧
            pointType = 2;              // 跳变类型为V字跳变点
        }
        // 右侧A字跳变点
        if (right_line[i - 1] - right_line[i] > 15)
        {
            jumpFlagR = 1;           // 标记出现右侧跳变点
            pointRX = right_line[i]; // 记录右侧跳变点X坐标
            pointRY = i;             // 记录右侧跳变点Y坐标
            pointSide = 2;           // 跳变方向为右侧
            pointType = 1;           // 跳变类型为A字跳变点
        }
        // 右侧V字跳变点
        else if (right_line[i] - right_line[i - 1] > 30)
        {
            jumpFlagR = 1;               // 标记出现右侧跳变点
            pointRX = right_line[i - 1]; // 记录右侧跳变点X坐标
            pointRY = i - 1;             // 记录右侧跳变点Y坐标
            pointSide = 2;               // 跳变方向为右侧
            pointType = 2;               // 跳变类型为V字跳变点
        }
    }

    // 只在单侧跳变且跳变点较低时认为是有效跳变点
    uint8 jumpFlag = (jumpFlagL ^ jumpFlagR) && ((pointLY > MT9V03X_H / 3) || (pointRY > MT9V03X_H / 3)); // 防止十字误判

    if ((pointLX == MT9V03X_W / 2) && (pointRX == MT9V03X_W / 2) && (pointLY > MT9V03X_H / 2) && (pointRY > MT9V03X_H / 2))
    {
        jumpFlag = 1; // 入环V形特殊情况
    }
    // 确认最终跳变点坐标
    if (jumpFlagL == 1) // 如果左边线跳变将左边线跳变点作为最终跳变点
    {
        pointX = pointLX;
        pointY = pointLY;
    }
    else if (jumpFlagR == 1) // 如果右边线跳变将右边线跳变点作为最终跳变点
    {
        pointX = pointRX;
        pointY = pointRY;
    }

    // 图像左下角与右下角坐标（用于补线插值的起点）
    // pointLLC*: 左下角 (Left Lower Corner)
    // pointLRC*: 右下角 (Right Lower Corner)
    // 这些点作为补线时的基准点，和检测到的跳变点 (pointX, pointY) 连线，从而向上逐行补齐丢失的边线。
    uint16 pointLLCX = 0;             // 左下角 X 坐标，图像左边界
    uint16 pointLLCY = MT9V03X_H - 1; // 左下角 Y 坐标，图像底部（最后一行）
    uint16 pointLRCX = MT9V03X_W - 1; // 右下角 X 坐标，图像右边界
    uint16 pointLRCY = MT9V03X_H - 1; // 右下角 Y 坐标，图像底部（最后一行）

    float stepLength; // 横坐标插值步长

    // 车辆状态判断
    if (jumpFlag)
    {                       // 若有有效跳变点
        if (pointType == 1) // 若为A字跳变点，说明出环或遇环
        {
            if (carType == 0) // 车辆的前一个状态为无环状态
            {
                carType = 1; // 遇环状态
            }
            else if (carType == 3) // 车辆的前一个状态为环内状态
            {
                carType = 4; // 出环状态
            }
        }
        else if (pointType == 2) // 若为V字跳变点
        {
            if (carType == 1) // 车辆的前一个状态为遇环状态
            {
                carType = 2; // 入环状态
            }
            else if (carType == 2) // 车辆的前一个状态为入环状态
            {
                carType = 3; // 环内状态
            }
            else if (carType == 4) // 车辆的前一个状态为出环状态
            {
                carType = 0; // 无环状态
            }
        }
    }

    // if (jumpFlag)
    // { // 若有有效跳变点
    //     if (pointType == 1)
    //     { // 若为A字跳变点，说明出环或遇环
    //         if (passRingFlag == 1)
    //         { // 遇环//遇环仅判断方向用
    //             meetRingFlag = 1;
    //             passRingFlag = 0;
    //         }
    //         else if (enterRingFlag_1 == 1)
    //         { // 出环
    //             leaveRingFlag = 1;
    //             enterRingFlag_1 = 0;
    //         }
    //     }
    //     else if (pointType == 2)
    //     { // 若为V字跳变点，说明入环或过环
    //         if (meetRingFlag == 1)
    //         { // 入环
    //             enterRingFlag_1 = 1;
    //             meetRingFlag = 0;
    //         }
    //         else if (leaveRingFlag == 1)
    //         { // 过环
    //             passRingFlag = 1;
    //             leaveRingFlag = 0;
    //         }
    //     }
    // }

    switch (carType)
    {
    case 0:                // 为默认无环状态
        if (jumpFlag == 1) // 出现跳变点
        {
            if (pointType == 2) // V字跳变点
            {
                if (ringSide == 1) // 左环
                {
                    // 补左边线
                    stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
                    for (i = 0; i < pointLLCY - pointY; i++)
                    {
                        left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
                        image[pointLLCY - i][left_line[pointLLCY - i]] = 0; // 显示补线
                    }
                }
                else if (ringSide == 2) // 右环
                {
                    // 补右边线
                    enterRingFlag_1 = 0;
                    stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
                    for (i = 0; i < pointLRCY - pointY; i++)
                    {
                        right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
                        image[pointLRCY - i][right_line[pointLRCY - i]] = 0; // 显示补线
                    }
                }
            }
        }
        break;
    case 1:                // 遇到环
        if (jumpFlag == 1) // 出现跳变点
        {
            ringSide = pointSide; // 环岛方向与跳变点方向一致
        }
        break;
    case 2:                // 入环状态
        if (jumpFlag == 1) // 出现跳变点
        {
            if (pointType == 2) // V字跳变点
            {
                if (ringSide == 1) // 左环
                {
                    // 补左边线
                    stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
                    for (i = 0; i < pointLLCY - pointY; i++)
                    {
                        left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
                        image[pointLLCY - i][left_line[pointLLCY - i]] = 0; // 显示补线
                    }
                }
                else if (ringSide == 2) // 右环
                {
                    // 补右边线
                    stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
                    for (i = 0; i < pointLRCY - pointY; i++)
                    {
                        right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
                        image[pointLRCY - i][right_line[pointLRCY - i]] = 0; // 显示补线
                    }
                }
            }
        }
        break;
    case 3:                // 环内状态
        if (jumpFlag == 1) // 出现跳变点
        {
            if (pointType == 2) // V字跳变点
            {
                if (ringSide == 1) // 左环
                {
                    // 补右边线
                    stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
                    for (i = 0; i < pointLRCY - pointY; i++)
                    {
                        right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
                        image[pointLRCY - i][right_line[pointLRCY - i]] = 0; // 显示补线
                    }
                }
                else if (ringSide == 2) // 右环
                {
                    // 补左边线
                    stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
                    for (i = 0; i < pointLLCY - pointY; i++)
                    {
                        left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
                        image[pointLLCY - i][left_line[pointLLCY - i]] = 0; // 显示补线
                    }
                }
            }
        }
        break;
    case 4:                // 出环状态
        if (jumpFlag == 1) // 出现跳变点
        {
            if (pointType == 1) // A字跳变点
            {
                if (ringSide == 1) // 左环
                {
                    // 从右下角向上补右边线
                    stepLength = (float)(pointLRCX - MT9V03X_W / 2) / (float)(pointLRCY);
                    for (i = 0; i < MT9V03X_H - 1; i++)
                    {
                        right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
                        image[pointLRCY - i][right_line[pointLRCY - i]] = 0; // 显示补线
                    }
                }
                else if (ringSide == 2) // 右环
                {
                    // 从坐下角向上补左边线
                    stepLength = (float)(MT9V03X_W / 2 - pointLLCX) / (float)(pointLLCY);
                    for (i = 0; i < MT9V03X_H - 1; i++)
                    {
                        left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
                        image[pointLLCY - i][left_line[pointLLCY - i]] = 0; // 显示补线
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    // if (meetRingFlag == 1) // 遇到环
    // {
    //     if (jumpFlag == 1) // 出现跳变点
    //     {
    //         ringSide = pointSide; // 环岛方向与跳变点方向一致
    //     }
    // }
    // else if (enterRingFlag_1 == 1)
    // {
    //     if (jumpFlag == 1) // 有效跳变点
    //     {
    //         // 入环补线
    //         // 入环标志点（pointAX，pointAY），图像左下角（pointLLCX，pointLLCY），图像右下角（pointLRCX，pointLRCY）

    //         if (ringSide == 1)
    //         { // 入环标志点在左边，从右下角点到入环标志点连线，补右边线
    //             enterRingFlag_2 = 1;
    //             // enterRingFlag_1 = 0;
    //             // stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
    //             // for (i = 0; i < pointLRCY - pointY; i++)
    //             // {
    //             //     right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
    //             //     image[pointLRCY - i][right_line[pointLRCY - i]] = 0; // 显示补线
    //             // }

    //             stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
    //             for (i = 0; i < pointLLCY - pointY; i++)
    //             {
    //                 left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
    //                 image[pointLLCY - i][left_line[pointLLCY - i]] = 0; // 显示补线
    //             }
    //         }
    //         else if (ringSide == 2)
    //         { // 入环标志点在右边，从左下角点到入环标志点连线，补左边线
    //             enterRingFlag_2 = 1;
    //             // enterRingFlag_1 = 0;
    //             // stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
    //             // for (i = 0; i < pointLLCY - pointY; i++)
    //             // {
    //             //     left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
    //             //     image[pointLLCY - i][left_line[pointLLCY - i]] = 0; // 显示补线
    //             // }

    //             stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
    //             for (i = 0; i < pointLRCY - pointY; i++)
    //             {
    //                 right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
    //                 image[pointLRCY - i][right_line[pointLRCY - i]] = 0; // 显示补线
    //             }
    //         }
    //     }
    // }
    // else if (enterRingFlag_2 == 1)
    // {
    //     if (jumpFlag == 1)
    //     {
    //         if (ringSide == 1)
    //         { // 若左圆环
    //             stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
    //             for (i = 0; i < pointLRCY - pointY; i++)
    //             {
    //                 right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
    //                 image[pointLRCY - i][right_line[pointLRCY - i]] = 0; // 显示补线
    //             }
    //         }
    //         else if (ringSide == 2)
    //         { // 若右圆环
    //             stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
    //             for (i = 0; i < pointLLCY - pointY; i++)
    //             {
    //                 left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
    //                 image[pointLLCY - i][left_line[pointLLCY - i]] = 0; // 显示补线
    //             }
    //         }
    //     }
    // }

    // else if (leaveRingFlag == 1)
    // {
    //     if (ringSide == 1)
    //     { // 若左圆环
    //         for (i = MT9V03X_H * 3 / 4; i < MT9V03X_H - 1; i++)
    //         { // 判断右边线下部分
    //             if (left_line[i] == MT9V03X_W - 1)
    //             {           // 若丢线
    //                 return; // 不改变偏差
    //             }
    //         }
    //     }
    //     if (ringSide == 2)
    //     {
    //         for (i = MT9V03X_H * 3 / 4; i < MT9V03X_H - 1; i++)
    //         {
    //             if (left_line[i] == 0)
    //             {
    //                 return;
    //             }
    //         }
    //     }
    // }

    // else if (passRingFlag == 1)
    // {
    //     if (jumpFlag == 1)
    //     {
    //         if (ringSide == 1)
    //         {
    //             stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
    //             for (i = 0; i < pointLLCY - pointY; i++)
    //             {
    //                 left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
    //                 image[pointLLCY - i][left_line[pointLLCY - i]] = 0;
    //             }
    //         }
    //         else if (ringSide == 2)
    //         {
    //             // When ringSide==2 (right side), we should patch/add to the right_line
    //             // (original code incorrectly wrote left_line and used right_line for image index)
    //             stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
    //             for (i = 0; i < pointLRCY - pointY; i++)
    //             {
    //                 right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
    //                 image[pointLRCY - i][right_line[pointLRCY - i]] = 0;
    //             }
    //         }
    //     }
    // }

    // /*----环岛处理在这里结束----*/

    for (i = 0; i < MT9V03X_H; i++)
    { // 重新计算中线
        mid_line[i] = (left_line[i] + right_line[i]) / 2;
        int m = mid_line[i];
        // Boundary-safe drawing of the midline (avoid out-of-bounds access)
        if (m > 0 && m < MT9V03X_W - 1)
        {
            image[i][m] = image[i][m - 1] = image[i][m + 1] = 0; // 粗线
        }
        else if (m == 0)
        {
            image[i][m] = image[i][m + 1] = 0;
        }
        else if (m == MT9V03X_W - 1)
        {
            image[i][m] = image[i][m - 1] = 0;
        }
    }
    // lcd_showint8(0, 5, jumpFlag);
    // lcd_showint16(0, 6, meetRingFlag * 1000 + enterRingFlag * 100 + leaveRingFlag * 10 + passRingFlag);

    // 取图像下1/4处的中线位置计算偏差
    // 取1/2处和3/4的图像计算偏差
    float tempOffset = 0.0f;
    for (i = MT9V03X_H / 4; i < MT9V03X_H * 3 / 4; i++)
    {
        tempOffset += (MT9V03X_W / 2 - mid_line[i]);
    }
    tempOffset /= (MT9V03X_H / 2);

    offset = tempOffset;
}