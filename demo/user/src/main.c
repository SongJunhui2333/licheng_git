#include "main.h" // 所有引脚信息更改在main.h里改宏
#include "isr.h"
#include "track.h"

int64_t time_count = 0;

uint8_t control1_stop_flag = 0;                // 基础1小车启停状态（0启动，1停止）
uint8_t control1_state = 0;                    // 基础1小车状态（0停止前，1停止后，4等待声音启动）
volatile uint8_t control1_sound_triggered = 0; // 声音启动锁存标志
uint64_t control1_stop_time = 0;               // 基础1小车停止时间
uint64_t control1_back_time = 0;               // 基础1小车停止后向后走的时间，单位ms
uint64_t control1_finish_time = 0;             // 基础1小车第二次完全停止的时间，单位ms
static uint8_t control1_led_blink_step = 0;    // 第二次完全停止后蓝灯闪烁步数
static uint64_t control1_led_blink_time = 0;   // 蓝灯上一次切换的时间
static uint8_t control1_led_on = 0;            // 蓝灯当前状态

uint8_t control2_state = 0;

int16 control2_encoder_count = 0;

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
    gpio_set_level(SOUND_PIN_OUTPUT, GPIO_HIGH); // 蜂鸣器默认关闭，低电平触发
    control1_state = 4;                          // 上电后先等待声音启动信号
    control1_sound_triggered = 0;

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
#define CONTROL1_BACK_RUN_MS 2500
#define CONTROL1_LED_BLINK_INTERVAL_MS 250 // 蓝灯单次闪烁间隔
#define CONTROL1_LED_BLINK_STEPS 6         // 3次闪烁 = 6次电平切换

#define TRACK_SERVO_ADJUST_GAIN 0.5f // 循迹舵机修正系数，数值越大转向越明显

static uint64_t control2_get_encoder_distance(void)
{
    uint64_t left_count = (encoder_data_1 >= 0) ? (uint64_t)encoder_data_1 : (uint64_t)(-encoder_data_1);
    uint64_t right_count = (encoder_data_2 >= 0) ? (uint64_t)encoder_data_2 : (uint64_t)(-encoder_data_2);

    return (left_count + right_count) / 2;
}

static void control2_reset_encoder_count(void)
{
    encoder_clear_count(ENCODER_1);
    encoder_clear_count(ENCODER_2);
    encoder_data_1 = 0;
    encoder_data_2 = 0;
    control2_encoder_count = 0;
}

static void control2_stop(void)
{
    pwm_set_duty(MOTOR1_PWM, 0);
    pwm_set_duty(MOTOR2_PWM, 0);
    Servo_Ctrl(SERVO_MOTOR_MID);
}

static void control2_set_forward(int drive_pwm)
{

    Servo_Ctrl(SERVO_MOTOR_MID);
    gpio_set_level(MOTOR1_DIR, MOTOR1_FORWARD_DIR_LEVEL);
    gpio_set_level(MOTOR2_DIR, MOTOR2_FORWARD_DIR_LEVEL);
    pwm_set_duty(MOTOR1_PWM, drive_pwm);
    pwm_set_duty(MOTOR2_PWM, drive_pwm);
}

static void control2_set_right_turn(int turn_pwm)
{
    Servo_Ctrl(SERVO_MOTOR_R_MAX);
    gpio_set_level(MOTOR1_DIR, MOTOR1_FORWARD_DIR_LEVEL);
    gpio_set_level(MOTOR2_DIR, MOTOR2_FORWARD_DIR_LEVEL);
    pwm_set_duty(MOTOR1_PWM, turn_pwm);
    pwm_set_duty(MOTOR2_PWM, turn_pwm);
}

static void control2_apply_state(uint8_t state, int drive_pwm)
{
    switch (state)
    {
    case 0:
        control2_set_forward(drive_pwm);
        break;
    case 1:
        control2_set_right_turn(drive_pwm);
        break;
    case 2:
        control2_set_forward(drive_pwm);
        break;
    case 3:
        control2_set_right_turn(drive_pwm);
        break;
    case 4:
        control2_set_forward(drive_pwm);
        break;
    case 5:
        control2_set_right_turn(drive_pwm);
        break;
    case 6:
        control2_set_forward(drive_pwm);
        break;
    default:
        control2_stop();
        break;
    }
}

