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
    // 左跳变点的坐标就为 （pointLX, pointLY）

    uint16 pointRX = 0; // 右边线跳变点X坐标
    uint16 pointRY = 0; // 右边线跳变点Y坐标
    // 右跳变点的坐标就为 （pointRX, pointRY）

    uint16 pointX = 0; // 跳变点X坐标
    uint16 pointY = 0; // 跳变点Y坐标
    // 最终确认的跳变点坐标就为 （pointX, pointY）

    uint8 pointSide = 0; // 跳变点方向，1表示左，2表示右
    uint8 pointType = 0; // 跳变点分类，1表示A字跳变点，2表示V字跳变点

    // Improve robustness: use adaptive threshold (based on lane width), require debounce
    // (consecutive rows), add safety bounds and make detection exclusive per-row
    // Configured in imageProcess.h using JP_DEFAULT_THRESHOLD, JP_DEBOUNCE_ROWS and JP_LANE_DIV_FACTOR
    for (i = MT9V03X_H * 1 / 3 + 1; i < MT9V03X_H - 1; i++)
    { // 只判断下2/3部分图像的跳变点
        // Adaptive threshold: don't use a fixed 20 px if the lane is wide/narrow
        int laneWidth = (int)right_line[i - 1] - (int)left_line[i - 1];
        int adaptive_th = JP_DEFAULT_THRESHOLD;
        if (laneWidth > 0)
        {
            int fromWidth = laneWidth / JP_LANE_DIV_FACTOR; // adaptive fraction of lane width
            if (fromWidth > adaptive_th)
                adaptive_th = fromWidth;
        }

        // Left side robust detection
        if ((left_line[i] > 0) && (left_line[i - 1] > 0) && (left_line[i] < MT9V03X_W) && (left_line[i - 1] < MT9V03X_W))
        {
            int diff = (int)left_line[i] - (int)left_line[i - 1];
            if (abs(diff) > adaptive_th)
            {
                // debounce: require same-sign large diffs for JUMP_DEBOUNCE consecutive rows
                int k, cnt = 1;
                for (k = 1; k < JP_DEBOUNCE_ROWS; k++)
                {
                    if ((i + k) >= MT9V03X_H)
                        break;
                    int ndiff = (int)left_line[i + k] - (int)left_line[i + k - 1];
                    if ((diff > 0 && ndiff > adaptive_th / 2) || (diff < 0 && ndiff < -adaptive_th / 2))
                    {
                        cnt++;
                    }
                    else
                        break;
                }

                if (cnt >= JP_DEBOUNCE_ROWS)
                {
                    jumpFlagL = 1; // mark left side jump
                    if (diff > 0)
                    { // A-shape (sudden rightward jump)
                        pointLX = left_line[i];
                        pointLY = i;
                        pointType = 1;
                    }
                    else
                    { // V-shape (sudden leftward fall)
                        pointLX = left_line[i - 1];
                        pointLY = i - 1;
                        pointType = 2;
                    }
                    pointSide = 1;
                }
            }
        }

        // Right side robust detection -- only check if left side wasn't detected on this row
        if (!jumpFlagL && (right_line[i] >= 0) && (right_line[i - 1] >= 0) && (right_line[i] < MT9V03X_W) && (right_line[i - 1] < MT9V03X_W))
        {
            int diffR = (int)right_line[i - 1] - (int)right_line[i];
            if (abs(diffR) > adaptive_th)
            {
                int k, cntR = 1;
                for (k = 1; k < JP_DEBOUNCE_ROWS; k++)
                {
                    if ((i + k) >= MT9V03X_H)
                        break;
                    int ndiff = (int)right_line[i + k - 1] - (int)right_line[i + k];
                    if ((diffR > 0 && ndiff > adaptive_th / 2) || (diffR < 0 && ndiff < -adaptive_th / 2))
                        cntR++;
                    else
                        break;
                }

                if (cntR >= JP_DEBOUNCE_ROWS)
                {
                    jumpFlagR = 1;
                    if (diffR > 0)
                    { // A-shape (right edge moved left)
                        pointRX = right_line[i];
                        pointRY = i;
                        pointType = 1;
                    }
                    else
                    { // V-shape
                        pointRX = right_line[i - 1];
                        pointRY = i - 1;
                        pointType = 2;
                    }
                    pointSide = 2;
                }
            }
        }
    }

    // tft180_show_int(0, 130, pointSide, 1);
    // tft180_show_int(50, 130, pointType, 1);

    // 只在单侧跳变且跳变点位于图像下2/3部分时认定为有效跳变点
    uint8 jumpFlag = (jumpFlagL ^ jumpFlagR) && ((pointLY > MT9V03X_H / 3) || (pointRY > MT9V03X_H / 3)); // 防止十字误判

    if ((pointLX == MT9V03X_W / 2) && (pointRX == MT9V03X_W / 2) && (pointLY > MT9V03X_H / 2) && (pointRY > MT9V03X_H / 2))
    {
        jumpFlag = 1; // 入环V形特殊情况
    }

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
    tft180_show_int(0, 115, ringSide, 1);

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
                // When ringSide==2 (right side), we should patch/add to the right_line
                // (original code incorrectly wrote left_line and used right_line for image index)
                stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
                for (i = 0; i < pointLRCY - pointY; i++)
                {
                    right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
                    image[pointLRCY - i][right_line[pointLRCY - i]] = 0;
                }
            }
        }
    }

    /*----环岛处理在这里结束----*/

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
    offset = MT9V03X_W / 2 - mid_line[MT9V03X_H - MT9V03X_H / 4];
}