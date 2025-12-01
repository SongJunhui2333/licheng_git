// // 辅助函数

// // 右环岛左线开始
// void R_mend_leftline_right_island(int type, int flag_R_L)
// {
//     if (flag_R_L == 2)
//     {
//         // SetText("TYPE:" + type);
//         if (type == 4) // 状态机4的拉线 封住环岛前方赛道
//         {
//             int j;
//             int delta = right_turn_up[0] - 0;
//             if (delta == 0)
//                 delta = 1;
//             float k = (right_turn_up[1] - L_black[0]) * 1.0f / (delta * 1.0f);
//             // setText用户自定义("斜率" + k);
//             float b = L_black[0];
//             for (j = (int)0; j <= (int)right_turn_up[0]; j++)
//             {
//                 int jicun = ((int)(k * j + b));
//                 if (jicun >= 185)
//                     jicun = 185;
//                 else if (jicun <= 0)
//                     jicun = 0;
//                 L_black[j] = (unsigned char)jicun;
//             }
//         }
//         else if (type == 5) // 重新扫线，从左边往右边扫  取消掉一半
//         {
//             int j = 0;
//             if (right_turn_up[0] >= 4)
//             {
//                 int delta = right_turn_up[0] - 3;
//                 if (delta == 0)
//                     delta = 1;
//                 float k = (right_turn_up[1] - L_black[3]) * 1.0f / (delta * 1.0f);
//                 float b = L_black[3] - k * 3;
//                 if (right_turn_up[0] <= 24)
//                 {
//                     right_turn_up[0] = 24;
//                     for (int ss = 0; ss <= 24; ss++)
//                     {
//                         R_black[ss] = 0;
//                     }
//                 }
//                 for (j = (int)3; j <= (int)right_turn_up[0]; j++)
//                 {
//                     int jicun = ((int)(k * j + b));
//                     if (jicun >= 185)
//                         jicun = 185;
//                     else if (jicun <= 0)
//                         jicun = 0;
//                     L_black[j] = (unsigned char)jicun;
//                 }
//             }
//         }
//         else if (type == 6) // 状态6正常处理
//         {
//             int j = 0;
//             int youguai_y = 0;
//             int youguai_x = 0;
//             int leftguai_y = 0;
//             int leftguai_x = 0;
//             /***找左下拐点***********/
//             for (j = 2; j <= 60; j++)
//             {
//                 if (L_black[j - 1] - L_black[j - 2] <= 0 && L_black[j] - L_black[j - 1] >= 0 && (L_black[j + 1] - L_black[j] > 0) && leftflag[j - 2] == 1 && leftflag[j - 1] == 1 && leftflag[j] == 1)
//                 {
//                     leftguai_y = (int)(j - 1);
//                     leftguai_x = L_black[j - 1];
//                     break;
//                 }
//             }
//             for (j = 8; j <= 60; j++)
//             {
//                 // 右连接处
//                 if (R_black[j] != 0)
//                 {
//                     youguai_y = j;
//                     youguai_x = R_black[j];
//                     break;
//                 }
//             }

//             if (leftguai_y != 0 && leftguai_x != 0 && youguai_y != 4 && youguai_x != 0)
//             {
//                 // SetText("找到右上拐点");
//                 // SetText("右上拐点坐标：" + youguai_y + "  " + youguai_x);
//                 // SetText("找到左下拐点");
//                 // SetText("左下拐点坐标：" + leftguai_y + "  " + leftguai_x);
//                 if (youguai_y >= leftguai_y + 4)
//                 {
//                     jiansu_flag = 1;
//                 }
//                 else
//                     jiansu_flag = 0;
//                 // 开始拉线
//                 int delta = youguai_y - leftguai_y;
//                 if (delta == 0)
//                     delta = 1;
//                 float k = (youguai_x - leftguai_x) * 1.0f / delta;
//                 float b = leftguai_x - k * leftguai_y;
//                 if (youguai_y >= leftguai_y)
//                 {
//                     for (j = (int)leftguai_y; j <= (int)youguai_y; j++)
//                     {
//                         int jicun = ((int)(k * j + b));
//                         if (jicun >= 185)
//                             jicun = 185;
//                         else if (jicun <= 0)
//                             jicun = 0;
//                         L_black[j] = (unsigned char)jicun;
//                     }
//                 }
//             }
//         }
//         else if (type == 7) // 状态7拉线
//         {

//             // parameterA = 140;
//             //  parameterB = -4;
//             //  leftlinefuzhi(parameterA, parameterB, 0, 60);
//             int begin = 3;
//             int i;
//             if (huandaotype != 6)
//             {
//                 for (i = 1; i < 50; i++)
//                 {
//                     if (Pixels[i - 1][10] == 1 && Pixels[i][10] == 1 && Pixels[i + 1][10] != 1)
//                     {
//                         begin = i;
//                         break;
//                     }
//                 }
//             }
//             if (huandaotype == 6)
//             {
//                 for (i = 1; i < 50; i++)
//                 {
//                     if (Pixels[i - 1][20] == 1 && Pixels[i][20] == 1 && Pixels[i + 1][20] != 1)
//                     {
//                         begin = i;
//                         break;
//                     }
//                 }
//             }

//             if (begin == 3)
//             {
//                 begin = 30;
//             }

//             if (flag_find_huan_leftout_point == 1 && left_turn_out[0] < 30)
//             {
//                 L_black[begin] = 0;
//                 L_black[begin - 1] = 10;
//                 advanced_regression(1, left_turn_out[0] - 2, left_turn_out[0], begin - 1, begin);
//                 leftlinefuzhi(parameterA, parameterB, (char)0, 60);
//             }
//             else
//             {
//                 L_black[begin] = 0;
//                 L_black[begin - 1] = 10;
//                 advanced_regression(1, 0, 1, begin - 1, begin);
//                 leftlinefuzhi(parameterA, parameterB, 0, 60);
//             }
//         }
//         else if (type == 8)
//         {

//             flag_blank_out_huandao = 0;
//         }
//     }
// }
// // 右环岛左线结束
// // 右环岛右线开始

// void R_mend_rightline_right_island(int type, int flag_R_L)
// {
//     int i = 0;
//     flag_R_L = 2;
//     if (flag_R_L == 2)
//     {
//         if (type == 1 || type == 2) // 状态机1和2操作从下拐点开始往上拉，拉到上拐点下五行  type右线赋值操作
//         {
//             int j = 0;
//             if (right_turn_down[0] >= 3 && right_turn_middle[0] >= 2 && right_turn_middle[0] >= right_turn_down[0])
//             {
//                 advanced_regression(2, right_turn_down[0] - 2, right_turn_down[0], right_turn_middle[0] - 1, right_turn_middle[0]);

//                 for (j = (int)right_turn_down[0]; j <= (int)right_turn_middle[0]; j++)
//                 {
//                     int jicun = (int)(parameterB * j + parameterA);
//                     if (jicun >= 185)
//                         jicun = 185;
//                     else if (jicun <= 0)
//                         jicun = 0;
//                     R_black[j] = (unsigned char)jicun;
//                 }
//             }
//             else
//             {
//                 advanced_regression(2, 6, 8, 10, 12);

