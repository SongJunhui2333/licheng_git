#include "main.h" // 所有引脚信息更改在main.h里改宏
#include <algorithm>

int64_t time_count = 0;

uint8_t control1_stop_flag = 0;  // 基础1小车启停状态（0启动，1停止）
uint8_t control1_state = 0;      // 基础1小车状状态（0停止前，1停止后）
uint64_t control1_stop_time = 0; // 基础1小车停止时间
uint64_t control1_back_time = 0; // 基础1小车停止后向后走的时间，单位ms

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

    // 编码器初始化
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B); // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B); // 初始化编码器模块与引脚 正交解码编码器模式

    Motor_Init(); // 电机初始化

    // 舵机初始化
    Servo_Init();

    // PID初始化
    My_Pid_Init();
    Servo_Pid_Init();

    // 循迹模块初始化
    Track_Init();

    timer_init(GPT_TIM_1, TIMER_MS);

    // 声光模块初始化
    light_sound_init();

    // 定时器初始化要放在最后

    // 定时器0初始化，5ms可调
    // 定时器0中断用于编码器读取与PID计算
    pit_ms_init(PIT_CH0, 5);

    // 定时器1初始化，20ms可调
    // 定时器1中断用于舵机控制
    pit_ms_init(PIT_CH1, 20);

    pit_ms_init(PIT_CH2, 70);

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

#define CONTROL1_STOP_WAIT_MS 1000 // 停止等待时间1秒
#define CONTROL1_BACK_RUN_MS 2200

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); // 不可删除
    debug_init();                  // 调试端口初始化
    system_delay_ms(300);          // 等待主板其他外设上电完成

    Init(); // 初始化操作
    interrupt_global_enable(0);

    timer_start(GPT_TIM_1); // 启动定时器

    while (1)
    {

        if (mt9v03x_finish_flag)
        {

            // 显示关键信息
            // tft180_show_int(0, 130, encoder_data_1, 3);
            // tft180_show_int(50, 130, encoder_data_2, 3);
            // tft180_show_float(0, 100, speed_pwm_l, 4, 2);
            // tft180_show_float(50, 100, speed_pwm_r, 4, 2);

            // tft180_show_int(50, 140, count, 1); // 显示斑马线停止行

            // 显示关键信息
            tft180_show_int(0, 100, encoder_data_1, 3);
            tft180_show_int(0, 115, encoder_data_2, 3);
            tft180_show_float(0, 130, speed_pwm, 4, 2);
        }

        uint64_t now_ms = timer_get(GPT_TIM_1);

        if (control1_state == 0)
        {
            speed_pwm = PidLocCtrl(&speed_pid_l, speed_target + 2, 1.f);
            pwm_set_duty(MOTOR1_PWM, MAX(speed_pwm, 0));
            gpio_set_level(MOTOR1_DIR, MOTOR1_FORWARD_DIR_LEVEL);

            speed_pwm = PidLocCtrl(&speed_pid_r, speed_target + 2, 1.f);
            pwm_set_duty(MOTOR2_PWM, MAX(speed_pwm, 0));
            gpio_set_level(MOTOR2_DIR, MOTOR2_FORWARD_DIR_LEVEL);
        }
        else if (control1_state == 1)
        {
            pwm_set_duty(MOTOR1_PWM, 0);
            pwm_set_duty(MOTOR2_PWM, 0);

            if (now_ms - control1_stop_time >= CONTROL1_STOP_WAIT_MS)
            {
                control1_state = 2;
                control1_back_time = now_ms;
                control1_stop_flag = 0;
            }
        }
        else if (control1_state == 2)
        {
            if (now_ms - control1_back_time < CONTROL1_BACK_RUN_MS)
            {
                speed_pwm = PidLocCtrl(&speed_pid_l, speed_target + 2, 1.f);
                pwm_set_duty(MOTOR1_PWM, MAX(speed_pwm, 0));
                gpio_set_level(MOTOR1_DIR, !MOTOR1_FORWARD_DIR_LEVEL);

                speed_pwm = PidLocCtrl(&speed_pid_r, speed_target + 2, 1.f);
                pwm_set_duty(MOTOR2_PWM, MAX(speed_pwm, 0));
                gpio_set_level(MOTOR2_DIR, !MOTOR2_FORWARD_DIR_LEVEL);
            }
            else
            {
                control1_state = 3;
                pwm_set_duty(MOTOR1_PWM, 0);
                pwm_set_duty(MOTOR2_PWM, 0);
            }
        }
        else
        {
            pwm_set_duty(MOTOR1_PWM, 0);
            pwm_set_duty(MOTOR2_PWM, 0);
        }

        if ((float)timer_get(GPT_TIM_1) / 1000.0f - control1_back_time > 4)
        {
            pwm_set_duty(MOTOR1_PWM, 0);
            pwm_set_duty(MOTOR2_PWM, 0);
        }

        CONTRAL1();
    }
}