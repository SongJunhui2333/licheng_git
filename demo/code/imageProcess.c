#include "imageProcess.h"

// 四步过环，遇环、入环、出环、过环
uint8 meetRingFlag = 0;    // 遇环标志位，第一个A字标志点——第一个V字标志点
uint8 enterRingFlag_1 = 0; // 入环标志位，第一个V字标志点——第二个A字标志点
uint8 enterRingFlag_2 = 0; // 上一帧跳变标志位
uint8 leaveRingFlag = 0;   // 出环标志位，第二个A字标志点——第二个V字标志点
uint8 passRingFlag = 1;    // 过环标志位，第二个V字标志点——下一个A字标志点
uint8 ringSide = 0;        // 环岛类型，1表示左，2表示右
uint16 jumpNum = 0;        // 车辆遇到跳变点的次数
uint16 jumpNow = 0;        // 第一个A点jumpNum记录

uint16 whiteNum_show = 0; // 记录图像一条内白色像素点数量

// 0表示无环状态
// 1表示车辆遇环状态
// 2表示车辆入环状态
// 3表示车辆环内状态
//  4表示车辆出环状态
uint8 carType = 0;

uint8 Vpoint_t = 20;

// 辅助寻点函数

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右下角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Right_Down_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Down_Point(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int right_down_line = 0;

    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // 下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (right_down_line == 0 &&                        // 只找第一个符合条件的点
            abs(right_line[i] - right_line[i + 1]) <= 5 && // 角点的阈值可以更改 当前角点阈值为5
            abs(right_line[i + 1] - right_line[i + 2]) <= 5 &&
            abs(right_line[i + 2] - right_line[i + 3]) <= 5 &&
            (right_line[i] - right_line[i - 2]) <= -5 &&
            (right_line[i] - right_line[i - 3]) <= -10 &&
            (right_line[i] - right_line[i - 4]) <= -10)
        {
            right_down_line = i; // 获取行数即可
            break;
        }
    }
    return right_down_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左下角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Left_Down_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Down_Point(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int left_down_line = 0;

    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // 下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (left_down_line == 0 &&                       // 只找第一个符合条件的点
            abs(left_line[i] - left_line[i + 1]) <= 5 && // 角点的阈值可以更改 当前角点阈值为5
            abs(left_line[i + 1] - left_line[i + 2]) <= 5 &&
            abs(left_line[i + 2] - left_line[i + 3]) <= 5 &&
            (left_line[i] - left_line[i - 2]) >= 5 &&
            (left_line[i] - left_line[i - 3]) >= 10 &&
            (left_line[i] - left_line[i - 4]) >= 10)
        {
            left_down_line = i; // 获取行数即可
            break;
        }
    }
    return left_down_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右单调性突变检测
  @param     起始点，终止行
  @return    点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      前5后5它最大（最小），那他就是角点