//                 for (j = 0; j <= right_turn_down[0] + 10; j++)
//                 {
//                     int jicun = (int)(parameterB * j + parameterA);
//                     if (jicun >= 185)
//                         jicun = 185;
//                     else if (jicun <= 0)
//                         jicun = 0;
//                     R_black[j] = (unsigned char)jicun;
//                 }
//             }
//         }
//         else if (type == 3) // 状态机三操作---仍旧拉底下的线
//         {
//             int j = 0;
//             int delta = right_turn_middle[0] - 0;
//             if (delta == 0)
//                 delta = 1;
//             float k = (right_turn_middle[1] - R_black[0]) * 1.0f / delta;
//             float b = R_black[0];
//             for (j = (int)0; j <= (int)right_turn_middle[0]; j++)
//             {
//                 int jicun = ((int)(k * j + b));
//                 if (jicun >= 185)
//                     jicun = 185;
//                 else if (jicun <= 0)
//                     jicun = 0;
//                 R_black[j] = (unsigned char)jicun;
//             }
//         }
//         else if (type == 8)
//         {
//             int begin = 1;
//             for (i = 0; i < 50; i++)
//             {
//                 if (R_black[i] == 0 && R_black[i + 1] == 0 && R_black[i + 2] != 0)
//                 {
//                     begin = i + 2;
//                     break;
//                 }
//             }

//             if (begin != 1)
//             {
//                 if (R_black[begin + 7] != 0)
//                 {
//                     if (begin <= 27)
//                     {
//                         advanced_regression(2, begin + 5, begin + 6, begin + 7, begin + 8);
//                     }
//                     if (begin >= 28)
//                     {
//                         regression(2, begin + 3, begin + 5);
//                     }
//                     monirightfuzhi(parameterA, parameterB, 0, 60);
//                     if (moniright[2] < L_black[2] && moniright[6] < L_black[6] && moniright[10] < L_black[10] && moniright[14] < L_black[14])
//                     {
//                         R_blackfuzhi(parameterA, parameterB, 0, begin + 2);
//                     }
//                 }
//                 else
//                 {
//                     if (begin <= 27)
//                     {
//                         advanced_regression(2, begin + 5, begin + 6, begin + 7, begin + 8);
//                     }
//                     if (begin >= 28)
//                     {
//                         regression(2, begin + 3, begin + 5);
//                     }
//                     monirightfuzhi(parameterA, parameterB, 0, 60);
//                     if (moniright[2] < L_black[2] && moniright[6] < L_black[6] && moniright[10] < L_black[10] && moniright[14] < L_black[14])
//                     {
//                         R_blackfuzhi(parameterA, parameterB, 0, begin + 2);
//                     }
//                 }
//             }
//             else
//             {
//                 advanced_regression(2, 27, 28, 29, 30);
//                 monirightfuzhi(parameterA, parameterB, 0, 60);
//                 if (moniright[2] < L_black[2] && moniright[6] < L_black[6] && moniright[10] < L_black[10] && moniright[14] < L_black[14])
//                 {
//                     R_blackfuzhi(parameterA, parameterB, 0, 60);
//                 }
//             }
//         }
//         else if (type == 4)
//         {
//             if (flag_find_huan_rightmiddle_point == 1 && right_turn_middle[0] <= 33)
//             {
//                 regression(2, right_turn_middle[0], right_turn_middle[0] + 1);
//                 R_blackfuzhi(parameterA, parameterB, 0, (int)(right_turn_middle[0] + 1));
//             }
//         }
//         else if (type == 7) // 状态7拉线
//         {
//             for (i = 0; i <= 60; i++)
//             {
//                 R_black[i] = 0;
//             }
//         }
//     }
// }

// // 右环岛右线结束

// void cakkuandu()
// {

//     int i;
//     for (i = 0; i <= 69; i++)
//     {
//         calkuan[i] = 0;
//     }
//     for (i = 0; i <= break_hangshu; i++)
//     {
//         calkuan[i] = L_black[i] - R_black[i];
//     }
//     for (i = 0; i <= break_hangshu; i++)
//     {
//         // SetText("行数+宽度" + "    " + i + "    " + calkuan[i]);
//     }
// }
// // 计算宽度结束

// // 找环岛拐点开始
// int flag_find_huan_leftout_point;
// int left_turn_out[2];
// void findhuan_leftout_point(int start_point, int end_point, int RoadName) // 找换到左下拐点  为左环岛而修改
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         // setText用户自定义("l_start"+ l_start);
//         for (j = start_point; j <= end_point; j++)
//         {
//             // 找左边突变(下拐点)；连续且是存在的点（）
//             if (L_black[j] - L_black[j - 1] <= 0 && L_black[j] - L_black[j + 1] <= 0 && L_black[j] - L_black[j + 2] <= 0 && L_black[j] - L_black[j - 2] <= 0 && (L_black[j] - L_black[j + 3] <= -1 && L_black[j] - L_black[j - 3] <= -1) && leftflag[j] == 1 && leftflag[j - 1] == 1 && leftflag[j - 2] == 1 && leftflag[j - 3] == 1 && leftflag[j + 1] == 1 && leftflag[j + 2] == 1 && leftflag[j + 3] == 1)
//             {
//                 int record = 0;
//                 int i;
//                 if (L_black[j - 1] == L_black[j - 2] && L_black[j - 2] == L_black[j - 3])
//                     j = j - 1;
//                 record = j;
//                 for (i = j; i >= 1; i--)
//                 {
//                     if (L_black[i] == L_black[i - 1])
//                         record = i - 1;
//                     if (L_black[i] != L_black[i - 1])
//                         break;
//                 }
//                 left_turn_out[0] = (int)(record); // 数组里面没有第0行
//                 left_turn_out[1] = L_black[record];
//                 flag_find_huan_leftout_point = 1;

//                 break;
//             }
//         }
//     }
// }

// int flag_find_huan_rightout_point;
// int right_turn_out[2];
// void findhuan_rightout_point(int start_point, int end_point, int RoadName) // 找环岛右下拐点
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         for (j = start_point; j <= end_point; j++)
//         {
//             // 找右边突变(下拐点)
//             if (R_black[j - 1] - R_black[j - 2] >= 0 && R_black[j - 1] - R_black[j - 3] >= 0 && R_black[j - 1] - R_black[j] >= 0 && R_black[j - 1] - R_black[j + 1] >= 0 && (R_black[j - 1] - R_black[j - 4] >= 1 && R_black[j - 1] - R_black[j + 2] >= 1) && rightflag[j - 2] == 1 && rightflag[j - 1] == 1 && rightflag[j] == 1 && rightflag[j + 1] == 1 && rightflag[j - 3] == 1 && rightflag[j - 4] == 1 && rightflag[j + 2] == 1 && R_black[j - 1] < 130 && R_black[j - 1] > 10)
//             {
//                 int record = 0;
//                 int i;
//                 record = j;
//                 for (i = j; i >= 1; i--)
//                 {
//                     if (R_black[i] == R_black[i - 1])
//                         record = i - 1;
//                     if (R_black[i] != R_black[i - 1])
//                         break;
//                 }
//                 right_turn_out[0] = (int)(record); // 数组里面没有第0行
//                 right_turn_out[1] = R_black[record];
//                 flag_find_huan_rightout_point = 1;

