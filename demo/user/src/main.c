#include "main.h" // 所有引脚信息更改在main.h里改宏

// *************************** 例程硬件连接说明 ***************************
/*

            连接好电机、主板、驱动板、摄像头、编码器
            (请确保全部正确连接后在使用此例程，所有外设均会使用)
*/
// *************************** 例程测试说明 ***************************
/*
            本例程包含功能：
            1.初始化摄像头并采集图像进行大津法二值化
            2.初始化屏幕并显示关键信息
            3.根据偏差计算最基本的舵机打角
            4.运用最基本pid参数和pid算法使电机旋转 (pid参数在pid.c中)


            注意！注意！注意！注意！
            注意！注意！注意！注意！
            注意！注意！注意！注意！

            1.请仔细阅读main.h的所有注释，确保所有宏都和自己的硬件连接对应
            2.若屏幕显示二值化后的图像有缺陷，可能不能正常走直线，
            可以自行更改二值化算法或找光环境良好且均匀的环境测试例程（例程不提供更多的二值化算法）

*/

// **************************** 代码区域 ****************************

// 舵机动作角度中值
// const float servo_motor_duty_middle = (SERVO_MOTOR_R_MAX + SERVO_MOTOR_L_MAX) / 2.f;
const float servo_motor_duty_middle = SERVO_MOTOR_MID;

void Init()
{

    // 初始化flash, 储存参数. 一个扇区有8页, 一页可以储存4096字节, 一个参数占4个字节, 因此一页最多只能存64个参数
    flash_init();

    // tft180_set_dir(TFT180_CROSSWISE);                                           // 需要先横屏 不然显示不下
    tft180_init();

    // mt9v03x摄像头初始化
    while (1)
    {
        if (mt9v03x_init())
            tft180_show_string(0, 16, "mt9v03x reinit.");
        else
            break;
        system_delay_ms(1000);
    }
    tft180_show_string(0, 16, "init success.");

    // 编码器初始化
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B); // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B); // 初始化编码器模块与引脚 正交解码编码器模式

    Motor_Init(); // 电机初始化

    // 舵机初始化
    Servo_Init();

    // PID初始化
    My_Pid_Init();
    Servo_Pid_Init();

    // 定时器初始化要放在最后

    // 定时器0初始化，5ms可调
    // 定时器0中断用于编码器读取与PID计算
    pit_ms_init(PIT_CH0, 5);

    // 定时器1初始化，20ms可调
    // 定时器1中断用于舵机控制
    pit_ms_init(PIT_CH1, 20);

    // // 定时器1初始化
    // pit_ms_init(PIT_CH1, 5);
    // // 按键初始化
    // key_init(5);
}

uint8_t image[MT9V03X_H][MT9V03X_W];
short hist_gram[256];
uint8_t left_line[MT9V03X_H];  // 左边线位置
uint8_t mid_line[MT9V03X_H];   // 中线位置
uint8_t right_line[MT9V03X_H]; // 右边线位置
float offset;                  // 定义偏离中线误差
unsigned char threshold = 0;   // 二值化阈值

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); // 不可删除
    debug_init();                  // 调试端口初始化
    system_delay_ms(300);          // 等待主板其他外设上电完成

    Init(); // 初始化操作
    interrupt_global_enable(0);

    while (1)
    {
        //  mt9v03x摄像头
        if (mt9v03x_finish_flag)
        {
            if (mt9v03x_finish_flag)
            {
                // 另寻空间将图像保存下来，以免产生因读写冲突带来的未知后果
                memcpy((uint8_t *)image, (uint8_t *)mt9v03x_image, sizeof(uint8_t) * MT9V03X_H * MT9V03X_W);
                // 获取直方图
                get_hist_gram((uint8_t *)image, MT9V03X_H, MT9V03X_W, hist_gram);
                // 计算大津法阈值
                threshold = get_threshold_otsu(hist_gram);

                // 二值化处理
                binaryzation_process((uint8_t *)image, MT9V03X_H, MT9V03X_W, threshold);
                // 边界线寻找
                // auxiliary_process((uint8_t *)image, MT9V03X_H, MT9V03X_W, threshold, left_line, mid_line, right_line);

                // 图像处理
                imageProcess((uint8_t *)image);

                for (uint8_t _i = 0; _i < MT9V03X_H; ++_i)
                {
                    // 将边界线也显示出来
                    image[_i][left_line[_i]] = 0;  // 显示左边线
                    image[_i][mid_line[_i]] = 0;   // 显示中线
                    image[_i][right_line[_i]] = 0; // 显示右边线
                }

                // 显示图像
                tft180_displayimage03x((uint8_t *)image, 125, 100);

                // 显示关键信息
                // tft180_show_int(0, 130, encoder_data_1, 3);
                // tft180_show_int(50, 130, encoder_data_2, 3);
                // tft180_show_float(0, 100, speed_pwm_l, 4, 2);
                // tft180_show_float(50, 100, speed_pwm_r, 4, 2);

                tft180_show_int(50, 140, Zebra_Stripes_Flag, 1); // 显示斑马线停止行

                // 显示阈值
                tft180_show_int(90, 140, threshold, 3);

                // 显示偏差
                tft180_show_float(0, 140, offset, 6, 2);

                // 处理完一帧图像后务必把该标志位清零！
                mt9v03x_finish_flag = 0;
            }
        }
    }
}
