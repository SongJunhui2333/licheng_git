#include "imageProcess.h"

// void imageProcess(uint8 image[MT9V03X_H][MT9V03X_W])
// {
//     uint16 i, j;
//     uint16 edgeL[MT9V03X_H];
//     uint16 edgeR[MT9V03X_H];
//     uint16 midline[MT9V03X_H];

//     edgeL[MT9V03X_H - 1] = 0;
//     edgeR[MT9V03X_H - 1] = MT9V03X_W - 1;
//     midline[MT9V03X_H - 1] = MT9V03X_W / 2;

//     uint8 jumpFlagL = 0;
//     uint8 jumpFlagR = 0;

//     uint16 pointX = 0;   // 跳变点X坐标
//     uint16 pointY = 0;   // 跳变点Y坐标
//     uint8 pointSide = 0; // 跳变点方向，1表示左，2表示右
//     uint8 pointType = 0; // 跳变点分类，1表示A字跳变点，2表示V字跳变点

//     for (i = MT9V03X_H - 2;; i--)
//     {
//         edgeL[i] = 0;
//         edgeR[i] = MT9V03X_W - 1;

//         for (j = midline[i + 1]; j > 0; j--)
//         {
//             if (image[i][j] == 0)
//             {
//                 edgeL[i] = j;
//                 break;
//             }
//         }

//         for (j = midline[i + 1]; j < MT9V03X_W - 1; j++)
//         {
//             if (image[i][j] == 0)
//             {
//                 edgeR[i] = j;
//                 break;
//             }
//         }

//         // 环岛处理开始（见到巨人的肩膀魁梧却布满荆棘，我决定自己动手丰衣足食）
//         // 环岛处理所有数都是非负数
//         // bingo：和十字区分：扫描到一侧的跳变点后（下半部分），对另一侧的边线进行判断（下3/4部分）

//         // 寻找跳变点

//         if (i > MT9V03X_H * 1 / 3)
//         { // 只判断下2/3部分图像的跳变点（赛道突然变窄）
//             // 左侧A字跳变点
//             if (edgeL[i + 1] - edgeL[i] > 20)
//             {
//                 jumpFlagL = 1;
//                 pointX = edgeL[i + 1];
//                 pointY = i + 1;
//                 pointSide = 1;
//                 pointType = 1;
//             }
//             // 左侧V字跳变点
//             else if (edgeL[i] - edgeL[i + 1] > 20)
//             {
//                 jumpFlagL = 1;
//                 pointX = edgeL[i];
//                 pointY = i;
//                 pointSide = 1;
//                 pointType = 2;
//             }
//             // 右侧A字跳变点
//             if (edgeR[i] - edgeR[i + 1] > 20)
//             {
//                 jumpFlagR = 1;
//                 pointX = edgeR[i + 1];
//                 pointY = i + 1;
//                 pointSide = 2;
//                 pointType = 1;
//             }
//             // 右侧V字跳变点
//             else if (edgeR[i + 1] - edgeR[i] > 20)
//             {
//                 jumpFlagR = 1;
//                 pointX = edgeR[i];
//                 pointY = i;
//                 pointSide = 2;
//                 pointType = 2;
//             }
//         }

//         midline[i] = (edgeL[i] + edgeR[i]) / 2; // 考虑环岛补线后不能边判断边线边计算中线，这个中线只用来下一步寻找边界

//         if (i == 0)
//         {
//             break;
//         }
//     }

//     // 四步过环，遇环、入环、出环、过环
//     static uint8 meetRingFlag = 0;  // 遇环标志位
//     static uint8 enterRingFlag = 0; // 入环标志位
//     static uint8 leaveRingFlag = 0; // 出环标志位
//     static uint8 ringSide = 0;      // 环岛类型，1表示左，2表示右

//     uint16 pointLLCX = 0;
//     uint16 pointLLCY = MT9V03X_H - 1;
//     uint16 pointLRCX = MT9V03X_W - 1;
//     uint16 pointLRCY = MT9V03X_H - 1;

//     uint16 pointTMX = MT9V03X_W / 2;
//     uint16 pointTMY = 0;

//     float stepLength; // 横坐标插值步长