//                 break;
//             }
//         }
//         // 如果右下拐点坐标不为空 且 环岛状态不是4 就认定找到环岛的右下拐点
//     }
// }
// void findhuan_rightdown_point(int start_point, int end_point, int RoadName) // 找环岛右下拐点
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         for (j = start_point; j <= end_point; j++)
//         {
//             // 找右边突变(下拐点)
//             if (R_black[j] - R_black[j - 1] >= 0 && R_black[j] - R_black[j - 2] >= 0 && R_black[j] - R_black[j + 1] >= 0 && R_black[j] - R_black[j + 2] >= 0 && (R_black[j] - R_black[j - 1] <= 5 || R_black[j] - R_black[j - 2] <= 5) && j <= break_hangshu - 3

//                 && (R_black[j] - R_black[j - 2] >= 1 || R_black[j] - R_black[j - 3] >= 1 || (j >= 4 && R_black[j] - R_black[j - 4] >= 1) || (j >= 5 && R_black[j] - R_black[j - 4] >= 1)) && (R_black[j] - R_black[j + 2] >= 1 || R_black[j] - R_black[j + 3] >= 1 || R_black[j] - R_black[j + 4] >= 1 || R_black[j] - R_black[j + 5] >= 1 || R_black[j] - R_black[j + 6] >= 1) && rightflag[j - 2] == 1 && rightflag[j - 1] == 1 && rightflag[j] == 1 && (rightflag[j + 1] == 1 || R_black[j + 1] == 0))
//             {
//                 if (R_black[j] < 175)
//                 {
//                     int record = 0;
//                     // int i;
//                     record = j;
//                     // for (i = j; i >= 1; i--)
//                     //{
//                     //     if (R_black[i] == R_black[i - 1]) record = i - 1;
//                     //     if (R_black[i] != R_black[i - 1]) break;
//                     // }
//                     right_turn_down[0] = (int)(record); // 数组里面没有第0行
//                     right_turn_down[1] = R_black[record];
//                     flag_find_huan_rightdown_point = 1;
//                     //    SetText("找到环岛右下拐点  找到右下拐点" + right_turn_down[0] + "     " + right_turn_down[1]);
//                     break;
//                 }
//             }
//         }
//         // 如果右下拐点坐标不为空 且 环岛状态不是4 就认定找到环岛的右下拐点
//     }
// }
// void findhuan100_rightdown_point(int start_point, int end_point, int RoadName) // 找环岛右下拐点
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         for (j = start_point; j <= end_point; j++)
//         {
//             // 找右边突变(下拐点)
//             if (R_black[j] - R_black[j - 1] >= 0 && R_black[j] - R_black[j - 2] >= 0 && R_black[j] - R_black[j + 1] >= 0 && R_black[j] - R_black[j + 2] >= 0 && R_black[j] - R_black[j - 1] <= 5 && R_black[j - 1] - R_black[j - 2] <= 5

//                 && (R_black[j] - R_black[j - 2] >= 1 || R_black[j] - R_black[j - 3] >= 1) && (R_black[j] - R_black[j + 2] >= 1 || R_black[j] - R_black[j + 3] >= 1 || R_black[j] - R_black[j + 4] >= 1 || R_black[j] - R_black[j + 5] >= 1 || R_black[j] - R_black[j + 6] >= 1) && rightflag[j - 2] == 1 && rightflag[j - 1] == 1 && rightflag[j] == 1 && rightflag[j + 1] == 1)
//             {
//                 if (R_black[j] < 175)
//                 {
//                     int record = 0;

//                     record = j;
//                     // for (i = j; i >= 1; i--)
//                     //{
//                     //     if (R_black[i] == R_black[i - 1]) record = i - 1;
//                     //     if (R_black[i] != R_black[i - 1]) break;
//                     // }
//                     right_turn_down[0] = (int)(record); // 数组里面没有第0行
//                     right_turn_down[1] = R_black[record];
//                     flag_find_huan_rightdown_point = 1;

//                     break;
//                 }
//             }
//         }
//         // 如果右下拐点坐标不为空 且 环岛状态不是4 就认定找到环岛的右下拐点
//     }
// }

// void findhuan_leftdown_point(int start_point, int end_point, int RoadName) // 找换到左下拐点  为左环岛而修改
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         // setText用户自定义("l_start"+ l_start);
//         for (j = start_point; j <= end_point; j++)
//         {
//             // 找左边突变(下拐点)；连续且是存在的点（）
//             if (My_Abs(L_black[j - 1], L_black[j - 2]) <= 4 && My_Abs(L_black[j], L_black[j - 1]) <= 5 && (L_black[j + 1] - L_black[j] >= 2) && (L_black[j + 2] - L_black[j] >= 2) && leftflag[j - 2] == 1 && leftflag[j - 1] == 1 && leftflag[j] == 1)
//             {
//                 int record = 0;
//                 int i;
//                 if (L_black[j - 1] == L_black[j - 2] && L_black[j - 2] == L_black[j - 3])
//                     j = j - 1;
//                 record = j;
//                 for (i = j; i >= 1; i--)
//                 {
//                     if (L_black[i] == L_black[i - 1])
//                         record = i - 1;
//                     if (L_black[i] != L_black[i - 1])
//                         break;
//                 }
//                 left_turn_down[0] = (int)(record); // 数组里面没有第0行
//                 left_turn_down[1] = L_black[record];
//                 flag_find_huan_leftdown_point = 1;
//                 // SetText("找到环岛左下拐点  找到左下拐点" + left_turn_down[0] + "     " + left_turn_down[1]);
//                 break;
//             }
//         }
//     }
// }
// void findhuan_rightmiddle_point(int start_point, int end_point) // 找环岛右中拐点  此函数待调整
// {
//     int i;
//     if ((huandao_memory == 4 || huandao_memoryforleft == 4) && break_hangshu >= 10)
//         end_point = break_hangshu - 10;
//     for (i = start_point; i <= end_point; i++)
//     {
//         // 找you拐点
//         if (i >= 8 && (R_black[i + 1] - R_black[i] <= 0) && (R_black[i + 2] - R_black[i] <= 0) &&
//             (R_black[i - 1] - R_black[i] <= 0) && (R_black[i - 2] - R_black[i] <= 0) && (R_black[i - 3] - R_black[i] <= 0) && (R_black[i - 4] - R_black[i] <= 0) && ((R_black[i - 5] - R_black[i] <= -1) || (R_black[i - 6] - R_black[i] <= -1) || (R_black[i - 7] - R_black[i] <= -1) || (R_black[i - 8] - R_black[i] <= -1))

//             && rightflag[i - 2] == 1 && rightflag[i - 1] == 1 && rightflag[i] == 1 && rightflag[i + 1] == 1 && rightflag[i + 2] == 1)
//         {
//             right_turn_middle[0] = (int)(i);
//             right_turn_middle[1] = R_black[i];
//             flag_find_huan_rightmiddle_point = 1;

