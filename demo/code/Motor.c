#include "Motor.h"

void Motor_Init()
{
    // 电机初始化
    pwm_init(MOTOR1_PWM, 17000, 0);                       // PWM 通道初始化频率 17KHz 占空比初始为 0
    gpio_init(MOTOR1_DIR, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO 初始化为输出 默认上拉输出高
    pwm_init(MOTOR2_PWM, 17000, 0);                       // PWM 通道初始化频率 17KHz 占空比初始为 0
    gpio_init(MOTOR2_DIR, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO 初始化为输出 默认上拉输出高
}

void Motor_Setspeed(int pwm)
{
    pwm_set_duty(MOTOR1_PWM, pwm);
    pwm_set_duty(MOTOR2_PWM, pwm);
}