//     if (jumpFlagL ^ jumpFlagR)
//     { // 若有一侧出现跳变点
//         if (pointType == 1)
//         { // 若为A字跳变点，说明出环或遇环
//             if (enterRingFlag == 1)
//             { // 出环
//                 leaveRingFlag = 1;
//                 if (ringSide == 1)
//                 { // 环岛在左边，从顶部中点到出环标志点连线，补右边线
//                     stepLength = (float)(pointTMX - pointX) / (float)(pointTMY - pointY);
//                     for (i = 0; i < pointTMY - pointY; i++)
//                     {
//                         edgeR[pointTMY - i] = pointTMX - (int)(i * stepLength);
//                         image[pointTMY - i][edgeR[pointTMY - i]] = image[pointTMY - i][edgeR[pointTMY - i] - 1] = image[pointTMY - i][edgeR[pointTMY - i] + 1] = 0; // 粗线
//                     }
//                 }
//                 else if (ringSide == 2)
//                 { // 环岛在右边，从顶部中点到出环标志点连线，补左边线
//                     stepLength = (float)(pointX - pointTMX) / (float)(pointTMY - pointY);
//                     for (i = 0; i < pointTMY - pointY; i++)
//                     {
//                         edgeL[pointTMY - i] = pointTMX + (int)(i * stepLength);
//                         image[pointTMY - i][edgeL[pointTMY - i]] = image[pointTMY - i][edgeL[pointTMY - i] - 1] = image[pointTMY - i][edgeL[pointTMY - i] + 1] = 0; // 粗线
//                     }
//                 }
//             }
//             else if (meetRingFlag == 0)
//             { // 遇环
//                 meetRingFlag = 1;
//                 ringSide = pointSide;
//             }
//             // 目前A字跳变点可以自动处理，无需补线
//         }
//         else if (pointType == 2)
//         { // 若为V字跳变点，说明入环或过环
//             if (leaveRingFlag == 1)
//             { // 过环//与入环相反//须先判断过环
//                 meetRingFlag = 0;
//                 enterRingFlag = 0;
//                 leaveRingFlag = 0;
//                 // 过环补线
//                 if (ringSide == 2)
//                 {
//                     stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
//                     for (i = 0; i < pointLRCY - pointY; i++)
//                     {
//                         edgeR[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
//                         image[pointLRCY - i][edgeR[pointLRCY - i]] = image[pointLRCY - i][edgeR[pointLRCY - i] - 1] = image[pointLRCY - i][edgeR[pointLRCY - i] + 1] = 0; // 粗线
//                     }
//                 }
//                 else if (ringSide == 1)
//                 {
//                     stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
//                     for (i = 0; i < pointLLCY - pointY; i++)
//                     {
//                         edgeL[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
//                         image[pointLLCY - i][edgeL[pointLLCY - i]] = image[pointLLCY - i][edgeL[pointLLCY - i] - 1] = image[pointLLCY - i][edgeL[pointLLCY - i] + 1] = 0; // 粗线
//                     }
//                 }
//             }
//             else if (meetRingFlag == 1)
//             { // 入环
//                 enterRingFlag = 1;
//                 // 入环补线
//                 // 入环标志点（pointAX，pointAY），图像左下角（pointLLCX，pointLLCY），图像右下角（pointLRCX，pointLRCY）
//                 if (ringSide == 1)
//                 { // 环岛在左边，从右下角点到入环标志点连线，补右边线
//                     stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
//                     for (i = 0; i < pointLRCY - pointY; i++)
//                     {
//                         edgeR[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
//                         image[pointLRCY - i][edgeR[pointLRCY - i]] = image[pointLRCY - i][edgeR[pointLRCY - i] - 1] = image[pointLRCY - i][edgeR[pointLRCY - i] + 1] = 0; // 粗线
//                     }
//                 }
//                 else if (ringSide == 2)
//                 { // 环岛在右边，从左下角点到入环标志点连线，补左边线
//                     stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
//                     for (i = 0; i < pointLLCY - pointY; i++)
//                     {
//                         edgeL[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
//                         image[pointLLCY - i][edgeL[pointLLCY - i]] = image[pointLLCY - i][edgeL[pointLLCY - i] - 1] = image[pointLLCY - i][edgeL[pointLLCY - i] + 1] = 0; // 粗线
//                     }
//                 }
//             }
//         }
//     }

//     for (i = 0; i < MT9V03X_H; i++)
//     { // 重新计算中线
//         midline[i] = (edgeL[i] + edgeR[i]) / 2;
//         image[i][midline[i]] = image[i][midline[i] - 1] = image[i][midline[i] + 1] = 0; // 粗线
//     }

//     // lcd_showint8(0, 4, jumpFlagL ^ jumpFlagR);
//     // lcd_showint8(0, 5, ringSide);
//     // lcd_showint8(0, 6, meetRingFlag * 100 + enterRingFlag * 10 + leaveRingFlag);

//     offset = MT9V03X_W / 2 - midline[MT9V03X_H - MT9V03X_W / 4];
// }