void car_task1(void)
{
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

        Track_Read_All(); // 读取循迹传感器状态到全局变量 x1-x8

        int8_t track_start_weight[] = {-12, -8, -4, -2, 2, 4, 8, 12}; // 轨迹权重数组，根据实际情况调整

        int8_t track_back_weight[] = {-6, -5, -4, -2, 2, 4, 5, 6}; // 轨迹权重数组，根据实际情况调整

        float servo_target = servo_motor_duty_middle;

        if (control1_state == 4)
        {
            gpio_set_level(SOUND_PIN_OUTPUT, GPIO_HIGH);
            pwm_set_duty(MOTOR1_PWM, 0);
            pwm_set_duty(MOTOR2_PWM, 0);

            if (control1_sound_triggered)
            {
                control1_sound_triggered = 0;
                control1_state = 0;
                control1_stop_flag = 0;
            }
        }
        else if (control1_state == 0)
        {
            gpio_set_level(SOUND_PIN_OUTPUT, GPIO_HIGH); // 正常行驶时关闭蜂鸣器

            float servo_error = x1 * track_start_weight[0] + x2 * track_start_weight[1] + x3 * track_start_weight[2] +
                                x4 * track_start_weight[3] + x5 * track_start_weight[4] + x6 * track_start_weight[5] +
                                x7 * track_start_weight[6] + x8 * track_start_weight[7];

            servo_target += servo_error * TRACK_SERVO_ADJUST_GAIN;

            Servo_Ctrl((uint16_t)servo_target);

            speed_pwm = PidLocCtrl(&speed_pid_l, speed_target + 2, 1.f);
            pwm_set_duty(MOTOR1_PWM, MAX(speed_pwm, 0));
            gpio_set_level(MOTOR1_DIR, MOTOR1_FORWARD_DIR_LEVEL);

            speed_pwm = PidLocCtrl(&speed_pid_r, speed_target + 2, 1.f);
            pwm_set_duty(MOTOR2_PWM, MAX(speed_pwm, 0));
            gpio_set_level(MOTOR2_DIR, MOTOR2_FORWARD_DIR_LEVEL);
        }
        else if (control1_state == 1)
        {
            gpio_set_level(SOUND_PIN_OUTPUT, GPIO_HIGH); // 第一次停止不响
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
            gpio_set_level(SOUND_PIN_OUTPUT, GPIO_HIGH); // 后退阶段关闭蜂鸣器

            if (now_ms - control1_back_time < CONTROL1_BACK_RUN_MS)
            {

                float servo_error = x1 * track_back_weight[0] + x2 * track_back_weight[1] + x3 * track_back_weight[2] +
                                    x4 * track_back_weight[3] + x5 * track_back_weight[4] + x6 * track_back_weight[5] +
                                    x7 * track_back_weight[6] + x8 * track_back_weight[7];

                servo_target += servo_error * TRACK_SERVO_ADJUST_GAIN;

                Servo_Ctrl((uint16_t)servo_target);

                speed_pwm = PidLocCtrl(&speed_pid_l, speed_target + 0.6, 1.f);
                pwm_set_duty(MOTOR1_PWM, MAX(speed_pwm, 0));
                gpio_set_level(MOTOR1_DIR, !MOTOR1_FORWARD_DIR_LEVEL);

                speed_pwm = PidLocCtrl(&speed_pid_r, speed_target + 0.6, 1.f);
                pwm_set_duty(MOTOR2_PWM, MAX(speed_pwm, 0));
                gpio_set_level(MOTOR2_DIR, !MOTOR2_FORWARD_DIR_LEVEL);
            }
            else
            {
                control1_state = 3;
                control1_finish_time = now_ms;
                control1_led_blink_step = 1;
                control1_led_blink_time = now_ms;
                control1_led_on = 1;
                gpio_set_level(BLUE_LED_PIN, GPIO_LOW); // 蓝灯低电平点亮
                pwm_set_duty(MOTOR1_PWM, 0);
                pwm_set_duty(MOTOR2_PWM, 0);
            }
        }
        else if (control1_state == 3)
        {
            pwm_set_duty(MOTOR1_PWM, 0);
            pwm_set_duty(MOTOR2_PWM, 0);

            if (now_ms - control1_finish_time < 1000)
            {
                gpio_set_level(SOUND_PIN_OUTPUT, GPIO_LOW); // 第二次完全停下后蜂鸣器响1秒
            }
            else
            {
                gpio_set_level(SOUND_PIN_OUTPUT, GPIO_HIGH); // 响完关闭
            }

            if (control1_led_blink_step < CONTROL1_LED_BLINK_STEPS &&
                now_ms - control1_led_blink_time >= CONTROL1_LED_BLINK_INTERVAL_MS)
            {
                control1_led_blink_time = now_ms;
                control1_led_on = !control1_led_on;
                gpio_set_level(BLUE_LED_PIN, control1_led_on ? GPIO_LOW : GPIO_HIGH);
                control1_led_blink_step++;
            }
        }
        else
        {
            gpio_set_level(SOUND_PIN_OUTPUT, GPIO_HIGH); // 其它状态保持关闭
            gpio_set_level(BLUE_LED_PIN, GPIO_HIGH);     // 其它状态保持关闭
            pwm_set_duty(MOTOR1_PWM, 0);
            pwm_set_duty(MOTOR2_PWM, 0);
        }

        if (control1_state >= 2 && (float)timer_get(GPT_TIM_1) / 1000.0f - control1_back_time > 4)
        {
            gpio_set_level(SOUND_PIN_OUTPUT, GPIO_HIGH);
            gpio_set_level(BLUE_LED_PIN, GPIO_HIGH);
            pwm_set_duty(MOTOR1_PWM, 0);
            pwm_set_duty(MOTOR2_PWM, 0);
        }

        CONTRAL1();
    }
}

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); // 不可删除
    debug_init();                  // 调试端口初始化
    system_delay_ms(300);          // 等待主板其他外设上电完成

    Init(); // 初始化操作
    interrupt_global_enable(0);

    timer_start(GPT_TIM_1); // 启动定时器

    // car_task1();

    // 四段直行与三次左转状态机
    // 计算 PWM 输出值
    int drive_pwm = (int)((MOTOR_PWM_MAX * CONTROL2_DRIVE_SPEED_PERCENT) / 100);
    int8_t control2_track_weight[] = {-8, -4, -2, -1, 1, 2, 4, 8};     // 轨迹权重数组，根据实际情况调整
    int8_t control2_track_weight_2[] = {-12, -8, -4, -2, 2, 4, 8, 12}; // 轨迹权重数组，根据实际情况调整

    control2_state = 0;
    control2_apply_state(control2_state, drive_pwm);
    control2_reset_encoder_count();

    while (1)
    {
        uint64_t now_ms = timer_get(GPT_TIM_1);
        uint64_t control2_encoder_distance = control2_get_encoder_distance();

        control2_encoder_count = (int16)control2_encoder_distance;

        if (control2_state == 2)
        {
            float servo_target = servo_motor_duty_middle;

            Track_Read_All(); // 读取循迹传感器状态到全局变量 x1-x8

            float servo_error = x1 * control2_track_weight[0] + x2 * control2_track_weight[1] +
                                x3 * control2_track_weight[2] + x4 * control2_track_weight[3] +
                                x5 * control2_track_weight[4] + x6 * control2_track_weight[5] +
                                x7 * control2_track_weight[6] + x8 * control2_track_weight[7];

            servo_target += servo_error * TRACK_SERVO_ADJUST_GAIN;
            Servo_Ctrl((uint16_t)servo_target);
        }
        if (control2_state == 6)
        {
            float servo_target = servo_motor_duty_middle;

            Track_Read_All(); // 读取循迹传感器状态到全局变量 x1-x8

            float servo_error = x1 * control2_track_weight_2[0] + x2 * control2_track_weight_2[1] +
                                x3 * control2_track_weight_2[2] + x4 * control2_track_weight_2[3] +
                                x5 * control2_track_weight_2[4] + x6 * control2_track_weight_2[5] +
                                x7 * control2_track_weight_2[6] + x8 * control2_track_weight_2[7];

            servo_target += servo_error * TRACK_SERVO_ADJUST_GAIN;
            Servo_Ctrl((uint16_t)servo_target);
        }

        switch (control2_state)
        {
        case 0:
            if (control2_encoder_distance >= CONTROL2_STRAIGHT1_ENCODER_TARGET)
            {
                control2_state = 1;
                control2_reset_encoder_count();
                control2_apply_state(control2_state, drive_pwm);
            }
            break;
        case 1:
            Track_Read_All();
            if (x3 == GPIO_HIGH || control2_encoder_distance >= CONTROL2_TURN1_ENCODER_TARGET)
            {
                control2_state = 2;
                control2_reset_encoder_count();
                control2_apply_state(control2_state, drive_pwm);
            }
            break;
        case 2:
            if (control2_encoder_distance >= CONTROL2_STRAIGHT2_ENCODER_TARGET)
            {

                control2_state = 3;
                control2_reset_encoder_count();
                control2_apply_state(control2_state, drive_pwm);
            }
            break;
        case 3:
            if (control2_encoder_distance >= CONTROL2_TURN2_ENCODER_TARGET)
            {
                control2_state = 4;
                control2_reset_encoder_count();
                control2_apply_state(control2_state, drive_pwm);
            }
            break;
        case 4:
            if (control2_encoder_distance >= CONTROL2_STRAIGHT3_ENCODER_TARGET)
            {
                control2_state = 5;
                control2_reset_encoder_count();
                control2_apply_state(control2_state, drive_pwm);
            }
            break;
        case 5:
            Track_Read_All();
            if (/*x4 == GPIO_HIGH ||*/ control2_encoder_distance >= CONTROL2_TURN3_ENCODER_TARGET)
            {
                control2_state = 6;
                control2_reset_encoder_count();
                control2_apply_state(control2_state, drive_pwm);
            }
            break;
        case 6:
            if (control2_encoder_distance >= CONTROL2_STRAIGHT4_ENCODER_TARGET)
            {

                control2_state = 7;
                control2_reset_encoder_count();
                control2_apply_state(control2_state, drive_pwm);
            }
            break;
        default:
            control2_stop();
            break;
        }
    }

    return 0;
}