-------------------------------------------------------------------------------------------------------------------*/
uint16 Monotonicity_Change_Right(int start, int end) // 单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    uint16 monotonicity_change_line = 0;

    if (start >= MT9V03X_H - 1 - 5) // 数组越界保护
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (start <= end)
        return monotonicity_change_line;
    for (i = start; i >= end; i--) // 会读取前5后5数据，所以前面对输入范围有要求
    {
        if (right_line[i] == right_line[i + 5] && right_line[i] == right_line[i - 5] &&
            right_line[i] == right_line[i + 4] && right_line[i] == right_line[i - 4] &&
            right_line[i] == right_line[i + 3] && right_line[i] == right_line[i - 3] &&
            right_line[i] == right_line[i + 2] && right_line[i] == right_line[i - 2] &&
            right_line[i] == right_line[i + 1] && right_line[i] == right_line[i - 1])
        { // 一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if (right_line[i] < right_line[i + 5] && right_line[i] < right_line[i - 5] &&
                 right_line[i] < right_line[i + 4] && right_line[i] < right_line[i - 4] &&
                 right_line[i] <= right_line[i + 3] && right_line[i] <= right_line[i - 3] &&
                 right_line[i] <= right_line[i + 2] && right_line[i] <= right_line[i - 2] &&
                 right_line[i] <= right_line[i + 1] && right_line[i] <= right_line[i - 1])
        { // 就很暴力，这个数据是在前5，后5中最大的，那就是单调突变点
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左单调性突变检测
  @param     起始点，终止行
  @return    点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      前5后5它最大（最小），那他就是角点
-------------------------------------------------------------------------------------------------------------------*/
uint16 Monotonicity_Change_Left(int start, int end) // 单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    uint16 monotonicity_change_line = 0;

    if (start >= MT9V03X_H - 1 - 5) // 数组越界保护
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (start <= end)
        return monotonicity_change_line;
    for (i = start; i >= end; i--) // 会读取前5后5数据，所以前面对输入范围有要求
    {
        if (left_line[i] == left_line[i + 5] && left_line[i] == left_line[i - 5] &&
            left_line[i] == left_line[i + 4] && left_line[i] == left_line[i - 4] &&
            left_line[i] == left_line[i + 3] && left_line[i] == left_line[i - 3] &&
            left_line[i] == left_line[i + 2] && left_line[i] == left_line[i - 2] &&
            left_line[i] == left_line[i + 1] && left_line[i] == left_line[i - 1])
        { // 一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if (left_line[i] > left_line[i + 5] && left_line[i] > left_line[i - 5] &&
                 left_line[i] > left_line[i + 4] && left_line[i] > left_line[i - 4] &&
                 left_line[i] >= left_line[i + 3] && left_line[i] >= left_line[i - 3] &&
                 left_line[i] >= left_line[i + 2] && left_line[i] >= left_line[i - 2] &&
                 left_line[i] >= left_line[i + 1] && left_line[i] >= left_line[i - 1])
        { // 就很暴力，这个数据是在前5，后5中最大的，那就是单调突变点
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}

static uint8 prevJumpFlag = 0; // 上一帧边线跳变标志位

// 图像处理函数
void imageProcess(uint8 image[MT9V03X_H][MT9V03X_W])
{
    uint16 i, j;

    // 寻找边线位置
    auxiliary_process((uint8_t *)image, MT9V03X_H, MT9V03X_W, threshold, left_line, mid_line, right_line);

    // /*----环岛处理在这里开始----*/

    uint8 jumpFlagL = 0;   // 左边线跳变标志位
    uint8 jumpFlagR = 0;   // 右边线跳变标志位
    uint8 changeFlagR = 0; // 右边线单调性突变标志位
    uint8 changeFlagL = 0; // 左边线单调性突变标志位

    uint16 pointLX = 0; // 左边线跳变点X坐标
    uint16 pointLY = 0; // 左边线跳变点Y坐标
    // 左跳变点的坐标就为 （pointLX, pointLY）

    uint16 changeLX = 0; // 左边线单调性突变点X坐标
    uint16 changeLY = 0; // 左边线单调性突变点Y坐标

    uint16 pointRX = 0; // 右边线跳变点X坐标
    uint16 pointRY = 0; // 右边线跳变点Y坐标
    // 右跳变点的坐标就为 （pointRX, pointRY）

    uint16 changeRX = 0; // 右边线单调性突变点X坐标
    uint16 changeRY = 0; // 右边线单调性突变点Y坐标

    uint16 pointX = 0; // 跳变点X坐标
    uint16 pointY = 0; // 跳变点Y坐标
    // 最终确认的跳变点坐标就为 （pointX, pointY）

    uint8 pointSide = 0; // 跳变点方向，1表示左，2表示右
    uint8 pointType = 0; // 跳变点分类，1表示A字跳变点，2表示V字跳变点

    int whiteNum = 0; // 记录图像一条内白色像素点数量

    // 寻找跳变点，以及判断跳变点类型
    for (i = MT9V03X_H * 1 / 4; i < MT9V03X_H - 1; i++)
    { // 只判断下1/4部分图像的跳变点
        // 左侧A字跳变点
        if (left_line[i] - left_line[i - 1] > 30)
        {
            jumpFlagL = 1;          // 标记出现左侧跳变点
            pointLX = left_line[i]; // 记录左侧跳变点X坐标
            pointLY = i;            // 记录左侧跳变点Y坐标
            pointSide = 1;          // 跳变方向为左侧
            pointType = 1;          // 跳变类型为A字跳变点
        }
        // 左侧V字跳变点
        else if (left_line[i - 1] - left_line[i] > Vpoint_t)
        {
            jumpFlagL = 1;              // 标记出现左侧跳变点
            pointLX = left_line[i - 1]; // 记录左侧跳变点X坐标
            pointLY = i - 1;            // 记录左侧跳变点Y坐标
            pointSide = 1;              // 跳变方向为左侧
            pointType = 2;              // 跳变类型为V字跳变点
        }
        // 右侧A字跳变点
        if (right_line[i - 1] - right_line[i] > 30)
        {
            jumpFlagR = 1;           // 标记出现右侧跳变点
            pointRX = right_line[i]; // 记录右侧跳变点X坐标
            pointRY = i;             // 记录右侧跳变点Y坐标
            pointSide = 2;           // 跳变方向为右侧
            pointType = 1;           // 跳变类型为A字跳变点
        }
        // 右侧V字跳变点
        else if (right_line[i] - right_line[i - 1] > Vpoint_t)
        {
            jumpFlagR = 1;               // 标记出现右侧跳变点
            pointRX = right_line[i - 1]; // 记录右侧跳变点X坐标
            pointRY = i - 1;             // 记录右侧跳变点Y坐标
            pointSide = 2;               // 跳变方向为右侧
            pointType = 2;               // 跳变类型为V字跳变点
        }
    }

    // 只在单侧跳变且跳变点较低时认为是有效跳变点
    uint8 jumpFlag = 0; // 防止十字误判

    if ((jumpFlagL ^ jumpFlagR) && ((pointLY > MT9V03X_H / 3) || (pointRY > MT9V03X_H / 3)))
    {
        jumpFlag = 1; // 有效跳变点
    }
    else
    {
        jumpFlag = 0; // 无效跳变点
        // jumpNow = 0;
        // jumpNum = 0;
        //  carType = 0;
    }

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

    // jumpFlag 由 0 变 1 时才认为是新出现的跳变点
    if (jumpFlag - prevJumpFlag > 0)
    {
        jumpNum++; // 跳变点计数加一
    }

    // 寻找单调性改变点，并判断单调性改变点位置
    changeRY = Monotonicity_Change_Right(MT9V03X_H - 1, MT9V03X_H / 8);                                                                          // 右边线单调性突变点
    changeLY = Monotonicity_Change_Left(MT9V03X_H - 1, MT9V03X_H / 8);                                                                           // 左边线单调性突变点
    uint8 changeFlag = (changeRY > MT9V03X_H / 8 && changeRY < MT9V03X_H * 3 / 4) || (changeLY > MT9V03X_H / 8 && changeLY < MT9V03X_H * 3 / 4); // 单调性突变标志位

    if (changeFlag != 0) // 若有单调性突变点
    {
        if ((changeRY > MT9V03X_H / 8 || changeRY < MT9V03X_H * 3 / 4))
        {
            changeFlagR = 1;                 // 右边线单调性突变标志位
            changeRX = right_line[changeRY]; // 右边线单调性突变点X坐标
        }
        else if ((changeLY > MT9V03X_H / 8 || changeLY < MT9V03X_H * 3 / 4))
        {
            changeFlagL = 1;                // 左边线单调性突变标志位
            changeLX = left_line[changeLY]; // 左边线单调性突变点X坐标
        }
    }

    changeFlagR = changeFlagL ^ changeFlagR; // 单侧单调性突变

    // 图像左下角与右下角坐标（用于补线插值的起点）
    // pointLLC*: 左下角 (Left Lower Corner)
    // pointLRC*: 右下角 (Right Lower Corner)
    // 这些点作为补线时的基准点，和检测到的跳变点 (pointX, pointY) 连线，从而向上逐行补齐丢失的边线。
    uint16 pointLLCX = 0;             // 左下角 X 坐标，图像左边界
    uint16 pointLLCY = MT9V03X_H - 1; // 左下角 Y 坐标，图像底部（最后一行）
    uint16 pointLRCX = MT9V03X_W - 1; // 右下角 X 坐标，图像右边界
    uint16 pointLRCY = MT9V03X_H - 1; // 右下角 Y 坐标，图像底部（最后一行）

    float stepLength; // 横坐标插值步长

    if (count_Show > 5)
    {
        carType = 0; // 无环状态
    }

    // 计时数大于某个值的原因是，由于在斑马线后发车，车辆会将斑马线识别为跳变点
    // 故在此要求计时数大于某个值开始车辆状态的转换
    if (!zebra_flag && timeNUM > 200) // 如果没有识别到斑马线且计时数大于200
    {

        // 车辆状态判断
        if (jumpFlag)
        {                       // 若有有效跳变点
            if (pointType == 1) // 若为A字跳变点，说明出环或遇环
            {
                if (carType == 0) // 车辆的前一个状态为无环状态
                {
                    carType = 1; // 遇环状态
                }
                // else if (carType == 3) // 车辆的前一个状态为环内状态
                // {
                //     carType = 4; // 出环状态
                // }
            }
            else if (pointType == 2) // 若为V字跳变点
            {
                if (carType == 1) // 车辆的前一个状态为遇环状态
                {
                    carType = 2;       // 入环状态
                    jumpNow = jumpNum; // 记录当前跳变点次数
                    Vpoint_t = 15;
                }
                else if (carType == 2 && (jumpNum - jumpNow >= 1)) // 车辆的前一个状态为入环状态，且遇到跳变点次数不少于3次
                {
                    carType = 3; // 环内状态
                    Vpoint_t = 10;
                }
                else if (carType == 4) // 车辆的前一个状态为出环状态
                {
                    // carType = 0; // 无环状态
                }
            }
        }

        if (!jumpFlag)
        {
            if (changeFlag) // 若有单调性突变点
            {
                if (carType == 3)
                {
                    carType = 4; // 出环状态
                }
            }
        }

        // 环岛处理
        // 根据车辆不同的状态进行不同的环岛处理
        switch (carType)
        {
        case 0: // 为默认无环状态
            // if (jumpFlag == 1) // 出现跳变点
            // {
            //     if (pointType == 2) // V字跳变点
            //     {
            //         if (ringSide == 1) // 左环
            //         {
            //             // 补左边线
            //             stepLength = (float)(pointX - pointLLCX) / (float)(pointLLCY - pointY);
            //             for (i = 0; i < pointLLCY - pointY; i++)
            //             {
            //                 left_line[pointLLCY - i] = pointLLCX + (int)(i * stepLength);
            //                 image[pointLLCY - i][left_line[pointLLCY - i]] = 0; // 显示补线
            //             }
            //         }
            //         else if (ringSide == 2) // 右环
            //         {
            //             // 补右边线
            //             enterRingFlag_1 = 0;
            //             stepLength = (float)(pointLRCX - pointX) / (float)(pointLRCY - pointY);
            //             for (i = 0; i < pointLRCY - pointY; i++)
            //             {
            //                 right_line[pointLRCY - i] = pointLRCX - (int)(i * stepLength);
            //                 image[pointLRCY - i][right_line[pointLRCY - i]] = 0; // 显示补线
            //             }
            //         }
            //     }
            // }
            break;
        case 1:                // 遇到环
            if (jumpFlag == 1) // 出现跳变点
            {
                ringSide = pointSide; // 环岛方向与跳变点方向一致
            }
            break;
        case 2: // 入环状态

            if (jumpNum - jumpNow >= 2)
            {
                carType = 3; // 环内状态
                break;
            }

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
                if (1) // V字跳变点
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
        case 4:             // 出环状态
            if (changeFlag) // 出现跳变点
            {
                if (ringSide == 1) // 左环
                {
                    // 从右下角向上补右边线
                    stepLength = (float)(right_line[changeRY] - 0) / (float)(changeRY - MT9V03X_H / 4);
                    for (i = 0; i < changeRY - 1 - MT9V03X_H / 4; i++)
                    {
                        right_line[right_line[changeRY] - i] = right_line[changeRY] - (int)(i * stepLength);
                        image[changeRY - i][right_line[changeRY - i]] = 0; // 显示补线
                    }
                }
                else if (ringSide == 2) // 右环
                {
                    // 从坐下角向上补左边线
                    stepLength = (float)(pointLRCX - left_line[changeLY]) / (float)(changeLY - MT9V03X_H / 4);
                    for (i = 0; i < changeLY - 1 - MT9V03X_H / 4; i++)
                    {
                        left_line[changeLY - i] = left_line[changeLY] + (int)(i * stepLength);
                        image[changeLY - i][left_line[changeLY - i]] = 0; // 显示补线
                    }

                    // 如果图像1/2左侧处没有边线，则画出环线
                }
            }
            else
            {
                if (1)
                {
                    for (int i = 0; i < MT9V03X_W - 1; i++)
                    {
                        if (image[MT9V03X_H * 3 / 5][i] >= 200)
                        {
                            whiteNum++;
                        }
                    }
                    whiteNum_show = whiteNum;
                    if (whiteNum > MT9V03X_W - 20)
                    {
                        // 补画转弯线
                        stepLength = MT9V03X_W / (MT9V03X_H / 2);
                        for (i = 0; i < MT9V03X_H / 2 - 1; i++)
                        {
                            left_line[MT9V03X_H - 1 - i] = (int)(i * stepLength);
                            image[MT9V03X_H / 2 - 1 - i][left_line[MT9V03X_H - 1 - i]] = 0; // 显示补线
                        }
                    }
                }
            }

            break;
        default:
            break;
        }

        // /*----环岛处理在这里结束----*/

        // for (i = 0; i < MT9V03X_H; i++)
        // { // 重新计算中线
        //     mid_line[i] = (left_line[i] + right_line[i]) / 2;
        //     int m = mid_line[i];
        //     // Boundary-safe drawing of the midline (avoid out-of-bounds access)
        //     if (m > 0 && m < MT9V03X_W - 1)
        //     {
        //         image[i][m] = image[i][m - 1] = image[i][m + 1] = 0; // 粗线
        //     }
        //     else if (m == 0)
        //     {
        //         image[i][m] = image[i][m + 1] = 0;
        //     }
        //     else if (m == MT9V03X_W - 1)
        //     {
        //         image[i][m] = image[i][m - 1] = 0;
        //     }
        // }

        // 重新计算中线

        // 这一帧结束后将当前帧的标志位存为上一帧标志位
        prevJumpFlag = jumpFlag;
    }

    for (i = 0; i < MT9V03X_H; i++)
    { // 重新计算中线
        mid_line[i] = (left_line[i] + right_line[i]) / 2;
        image[i][mid_line[i]] = 0; // 粗线
    }

    // lcd_showint8(0, 5, jumpFlag);
    // lcd_showint16(0, 6, meetRingFlag * 1000 + enterRingFlag * 100 + leaveRingFlag * 10 + passRingFlag);

    // 取图像下1/4处的中线位置计算偏差
    // 取1/2处和3/4的图像计算偏差
    float tempOffset = 0.0f;
    for (i = MT9V03X_H / 2; i < MT9V03X_H * 3 / 4; i++)
    {
        tempOffset += (MT9V03X_W / 2 - mid_line[i]);
    }
    tempOffset /= (MT9V03X_H / 4);

    // float tempOffset = MT9V03X_W / 2 - mid_line[MT9V03X_H * 3 / 4];
    offset = tempOffset; // 更新偏差值
}