void imageProcess(uint8 image[MT9V03X_H][MT9V03X_W])
{
    uint16 i, j;
    uint16 edgeL[MT9V03X_H];
    uint16 edgeR[MT9V03X_H];
    uint16 midline[MT9V03X_H];

    edgeL[MT9V03X_H - 1] = 0;
    edgeR[MT9V03X_H - 1] = MT9V03X_W - 1;
    midline[MT9V03X_H - 1] = MT9V03X_W / 2;

    for (i = MT9V03X_H - 2;; i--)
    {
        edgeL[i] = 0;
        edgeR[i] = MT9V03X_W - 1;

        for (j = midline[i + 1]; j > 0; j--)
        {
            if (image[i][j] == 0)
            {
                edgeL[i] = j;
                break;
            }
        }

        for (j = midline[i + 1]; j < MT9V03X_W - 1; j++)
        {
            if (image[i][j] == 0)
            {
                edgeR[i] = j;
                break;
            }
        }

        midline[i] = (edgeL[i] + edgeR[i]) / 2; // 考虑环岛补线后不能边判断边线边计算中线，这个中线只用来下一步寻找边界

        if (i == 0)
        {
            break;
        }
    }

    /*----环岛处理在这里开始----*/

    uint8 jumpFlagL = 0;
    uint8 jumpFlagR = 0;

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
        if (edgeL[i] - edgeL[i - 1] > 20)
        {
            jumpFlagL = 1;
            pointLX = edgeL[i];
            pointLY = i;
            pointSide = 1;
            pointType = 1;
        }
        // 左侧V字跳变点
        else if (edgeL[i - 1] - edgeL[i] > 20)
        {
            jumpFlagL = 1;
            pointLX = edgeL[i - 1];
            pointLY = i - 1;
            pointSide = 1;
            pointType = 2;
        }
        // 右侧A字跳变点
        if (edgeR[i - 1] - edgeR[i] > 20)
        {
            jumpFlagR = 1;
            pointRX = edgeR[i];
            pointRY = i;
            pointSide = 2;
            pointType = 1;
        }
        // 右侧V字跳变点
        else if (edgeR[i] - edgeR[i - 1] > 20)
        {
            jumpFlagR = 1;
            pointRX = edgeR[i - 1];
            pointRY = i - 1;
            pointSide = 2;
            pointType = 2;
        }
    }

    tft180_show_int(0, 130, pointSide, 1);
    tft180_show_int(50, 130, pointType, 1);

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

    uint16 pointLLCX = 0;
    uint16 pointLLCY = MT9V03X_H - 1;
    uint16 pointLRCX = MT9V03X_W - 1;
    uint16 pointLRCY = MT9V03X_H - 1;

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

    if (meetRingFlag == 1)
    {
        if (jumpFlag == 1)
        {
            ringSide = pointSide;
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
                    edgeR[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
                    image[pointLRCY - i][edgeR[pointLRCY - i]] = 0;
                }
            }
            else if (ringSide == 2)
            { // 入环标志点在右边，从左下角点到入环标志点连线，补左边线
                stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
                for (i = 0; i < pointLLCY - pointY; i++)
                {
                    edgeL[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
                    image[pointLLCY - i][edgeL[pointLLCY - i]] = 0;
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
                if (edgeR[i] == MT9V03X_W - 1)
                {           // 若丢线
                    return; // 不改变偏差
                }
            }
        }
        if (ringSide == 2)
        {
            for (i = MT9V03X_H * 3 / 4; i < MT9V03X_H - 1; i++)
            {
                if (edgeL[i] == 0)
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
                    edgeL[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
                    image[pointLLCY - i][edgeL[pointLLCY - i]] = 0;
                }
            }
            else if (ringSide == 2)
            {
                stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
                for (i = 0; i < pointLRCY - pointY; i++)
                {
                    edgeR[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
                    image[pointLRCY - i][edgeR[pointLRCY - i]] = 0;
                }
            }
        }
    }

    /*----环岛处理在这里结束----*/

    for (i = 0; i < MT9V03X_H; i++)
    { // 重新计算中线
        midline[i] = (edgeL[i] + edgeR[i]) / 2;
        image[i][midline[i]] = image[i][midline[i] - 1] = image[i][midline[i] + 1] = 0; // 粗线
    }
    // lcd_showint8(0, 5, jumpFlag);
    // lcd_showint16(0, 6, meetRingFlag * 1000 + enterRingFlag * 100 + leaveRingFlag * 10 + passRingFlag);

    offset = MT9V03X_W / 2 - midline[88];
}