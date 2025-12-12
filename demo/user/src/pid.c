#include "pid.h"

// 电机PID初始化参数
float speed_KP = 50, speed_KI = 0, speed_KD = 0.0, speed_IMAX = 5000.0, speed_OUTMAX = 8000.0;
float speed_target = 70.0;
float speed_real = 0.0;
float speed_pwm_l = 0.0;
float speed_pwm_r = 0.0;

pid_param_t speed_pid_l; // 左电机PID
pid_param_t speed_pid_r; // 右电机PID

// 舵机PID初始化参数
float servo_KP = 0.40, servo_KI = 0.0, servo_KD = 0.0, servo_IMAX = 0.0, servo_OUTMAX = PWM_DUTY_MAX;
// 动态PID基础参数
float servo_kp_base = 0.3;
float servo_kd_base = 2.0;

pid_param_t servo_pid; // 舵机PID

/*******************************************************************************
 * 函 数 名         : My_Pid_Init
 * 函数功能         : PID初始化
 * 输    入         : 无
 * 输    出         : 无
 *******************************************************************************/
void My_Pid_Init(void)
{
    Pid_Param_Init(&speed_pid_l, speed_KP, speed_KI, speed_KD, speed_IMAX, speed_OUTMAX);
    Pid_Param_Init(&speed_pid_r, speed_KP, speed_KI, speed_KD, speed_IMAX, speed_OUTMAX);
}

/*******************************************************************************
 * 函 数 名         : Servo_Pid_Init
 * 函数功能         : 舵机PID初始化
 * 输    入         : 无
 * 输    出         : 无
 *******************************************************************************/
void Servo_Pid_Init(void)
{
    Pid_Param_Init(&servo_pid, servo_KP, servo_KI, servo_KD, servo_IMAX, servo_OUTMAX);
}

/*******************************************************************************
 * 函 数 名         : my_abs
 * 函数功能         : 取绝对值
 * 输    入         : val
 * 输    出         : float
 *******************************************************************************/
float My_abs(float val)
{
    if (val < 0)
        return -val;
    else
        return val;
}

void dynamic_pid_value_set(float mid_err)
{
    // 一次Kp
    servo_pid.kp = servo_kp_base + My_abs(mid_err) * 0.045;
    // 二次Kp
    // servo_pid.kp = servo_kp_base + mid_err * mid_err * 0.0015;
    servo_pid.ki = 0.0;
    servo_pid.kd = servo_kd_base;
}

/*******************************************************************************
 * 函 数 名         : Pid_Param_Init
 * 函数功能         : PID参数初始化
 * 输    入         : imax:积分项最大值
 * 输    出         : 无
 *******************************************************************************/
void Pid_Param_Init(pid_param_t *pid, float kp, float ki, float kd, float imax, float outmax)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->imax = imax;
    pid->out_p = 0;
    pid->out_i = 0;
    pid->out_d = 0;
    pid->out = 0;
    pid->outmax = outmax;
    pid->integrator = 0;
    pid->last_error = 0;
    pid->last_derivative = 0;
    pid->last_t = 0;
}

/*******************************************************************************
 * 函 数 名         : PidLocCtrl
 * 函数功能	   : 位置式PID控制
 * 输    入         : pid, error, t
 * 输    出         : float
 *******************************************************************************/
float PidLocCtrl(pid_param_t *pid, float error, float t)
{
    /* 累积误差 */
    pid->integrator += error;

    /* 误差限幅 */
    pid->out_i = constrain_float(pid->integrator, -pid->imax, pid->imax);

    pid->out_p = pid->kp * error;
    pid->out_i = pid->ki * t * pid->integrator;
    pid->out_d = pid->kd / t * (error - pid->last_error);

    pid->last_error = error;

    pid->out = pid->out_p + pid->out_i + pid->out_d;

    pid->out = constrain_float(pid->out, -pid->outmax, pid->outmax);

    return pid->out;
}

/*******************************************************************************
 * 函 数 名         : PidIncCtrl
 * 函数功能	   : 增量式PID控制
 * 输    入         : pid, error, t
 * 输    出         : float
 *******************************************************************************/
float PidIncCtrl(pid_param_t *pid, float error, float t)
{

    pid->out_p = pid->kp * (error - pid->last_error);
    pid->out_i = pid->ki * error * t;
    pid->out_d = pid->kd / t * ((error - pid->last_error) - pid->last_derivative);

    pid->last_derivative = error - pid->last_error;
    pid->last_error = error;

    pid->out += pid->out_p + pid->out_i + pid->out_d;

    pid->out = constrain_float(pid->out, -pid->outmax, pid->outmax);
    return pid->out;
}

/*******************************************************************************
 * 函 数 名         : constrain_float
 * 函数功能         : 浮点型数限幅
 * 输    入         : amt,low,high
 * 输    出         : float
 *******************************************************************************/
float constrain_float(float amt, float low, float high)
{
    return ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)));
}

/*******************************************************************************
 * 函 数 名         : constrain_short
 * 函数功能         : 短整型数限幅
 * 输    入         : amt,low,high
 * 输    出         : short
 *******************************************************************************/
short constrain_short(short amt, short low, short high)
{
    return ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)));
}