//             break;
//         }
//     }
// }
// void findhuan_leftmiddle_point(int start_point, int end_point) // 找环岛左中拐点
// {
//     int i;
//     for (i = start_point; i <= end_point; i++)
//     {
//         // 找左拐点
//         if (i >= 8 && (L_black[i + 1] - L_black[i] >= 0) && (L_black[i + 2] - L_black[i] >= 0) && (L_black[i - 1] - L_black[i] >= 0) && (L_black[i - 2] - L_black[i] >= 0) && (L_black[i - 3] - L_black[i] >= 0) && (L_black[i - 4] - L_black[i] >= 1 || L_black[i - 5] - L_black[i] >= 1) && (leftflag[i - 2] == 1 || leftflag[i - 3] == 1) && leftflag[i - 1] == 1 && leftflag[i] == 1 && leftflag[i + 1] == 1 && (leftflag[i + 2] == 1 || leftflag[i + 3] == 1) && i <= break_hangshu - 4)
//         {
//             int record = 0;
//             int j;
//             record = i;
//             for (j = i; j >= 1; j--)
//             {
//                 if (L_black[j] == L_black[j - 1])
//                     record = j - 1;
//                 if (L_black[j] != L_black[j - 1])
//                     break;
//             }
//             if (L_black[record] <= 178)
//             {
//                 left_turn_middle[0] = (int)(record);
//                 left_turn_middle[1] = L_black[record];
//                 flag_find_huan_leftmiddle_point = 1;

//                 break;
//             }
//         }
//     }
// }
// void findhuan_rightup_point(int start_point, int end_point, int RoadName) // 找环岛右上拐点
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         for (j = start_point; j <= end_point; j++)
//         {
//             // 如果出现了连续的行数超过45行判定为长直道，此时不存在拐点
//             // if (flag_s >= 47) break;
//             // 右上拐点
//             if ((R_black[j - 1] - R_black[j - 2]) >= 4 && My_Abs(R_black[j], R_black[j - 1]) <= 4 && My_Abs(R_black[j + 1], R_black[j]) <= 4 && rightflag[j - 1] == 1 && rightflag[j] == 1 && rightflag[j + 1] == 1 && My_Abs(R_black[j - 1], LCenter[j - 1]) <= 45 && My_Abs(R_black[j], LCenter[j]) <= 45 && (huandao_memory != 4 || (huandao_memory == 4 && R_black[j - 1] >= 20 && j < break_hangshu - 2))) //
//             {
//                 right_turn_up[0] = (int)(j - 1);
//                 right_turn_up[1] = R_black[j - 1];
//                 flag_find_huan_rightup_point = 1;
//                 //  SetText("找到环岛右上拐点  找到右上拐点" + right_turn_up[0] + "     " + right_turn_up[1]);
//                 break;
//             }
//         }
//     }
// }
// void findhuan_leftup_point(int start_point, int end_point, int RoadName) // 找环岛左上拐点
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         for (j = start_point; j <= end_point; j++)
//         {
//             // 如果出现了连续的行数超过45行判定为长直道，此时不存在拐点
//             // if (flag_s >= 47) break;
//             // 左上拐点
//             if ((L_black[j - 2] - L_black[j - 1]) >= 6 && My_Abs(L_black[j], L_black[j - 1]) <= 3 && My_Abs(L_black[j + 1], L_black[j]) <= 3 && leftflag[j - 1] == 1 && leftflag[j] == 1 && leftflag[j + 1] == 1 && My_Abs(L_black[j - 1], LCenter[j - 1]) <= 45 && My_Abs(L_black[j], LCenter[j]) <= 45

//                 && ((huandao_memoryforleft != 4 || (huandao_memoryforleft == 4 && L_black[j - 1] <= 160 && j <= break_hangshu - 2))))
//             {

//                 left_turn_up[0] = (int)(j - 1); // 数组里面没有第0行
//                 left_turn_up[1] = L_black[j - 1];
//                 flag_find_huan_leftup_point = 1;
//                 //    SetText("找到环岛左上拐点77    找到左上拐点77 " + left_turn_up[0] + "     " + left_turn_up[1]);
//                 break;
//             }
//         }
//     }
// }
// void findhuan_rightup_point77(int start_point, int end_point, int RoadName) // 找环岛右上拐点
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         for (j = start_point; j <= end_point; j++)
//         {
//             // 如果出现了连续的行数超过45行判定为长直道，此时不存在拐点
//             // if (flag_s >= 47) break;
//             // 右上拐点
//             if ((R_black[j - 1] - R_black[j - 2]) >= 4 && My_Abs(R_black[j], R_black[j - 1]) <= 4 && My_Abs(R_black[j + 1], R_black[j]) <= 4 && rightflag[j - 1] == 1 && rightflag[j] == 1 && rightflag[j + 1] == 1)
//             {
//                 right_turn_up[0] = (int)(j - 1);
//                 right_turn_up[1] = R_black[j - 1];
//                 flag_find_huan_rightup_point = 1;
//                 // SetText("找到环岛右上拐点77  找到右上拐点77" + right_turn_up[0] + "     " + right_turn_up[1]);
//                 break;
//             }
//         }
//     }
// }
// void findhuan_rightup_point44(int start_point, int end_point, int RoadName) // 找环岛右上拐点
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         for (j = start_point; j <= end_point; j++)
//         {
//             if (R_black[j] >= 20)
//             { // 如果出现了连续的行数超过45行判定为长直道，此时不存在拐点
//               // if (flag_s >= 47) break;
//               // 右上拐点
//                 if ((R_black[j - 1] - R_black[j - 2]) >= 4 && My_Abs(R_black[j], R_black[j - 1]) <= 4 && My_Abs(R_black[j + 1], R_black[j]) <= 4 && rightflag[j - 1] == 1 && rightflag[j] == 1 && rightflag[j + 1] == 1 && My_Abs(R_black[j - 1], LCenter[j - 1]) <= 60 && My_Abs(R_black[j], LCenter[j]) <= 60 && (huandao_memory != 4 || (huandao_memory == 4 && j <= break_hangshu - 2)))
//                 {
//                     right_turn_up[0] = (int)(j - 1);
//                     right_turn_up[1] = R_black[j - 1];
//                     flag_find_huan_rightup_point = 1;
//                     //    SetText("找到环岛右上拐点  找到右上拐点44" + right_turn_up[0] + "     " + right_turn_up[1]);
//                     break;
//                 }
//             }
//         }
//     }
// }
// void findhuan_leftup_point77(int start_point, int end_point, int RoadName) // 找环岛左上拐点
// {
//     int j;
//     if (RoadName == ROUNDISLAND)
//     {
//         for (j = start_point; j <= end_point; j++)
//         {
//             // 如果出现了连续的行数超过45行判定为长直道，此时不存在拐点
//             // if (flag_s >= 47) break;
//             // 左上拐点
//             if ((L_black[j - 2] - L_black[j - 1]) >= 4 && My_Abs(L_black[j], L_black[j - 1]) <= 3 && My_Abs(L_black[j + 1], L_black[j]) <= 3 && leftflag[j - 1] == 1 && leftflag[j] == 1 && leftflag[j + 1] == 1)
//             {

//                 left_turn_up[0] = (int)(j - 1); // 数组里面没有第0行
//                 left_turn_up[1] = L_black[j - 1];
//                 flag_find_huan_leftup_point = 1;

//                 break;
//             }
//         }
//     }
// }

