#include "track.h"
#include "Servo.h"

uint8_t x1 = 0;
uint8_t x2 = 0;
uint8_t x3 = 0;
uint8_t x4 = 0;
uint8_t x5 = 0;
uint8_t x6 = 0;
uint8_t x7 = 0;
uint8_t x8 = 0;

void Track_Init(void)
{
    gpio_init(TRACK_READPORT_0, GPO, 0, GPO_PUSH_PULL);
    gpio_init(TRACK_READPORT_1, GPO, 0, GPO_PUSH_PULL);
    gpio_init(TRACK_READPORT_2, GPO, 0, GPO_PUSH_PULL);
    gpio_init(TRACK_OUTPUT, GPI, 0, GPI_PULL_UP);
}

void Track_GPIO_test(void)
{
    Track_Read_All();

    tft180_show_int(0, 32, x1, 1);
    tft180_show_int(0, 48, x2, 1);
    tft180_show_int(0, 64, x3, 1);
    tft180_show_int(0, 80, x4, 1);

    tft180_show_int(60, 16, x5, 1);
    tft180_show_int(60, 48, x6, 1);
    tft180_show_int(60, 64, x7, 1);
    tft180_show_int(60, 80, x8, 1);
}

static void Track_Select_Channel(uint8_t ad2, uint8_t ad1, uint8_t ad0)
{
    gpio_set_level(TRACK_READPORT_2, ad2);
    gpio_set_level(TRACK_READPORT_1, ad1);
    gpio_set_level(TRACK_READPORT_0, ad0);
}

void Track_Read_All(void)
{
    Track_Select_Channel(0, 0, 0);
    system_delay_us(50);
    x1 = gpio_get_level(TRACK_OUTPUT);

    Track_Select_Channel(0, 0, 1);
    system_delay_us(50);
    x2 = gpio_get_level(TRACK_OUTPUT);

    Track_Select_Channel(0, 1, 0);
    system_delay_us(50);
    x3 = gpio_get_level(TRACK_OUTPUT);

    Track_Select_Channel(0, 1, 1);
    system_delay_us(50);
    x4 = gpio_get_level(TRACK_OUTPUT);

    Track_Select_Channel(1, 0, 0);
    system_delay_us(50);
    x5 = gpio_get_level(TRACK_OUTPUT);

    Track_Select_Channel(1, 0, 1);
    system_delay_us(50);
    x6 = gpio_get_level(TRACK_OUTPUT);

    Track_Select_Channel(1, 1, 0);
    system_delay_us(50);
    x7 = gpio_get_level(TRACK_OUTPUT);

    Track_Select_Channel(1, 1, 1);
    system_delay_us(50);
    x8 = gpio_get_level(TRACK_OUTPUT);
}

void CONTRAL1(void)
{
    // 仅在“正常前进”状态下检测 x4、x5 同时为低电平时触发第一次停车等待
    if (control1_state == 0)
    {
        if (x4 == GPIO_LOW && x5 == GPIO_LOW)
        {
            Servo_Ctrl(SERVO_MOTOR_MID);
            control1_stop_flag = 1;                    // 标记检测到停止信号（可用于UI显示）
            control1_state = 1;                        // 进入停车等待状态
            control1_stop_time = timer_get(GPT_TIM_1); // 记录停车时间（ms）
        }
    }
}

// 使用舵机转向（后轮舵机）配合前轮驱动实现 90 度转弯
// dir = 0 -> 左转
// dir = 1 -> 右转
// 实现：先把舵机转到极限角度，再前驱行驶固定时间，最后复位舵机并停止
// 转向驱动参数
#define TURN_SERVO_DURATION_MS 1500
#define TURN_SERVO_SETTLE_MS 200
// 转向时前轮驱动输出占最大 PWM 的百分比（1-100），根据实际硬件调整
// 增加转向驱动输出以确保电机能可靠启动
#define TURN_SPEED_PERCENT 10
void Turn90(uint8_t dir, uint32_t turn_ms)
{
    uint64_t start;

    // 设置舵机到转向极限
    if (dir == 0)
    {
        // 左转
        Servo_Ctrl(SERVO_MOTOR_L_MAX);
    }
    else
    {
        // 右转
        Servo_Ctrl(SERVO_MOTOR_R_MAX);
    }

    // 等待舵机到位
    start = timer_get(GPT_TIM_1);
    while ((timer_get(GPT_TIM_1) - start) < TURN_SERVO_SETTLE_MS)
        ;

    // 前轮驱动前进，完成转弯弧线
    gpio_set_level(MOTOR1_DIR, MOTOR1_FORWARD_DIR_LEVEL);
    gpio_set_level(MOTOR2_DIR, MOTOR2_FORWARD_DIR_LEVEL);
    // 使用基于 MOTOR_PWM_MAX 的百分比输出（避免使用过小常量）
    int turn_pwm = (int)((MOTOR_PWM_MAX * TURN_SPEED_PERCENT) / 100);
    pwm_set_duty(MOTOR1_PWM, turn_pwm);
    pwm_set_duty(MOTOR2_PWM, turn_pwm);

    start = timer_get(GPT_TIM_1);
    while ((timer_get(GPT_TIM_1) - start) < turn_ms)
        ;

    // 停车并复位舵机
    pwm_set_duty(MOTOR1_PWM, 0);
    pwm_set_duty(MOTOR2_PWM, 0);
    Servo_Ctrl(SERVO_MOTOR_MID);
}