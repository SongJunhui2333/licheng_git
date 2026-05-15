#include "Servo.h"

void Servo_Init()
{
    pwm_init(SERVO_MOTOR_PWM, SERVO_MOTOR_FREQ, SERVO_MOTOR_DUTY(SERVO_MOTOR_MID));
}

void Servo_Ctrl(float duty)
{
    float tmp_duty = duty;
    // 限幅
    tmp_duty = MAX(tmp_duty, MIN(SERVO_MOTOR_L_MAX, SERVO_MOTOR_R_MAX));
    tmp_duty = MIN(tmp_duty, MAX(SERVO_MOTOR_L_MAX, SERVO_MOTOR_R_MAX));
    pwm_set_duty(SERVO_MOTOR_PWM, SERVO_MOTOR_DUTY(tmp_duty));
}

void Servo_Ctrl_Loop(float mid_err)
{
    float u;
    u = PidLocCtrl(&servo_pid, mid_err, 0.5f); // 计算PID输出
    float tmp_duty = SERVO_MOTOR_MID - SERVO_DIR * u;
    // 限幅
    tmp_duty = MAX(tmp_duty, MIN(SERVO_MOTOR_L_MAX, SERVO_MOTOR_R_MAX));
    tmp_duty = MIN(tmp_duty, MAX(SERVO_MOTOR_L_MAX, SERVO_MOTOR_R_MAX));
    pwm_set_duty(SERVO_MOTOR_PWM, SERVO_MOTOR_DUTY(tmp_duty)); // 控制打角
}

float mid_errsum(int line_start, float lines)
{
    int i;
    float err = 0;
    // 常规误差
    for (i = line_start; i <= line_start + lines; i++)
    {
        err += (MT9V03X_W / 2 - mid_line[i]); // 计算误差和
    }
    err = err / lines;
    return err; // 注意此处，误差有正负，还有小数，注意数据类型
}