// void advanced_regression(int type, int startline1, int endline1, int startline2, int endline2)
// {
//     int i = 0;
//     int sumlines1 = endline1 - startline1;
//     int sumlines2 = endline2 - startline2;
//     int sumX = 0;
//     int sumY = 0;
//     float averageX = 0;
//     float averageY = 0;
//     float sumUp = 0;
//     float sumDown = 0;
//     if (type == 0) // 拟合中线
//     {
//         /**计算sumX sumY**/
//         for (i = startline1; i < endline1; i++)
//         {
//             sumX += i;
//             sumY += LCenter[i];
//         }
//         for (i = startline2; i < endline2; i++)
//         {
//             sumX += i;
//             sumY += LCenter[i];
//         }
//         averageX = (float)(sumX / (sumlines1 + sumlines2)); // x的平均值
//         averageY = (float)(sumY / (sumlines1 + sumlines2)); // y的平均值
//         for (i = startline1; i < endline1; i++)
//         {
//             sumUp += (LCenter[i] - averageY) * (i - averageX);
//             sumDown += (i - averageX) * (i - averageX);
//         }
//         for (i = startline2; i < endline2; i++)
//         {
//             sumUp += (LCenter[i] - averageY) * (i - averageX);
//             sumDown += (i - averageX) * (i - averageX);
//         }
//         if (sumDown == 0)
//             parameterB = 0;
//         else
//             parameterB = sumUp / sumDown;
//         parameterA = averageY - parameterB * averageX;
//     }
//     else if (type == 1) // 拟合左线
//     {
//         /**计算sumX sumY**/
//         for (i = startline1; i < endline1; i++)
//         {
//             sumX += i;
//             sumY += L_black[i];
//         }
//         for (i = startline2; i < endline2; i++)
//         {
//             sumX += i;
//             sumY += L_black[i];
//         }
//         averageX = (float)(sumX / (sumlines1 + sumlines2)); // x的平均值
//         averageY = (float)(sumY / (sumlines1 + sumlines2)); // y的平均值
//         for (i = startline1; i < endline1; i++)
//         {
//             sumUp += (L_black[i] - averageY) * (i - averageX);
//             sumDown += (i - averageX) * (i - averageX);
//         }
//         for (i = startline2; i < endline2; i++)
//         {
//             sumUp += (L_black[i] - averageY) * (i - averageX);
//             sumDown += (i - averageX) * (i - averageX);
//         }
//         if (sumDown == 0)
//             parameterB = 0;
//         else
//             parameterB = sumUp / sumDown;
//         parameterA = averageY - parameterB * averageX;
//     }
//     else if (type == 2) // 拟合右线
//     {
//         /**计算sumX sumY**/
//         for (i = startline1; i < endline1; i++)
//         {
//             sumX += i;
//             sumY += R_black[i];
//         }
//         for (i = startline2; i < endline2; i++)
//         {
//             sumX += i;
//             sumY += R_black[i];
//         }
//         averageX = (float)(sumX / (sumlines1 + sumlines2)); // x的平均值
//         averageY = (float)(sumY / (sumlines1 + sumlines2)); // y的平均值
//         for (i = startline1; i < endline1; i++)
//         {
//             sumUp += (R_black[i] - averageY) * (i - averageX);
//             sumDown += (i - averageX) * (i - averageX);
//         }
//         for (i = startline2; i < endline2; i++)
//         {
//             sumUp += (R_black[i] - averageY) * (i - averageX);
//             sumDown += (i - averageX) * (i - averageX);
//         }
//         if (sumDown == 0)
//             parameterB = 0;
//         else
//             parameterB = sumUp / sumDown;
//         parameterA = averageY - parameterB * averageX;
//     }
// }

// void regression(int type, int startline, int endline) // 最小二乘法拟合曲线，分别拟合中线，左线，右线,type表示拟合哪几条线   xy 颠倒
// {
//     int i = 0;
//     int sumlines = endline - startline;
//     int sumX = 0;
//     int sumY = 0;
//     float averageX = 0;
//     float averageY = 0;
//     float sumUp = 0;
//     float sumDown = 0;
//     if (type == 0) // 拟合中线
//     {
//         for (i = startline; i < endline; i++)
//         {
//             sumX += i;
//             sumY += LCenter[i];
//         }
//         if (sumlines != 0)
//         {
//             averageX = (float)(sumX / sumlines); // x的平均值
//             averageY = (float)(sumY / sumlines); // y的平均值
//         }
//         else
//         {
//             averageX = 0; // x的平均值
//             averageY = 0; // y的平均值
//         }
//         for (i = startline; i < endline; i++)
//         {
//             sumUp += (LCenter[i] - averageY) * (i - averageX);
//             sumDown += (i - averageX) * (i - averageX);
//         }
//         if (sumDown == 0)
//             parameterB = 0;
//         else
//             parameterB = sumUp / sumDown;
//         parameterA = averageY - parameterB * averageX;
//     }
//     else if (type == 1) // 拟合左线
//     {
//         for (i = startline; i < endline; i++)
//         {
//             sumX += i;
//             sumY += L_black[i];
//         }
//         if (sumlines == 0)
//             sumlines = 1;
//         averageX = (float)(sumX / sumlines); // x的平均值
//         averageY = (float)(sumY / sumlines); // y的平均值
//         for (i = startline; i < endline; i++)
//         {
//             sumUp += (L_black[i] - averageY) * (i - averageX);
//             sumDown += (i - averageX) * (i - averageX);
//         }
//         if (sumDown == 0)
//             parameterB = 0;
//         else
//             parameterB = sumUp / sumDown;
//         parameterA = averageY - parameterB * averageX;
//     }
//     else if (type == 2) // 拟合右线
//     {
//         for (i = startline; i < endline; i++)
//         {
//             sumX += i;
//             sumY += R_black[i];
//         }
//         if (sumlines == 0)
//             sumlines = 1;
//         averageX = (float)(sumX / sumlines); // x的平均值
//         averageY = (float)(sumY / sumlines); // y的平均值
//         for (i = startline; i < endline; i++)
//         {
//             sumUp += (R_black[i] - averageY) * (i - averageX);
//             sumDown += (i - averageX) * (i - averageX);
//         }
//         if (sumDown == 0)
//             parameterB = 0;
//         else
//             parameterB = sumUp / sumDown;
//         parameterA = averageY - parameterB * averageX;
//     }
// }

// // 求两数之差绝对值开始
// int My_Abs(int a, int b)
// {

//     if ((a - b) > 0)
//         return ((int)(a - b));
//     else
//         return ((int)(b - a));
// }
// // 求两数之差绝对值结束
// // 左右线赋值开始
// void leftlinefuzhi(float A, float B, int8 start_point, int8 end_point)
// {
//     int8 m;
//     for (m = start_point; m <= end_point; m++)
//     {
//         if ((B * m + A) >= 255)
//             L_black[m] = 255;
//         if ((B * m + A) <= 0)
//             L_black[m] = 0;
//         else if (0 < (B * m + A) && (B * m + A) < 255)
//             L_black[m] = (int8)(B * m + A);
//     }
// }
// void R_blackfuzhi(float A, float B, int8 start_point, int8 end_point)
// {
//     int8 m;
//     for (m = start_point; m <= end_point; m++)
//     {
//         if ((B * m + A) < 0)
//         {
//             R_black[m] = 0;
//         }
//         else if ((B * m + A) >= 0 && (B * m + A) <= 255)
//             R_black[m] = (int8)(B * m + A);
//         if ((B * m + A) >= 255)
//             R_black[m] = 255;
//     }
// }
// // 左右线赋值结束
// // 斜入十字赋值工具开始
// void zhongxianfuzhi(int8 start_point, int8 end_point)
// {
//     int8 m;
//     for (m = start_point; m <= end_point; m++)
//     {
//         LCenter[m] = (int8)(L_black[m] / 2 + R_black[m] / 2);
//     }
// }
// void monileftfuzhi(float A, float B, int start_point, int end_point)
// {
//     int m;
//     for (m = start_point; m <= end_point; m++)
//     {
//         if ((B * m + A) >= 255)
//             monileft[m] = 255;
//         if ((B * m + A) <= 0)
//             monileft[m] = 0;
//         else if (0 < (B * m + A) && (B * m + A) < 255)
//             monileft[m] = (int)(B * m + A);
//     }
// }
// void monirightfuzhi(float A, float B, int start_point, int end_point)
// {
//     int m;
//     for (m = start_point; m <= end_point; m++)
//     {
//         if ((B * m + A) >= 255)
//             moniright[m] = 255;
//         if ((B * m + A) <= 0)
//             moniright[m] = 0;
//         else if (0 < (B * m + A) && (B * m + A) < 255)
//             moniright[m] = (int)(B * m + A);
//     }
// }
// void monizhongfuzhi(float A, float B, int start_point, int end_point)
// {
//     int m;
//     for (m = start_point; m <= end_point; m++)
//     {
//         if ((B * m + A) >= 255)
//             monimiddle[m] = 255;
//         if ((B * m + A) <= 0)
//             monimiddle[m] = 0;
//         else if (0 < (B * m + A) && (B * m + A) < 255)
//             monimiddle[m] = (int)(B * m + A);
//     }
// }

