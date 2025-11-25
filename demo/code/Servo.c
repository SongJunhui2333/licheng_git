#include "Servo.h"

void Servo_Init()
{
    pwm_init(SERVO_MOTOR_PWM, SERVO_MOTOR_FREQ, SERVO_MOTOR_DUTY(count));
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
    float duty;
    float tmp_duty;
    duty = PidLocCtrl(&servo_pid, mid_err, 0);
    tmp_duty = duty;
    // 限幅
    tmp_duty = MAX(tmp_duty, MIN(SERVO_MOTOR_L_MAX, SERVO_MOTOR_R_MAX));
    tmp_duty = MIN(tmp_duty, MAX(SERVO_MOTOR_L_MAX, SERVO_MOTOR_R_MAX));
    pwm_set_duty(SERVO_MOTOR_PWM, SERVO_MOTOR_DUTY(tmp_duty)); // 控制打角
}