// /*
// 注释1:环岛的八个状态
// 环岛分为八个状态。以右环岛为例，一二三状态都在未入环的直道上，这时候需要把右环岛的
// 中拐点和下拐点拉起来，补成一条标准的直道。四状态在右环岛的入口那边，需要把右环岛的
// 上拐点和左边边线上的某一点连线，封住前方的道路，让车模进入圆环。五六状态都在环岛里
// 面跑，没有什么技巧。其状态就是到出环岛的口了，这时候需要把左下角的点和白色赛道的右
// 上角的点连起来，让车子右转。八状态车模基本上就已经正了，这个时候需要做的事把右环岛
// 中拐点和上拐点连起来，让车模跑直线。

// */

// /*注释2:
// 各个变量的含义
// huandaotype:环岛的类型，在跑之前先手动输入环岛大小 50 / 60... 来给出不同的判断
// turepodaoflag :坡道标志位，即坡道上不判断
// countpodao :坡道结束的帧数计数
// rukuflag :车库标志位，在判断要进入车库时不判环岛
// threemode :三叉模式标志位
// firstku、countfirstku:第一次经过车库时标志位(因为我的车要跑两圈)
// truedoubleright :找到右边线的双断点时置1
// huandao_memory :环岛状态标志位

// flag_find_huan_leftdown_point
// flag_find_huan_rightdown_point
// flag_find_huan_rightmiddle_point
// flag_find_huan_leftmiddle_point
// flag_find_huan_rightup_point
// flag_find_huan_leftup_point :环岛的拐点是否找到的标志

// ROUNDISLAND:这个是个常量，大家可以直接忽视

// right_turn_down[0]
// right_turn_down[1]:诸如此类的是拐点的数组，0存的是行数，1存的是列数

// R_black[70]/L_black[70]:左右边线数组
// calkuan[i]:第i行的宽度
// truepianfangflag [i]:方差是否满足标志位，大家可以看环岛辅助判断的文章，有具体方法
// leftk :左边线之前算出的斜率
// break_hangshu:左边线的有效行数
// huandao_flag ：是否判断到环岛的标志

// huandao_flag_R_L:左环还是右环

// crossrecord
// crossderecord
// cross31 :这三个没有用 大家直接忽略

// parameterB :斜率
// */

// void juge_left_succesive_and_if_k_limit()
// {
//     if (huandaotype != 0 && turepodaoflag == 0 && (countpodao == 0 || countpodao > 1) && rukuflag == 0 && (threemode == 0 || threemode == 6 || threemode == 12) && firstku == 0 && countfirstku == 0)
//     {
//         if (truedoubleright != 0 || huandao_memory != 0)
//         {
//             if (huandao_memoryforleft == 0)
//             {
//                 int i;
//                 float leftk = 0;
//                 cakkuandu();
//                 flag_find_huan_leftdown_point = 0;
//                 flag_find_huan_rightdown_point = 0;
//                 flag_find_huan_rightmiddle_point = 0;
//                 flag_find_huan_leftmiddle_point = 0;
//                 flag_find_huan_rightup_point = 0;
//                 flag_find_huan_leftup_point = 0;

//                 if (huandao_memory != 3 && huandao_memory != 4) // 环岛找拐点开始  右下
//                 {
//                     if (huandaotype != 1 && huandaotype != 6)
//                     {
//                         findhuan_rightdown_point(3, 28, ROUNDISLAND);
//                     }
//                     if (huandaotype == 1)
//                     {
//                         findhuan_rightdown_point(3, 22, ROUNDISLAND);
//                     }
//                     if (huandaotype == 6)
//                     {
//                         findhuan_rightdown_point(3, 34, ROUNDISLAND);
//                     }
//                 }
//                 if (huandao_memory == 3)
//                 {
//                     if (huandaotype == 1)
//                     {
//                         findhuan_rightdown_point(3, 10, ROUNDISLAND);
//                     }
//                     if (huandaotype == 2)
//                     {
//                         findhuan_rightdown_point(3, 10, ROUNDISLAND);
//                     }
//                     if (huandaotype == 3)
//                     {
//                         findhuan_rightdown_point(3, 14, ROUNDISLAND);
//                     }
//                     if (huandaotype == 6)
//                     {
//                         findhuan_rightdown_point(3, 16, ROUNDISLAND);
//                     }
//                 }
//                 if (huandao_memory == 4)
//                 {
//                     findhuan_rightdown_point(3, 18, ROUNDISLAND);
//                 }
//                 findhuan_leftdown_point(5, 32, ROUNDISLAND); // 左下

//                 if (flag_find_huan_rightdown_point == 1 && (right_turn_down[0] + 8) < 45 && huandaotype != 3) // 右中 下找到
//                 {
//                     if (huandao_memory != 4)
//                     {
//                         findhuan_rightmiddle_point(right_turn_down[0] + 8, 46);
//                     }
//                     if (huandao_memory == 4)
//                     {
//                         findhuan_rightmiddle_point(right_turn_down[0] + 8, 40);
//                     }
//                 }
//                 if (flag_find_huan_rightdown_point == 0 && huandaotype != 3)
//                 {
//                     if (huandao_memory != 4)
//                     {
//                         findhuan_rightmiddle_point(3, 46);
//                     }
//                     if (huandao_memory == 4)
//                     {
//                         findhuan_rightmiddle_point(3, 46);
//                     }
//                 }
//                 if (flag_find_huan_rightdown_point == 1 && (right_turn_down[0] + 8) < 45 && huandaotype == 3) // 右中 下找到
//                 {
//                     if (huandao_memory != 4)
//                     {
//                         findhuan_rightmiddle_point(right_turn_down[0] + 8, 46);
//                     }
//                     if (huandao_memory == 4)
//                     {
//                         findhuan_rightmiddle_point(right_turn_down[0] + 8, 40);
//                     }
//                 }
//                 if (flag_find_huan_rightdown_point == 0 && huandaotype == 3)
//                 {
//                     if (huandao_memory != 4)
//                     {
//                         findhuan_rightmiddle_point(3, 46);
//                     }
//                     if (huandao_memory == 4)
//                     {
//                         findhuan_rightmiddle_point(3, 40);
//                     }
//                 }

//                 if (flag_find_huan_leftdown_point == 1 && (left_turn_down[0] + 3) < 45) // 左中 下找到
//                 {
//                     findhuan_leftmiddle_point(left_turn_down[0] + 3, 45);
//                 }
//                 if (flag_find_huan_leftdown_point == 0)
//                 {
//                     findhuan_leftmiddle_point(3, 45);
//                 }
//                 if (flag_find_huan_rightmiddle_point == 1 && (right_turn_middle[0] + 2) < 50)

//                 {
//                     findhuan_rightup_point(right_turn_middle[0] + 2, 50, ROUNDISLAND);
//                 } // 右上
//                 if (flag_find_huan_rightmiddle_point == 0)
//                 {
//                     findhuan_rightup_point(25, 50, ROUNDISLAND);
//                 }
//                 if (flag_find_huan_leftmiddle_point == 1 && (left_turn_middle[0] + 2) < 50) // 左上
//                 {
//                     findhuan_leftup_point(left_turn_middle[0] + 2, 50, ROUNDISLAND);
//                 }
//                 if (flag_find_huan_leftmiddle_point == 0)
//                 {
//                     findhuan_leftup_point(25, 50, ROUNDISLAND);
//                 }

//                 if (huandao_memory == 4)
//                 {
//                     flag_find_huan_rightup_point = 0;
//                     findhuan_rightup_point44(20, 40, ROUNDISLAND);
//                 } // 环岛找拐点结束

//                 if (huandao_memory == 7)
//                 {
//                     flag_find_huan_leftout_point = 0;
//                     findhuan_leftout_point(5, 35, ROUNDISLAND);
//                 }

//                 int begin = 0;
//                 for (i = 3; i < 50; i++)
//                 {
//                     if (R_black[i] == 0 && R_black[i + 1] == 0 && R_black[i + 2] != 0)
//                     {
//                         begin = i + 2;
//                         break;
//                     }
//                 }

//                 if (huandao_memory == 7 && begin > 3)
//                 {
//                     flag_find_huan_rightup_point = 0;
//                     findhuan_rightup_point77(3, begin + 5, ROUNDISLAND);
//                 }
//                 for (i = 0; i < 12; i++)
//                 {
//                     if (R_black[i] <= 4)
//                     {
//                         Rhave0 = 1;
//                         break;
//                     }
//                     Rhave0 = 0;
//                 }
//                 if (huandao_memory == 4)
//                 {
//                     if (Rhave0 == 1)
//                     {
//                         flag_find_huan_rightmiddle_point = 0;
//                         findhuan_rightmiddle_point(3, 30);
//                     }
//                     if (Rhave0 == 0)
//                     {
//                         flag_find_huan_rightmiddle_point = 0;
//                         findhuan_rightmiddle_point(3, 20);
//                     }
//                 }
//                 int hang;
//                 int end = 0;
//                 for (hang = 1; hang < 69; hang++) // 去掉杂点
//                 {

//                     if (LCenter[hang] == 0 && LCenter[hang + 1] == 0)
//                     {

//                         end = hang - 1;
//                         break;
//                     }
//                     if (My_Abs(LCenter[hang], LCenter[hang + 1]) > 20 && hang > 5)
//                     {

//                         end = hang - 1;
//                         break;
//                     }
//                 }
//                 if (end > 10)
//                 {
//                     advanced_regression(1, end - 4, end - 3, end - 2, end);
//                     leftk = parameterB;
//                 }
//                 regression(1, break_hangshu - 9, break_hangshu - 7);

//                 if (huandao_memory <= 8)
//                 {
//                     /*******行驶到环岛出口或者在行驶到环岛出口前********/
//                     if (huandao_memory <= 3)
//                     { //    30

//                         if (huandao_memory == 2)
//                         {
//                             if ((R_black[10] <= 8))
//                             {
//                                 //      SetText("行驶到环岛出口（没进环岛呢）");
//                                 huandao_memory = 3;
//                                 huandao_procedure_variable = 1;
//                             }
//                         }
//                         else if (huandao_memory == 0 && right_turn_down[0] > 3 && right_turn_down[0] <= 40 &&
//                                  flag_find_huan_rightdown_point == 1 &&
//                                  (calkuan[26] > 80 || calkuan[27] > 80 || calkuan[28] > 80 || calkuan[29] > 80 || calkuan[30] > 80 || calkuan[31] > 80 || calkuan[32] > 80 || calkuan[33] > 80 || calkuan[34] > 80 || calkuan[35] > 80 || calkuan[36] > 80) && truepianfangflag == 1 && continueleft == 1 && huandao_memoryforleft == 0 && leftk >= -5 && break_hangshu >= 37)
//                         {
//                             huandao_flag = 1;
//                             huandao_flag_R_L = 2;
//                             huandao_memory = 1;
//                             crossrecord = 0;
//                             crossderecord = 0;
//                             cross31 = 0;
//                             //   SetText("行驶到环岛出口前（没进环岛呢）(远)");
//                         }

//                         // 快速进入右边环岛3
//                         else if ((huandao_memory == 0 || huandao_memory == 1) && ((right_turn_middle[0] > 3 && right_turn_middle[0] <= 32 && huandaotype != 6 && huandaotype != 1) || (right_turn_middle[0] > 3 && right_turn_middle[0] <= 39 && huandaotype == 6) || (right_turn_middle[0] > 3 && right_turn_middle[0] <= 35 && huandaotype == 1)) &&
//                                  flag_find_huan_rightmiddle_point == 1 && break_hangshu >= 37 && (continueleft == 1 && huandao_memoryforleft == 0) && (((L_black[6] <= 160 && R_black[6] <= 10 && L_black[7] <= 160 && R_black[7] <= 10 && L_black[8] <= 160 && R_black[8] <= 10 && L_black[9] <= 160 && R_black[9] <= 10) || ((L_black[6] - R_black[6] > 165 && L_black[7] - R_black[7] > 165 && L_black[8] - R_black[8] > 165 && L_black[9] - R_black[9] > 165))) || ((L_black[6] <= 150 && R_black[6] <= 1 && L_black[7] <= 150 && R_black[7] <= 1 && L_black[8] <= 150 && R_black[8] <= 1 && L_black[9] <= 160 && R_black[9] <= 1))) && truepianfangflag == 1 && right_turn_middle[0] <= break_hangshu - 8) /*&&  */

//                         {
//                             huandao_flag = 1;
//                             huandao_flag_R_L = 2;
//                             huandao_memory = 3;
//                             crossrecord = 0;
//                             crossderecord = 0;
//                             cross31 = 0;
//                             //  SetText("break_hangshu  "+ break_hangshu);
//                             // SetText("right_turn_middle[0]  "+ right_turn_middle[0]);
//                             //  SetText("快速进入右边环岛3");
//                         }
//                         else if (huandao_memory == 3 && right_turn_middle[0] >= 0 && right_turn_middle[0] <= 20 && flag_find_huan_rightmiddle_point == 1 && huandaotype == 6)
//                         {
//                             huandao_flag = 1;
//                             huandao_flag_R_L = 2;
//                             flag_rukou = 1;
//                             huandao_memory = 4;
//                             // SetText("行驶到环岛出口前（没进环岛呢）(远)11");
//                         }
//                         else if (huandao_memory == 3 && right_turn_middle[0] >= 0 && right_turn_middle[0] <= 20 && flag_find_huan_rightmiddle_point == 1 && huandaotype == 4)
//                         {
//                             huandao_flag = 1;
//                             huandao_flag_R_L = 2;
//                             flag_rukou = 1;
//                             huandao_memory = 4;
//                             // SetText("行驶到环岛出口前（没进环岛呢）(远)22");
//                         }
//                         else if (huandao_memory == 3 && right_turn_middle[0] >= 0 && right_turn_middle[0] <= 20 && flag_find_huan_rightmiddle_point == 1 && huandaotype == 3)
//                         {
//                             huandao_flag = 1;
//                             huandao_flag_R_L = 2;
//                             flag_rukou = 1;
//                             huandao_memory = 4;
//                             //  SetText("行驶到环岛出口前（没进环岛呢）(远)22");
//                         }
//                         else if (huandao_memory == 3 && right_turn_middle[0] >= 0 && right_turn_middle[0] <= 18 && flag_find_huan_rightmiddle_point == 1 && huandaotype == 2)
//                         {
//                             huandao_flag = 1;
//                             huandao_flag_R_L = 2;
//                             flag_rukou = 1;
//                             huandao_memory = 4;
//                             //   SetText("行驶到环岛出口前（没进环岛呢）(远)33");
//                         }
//                         else if (huandao_memory == 3 && right_turn_middle[0] >= 0 && right_turn_middle[0] <= 18 && flag_find_huan_rightmiddle_point == 1 && huandaotype == 1)
//                         {
//                             huandao_flag = 1;
//                             huandao_flag_R_L = 2;
//                             flag_rukou = 1;
//                             huandao_memory = 4;
//                             // SetText("行驶到环岛出口前（没进环岛呢）(远)444");
//                         }
//                         if (huandao_memory == 1 && ((right_turn_down[0] <= 15 && right_turn_down[0] > 0 && flag_find_huan_rightdown_point == 1) || (right_turn_middle[0] <= 37 && right_turn_middle[0] > 0 && flag_find_huan_rightmiddle_point == 1 && flag_find_huan_rightdown_point == 0)) && break_hangshu >= 35)
//                         {
//                             huandao_flag = 1;
//                             huandao_flag_R_L = 2;
//                             huandao_memory = 2;
//                             //  SetText("行驶到环岛出口前（没进环岛呢）(远)555");
//                         }
//                     }
//                     else if (huandao_memory == 4 && break_hangshu <= 31 && huandaotype != 3 && ((L_black[end] - R_black[end] < 75 && L_black[end] - R_black[end] != 0) || (L_black[end - 1] - R_black[end - 1] < 75 && L_black[end - 1] - R_black[end - 1] != 0) || (L_black[end - 2] - R_black[end - 2] < 75 && L_black[end - 2] - R_black[end - 2] != 0)) && huandaotype != 3 && huandaotype != 6)
//                     {
//                         huandao_flag = 1;
//                         flag_rukou2 = 1;
//                         huandao_memory = 5;
//                         // SetText("行驶到环岛入口（没进环岛呢）");
//                     }
//                     else if (huandao_memory == 4 && break_hangshu <= 31 && huandaotype == 3 && ((L_black[end] - R_black[end] < 75 && L_black[end] - R_black[end] != 0) || (L_black[end - 1] - R_black[end - 1] < 75 && L_black[end - 1] - R_black[end - 1] != 0) || (L_black[end - 2] - R_black[end - 2] < 75 && L_black[end - 2] - R_black[end - 2] != 0)))
//                     {
//                         huandao_flag = 1;
//                         flag_rukou2 = 1;
//                         huandao_memory = 5;
//                         // SetText("行驶到环岛入口（没进环岛呢）");
//                     }
//                     else if (huandao_memory == 4 && break_hangshu <= 35 && huandaotype == 6 && ((L_black[end] - R_black[end] < 75 && L_black[end] - R_black[end] != 0) || (L_black[end - 1] - R_black[end - 1] < 75 && L_black[end - 1] - R_black[end - 1] != 0) || (L_black[end - 2] - R_black[end - 2] < 75 && L_black[end - 2] - R_black[end - 2] != 0)))
//                     {
//                         huandao_flag = 1;
//                         flag_rukou2 = 1;
//                         huandao_memory = 5;
//                         // SetText("行驶到环岛入口（没进环岛呢）");
//                     }
//                     else if (huandao_memory == 5 && break_hangshu <= 31 && huandaotype != 6)
//                     {
//                         huandao_flag = 1;
//                         huandao_memory = 6;
//                         flag_rukou = 3;
//                     }
//                     else if (huandao_memory == 5 && break_hangshu <= 35 && huandaotype == 6)
//                     {
//                         huandao_flag = 1;
//                         huandao_memory = 6;
//                         flag_rukou = 3;
//                     }
//                     else if (huandao_memory == 6 && left_turn_down[0] >= 1 && left_turn_down[0] <= 22 && flag_find_huan_leftdown_point == 1 && left_turn_down[1] >= 20)
//                     {
//                         huandao_flag = 1;
//                         huandao_memory = 7;
//                         // SetText("出环了");
//                     }
//                     else if (huandao_memory == 7 && flag_find_huan_rightup_point == 1 && parameterB <= 2 && (flag_find_huan_leftout_point == 0 || left_turn_out[0] >= 30) && huandaotype != 6)
//                     {
//                         huandao_flag = 1;
//                         huandao_memory = 8;
//                     }
//                     else if (huandao_memory == 7 && flag_find_huan_rightup_point == 1 && parameterB <= 2 && (flag_find_huan_leftout_point == 0 || left_turn_out[0] >= 30) && huandaotype == 6 && right_turn_down[0] >= 20)
//                     {
//                         huandao_flag = 1;
//                         huandao_memory = 8;
//                     }
//                     else if (huandao_memory == 8 && rightflag[15] == 1 && rightflag[16] == 1 && rightflag[17] == 1 && (continueright == 1 || kuleftdownflag == 1 || kurightdownflag == 1 || kuleftupflag == 1 || kurightupflag == 1) && R_black[10] != 0 && R_black[11] != 0)
//                     {
//                         huandao_memory = 9;
//                         huandao_memory = 0;
//                         huandao_flag = 0;
//                         huandao_procedure_variable = 0;
//                     }
//                 }
//                 R_mend_leftline_right_island((int)huandao_memory, (int)huandao_flag_R_L);
//                 R_mend_rightline_right_island((int)huandao_memory, (int)huandao_flag_R_L);
//                 if (huandao_memory == 5 || huandao_memory == 6)
//                 {
//                     advanced_regression(1, 6, 8, 10, 12);

//                     if (parameterB == 0)
//                     {
//                         advanced_regression(1, 0, 2, right_turn_up[0], right_turn_up[0] + 1);
//                         leftlinefuzhi(parameterA, parameterB, 0, 60);
//                     }
//                 }
//                 last_memory = huandao_memory;
//                 int j = 0;
//                 if (huandao_memory <= 8 && huandao_memory != 0)
//                 {
//                     for (j = 0; j < 70; j++)
//                     {

//                         LCenter[j] = (unsigned char)((L_black[j] + R_black[j]) / 2);
//                     }
//                 }
//             }
//         }
//     }
// }
// // 右环岛主体结束