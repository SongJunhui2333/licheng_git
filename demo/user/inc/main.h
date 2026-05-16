#include "img_process.h"
#include "zf_common_headfile.h"

// 编码器引脚信息
// 请确保两轮编码器前进时回传值都为正数，若回传为负请isr.c中encoder_get_count前修改正负号
// 左轮编码器
#define ENCODER_1 (QTIMER1_ENCODER1)
#define ENCODER_1_A (QTIMER1_ENCODER1_CH1_C0)
#define ENCODER_1_B (QTIMER1_ENCODER1_CH2_C1)
// 右轮编码器
#define ENCODER_2 (QTIMER1_ENCODER2)
#define ENCODER_2_A (QTIMER1_ENCODER2_CH1_C2)
#define ENCODER_2_B (QTIMER1_ENCODER2_CH2_C24)

// 电机参数设置
// !!! 请确保dir和pwm脚不要弄反了，否则电机会疯转 !!!
// 左轮电机
#define MOTOR1_PWM (PWM2_MODULE0_CHA_C6)
#define MOTOR1_DIR (C7)
// 左电机前进需要的DIR脚的电平 (GPIO_HIGH or GPIO_LOW) 请自行测试
#define MOTOR1_FORWARD_DIR_LEVEL (GPIO_HIGH)
// 右轮电机
#define MOTOR2_PWM (PWM2_MODULE1_CHA_C8)
#define MOTOR2_DIR (C9)
// 右电机前进需要的DIR脚的电平 (GPIO_HIGH or GPIO_LOW) 请自行测试
#define MOTOR2_FORWARD_DIR_LEVEL (GPIO_HIGH)
// 对电机输出进行限幅
#define MOTOR_PWM_MAX (10000)

// 摄像头参数设置
extern uint8_t image[MT9V03X_H][MT9V03X_W];
extern short hist_gram[256];
extern uint8_t left_line[MT9V03X_H];  // 左边线位置
extern uint8_t mid_line[MT9V03X_H];   // 中线位置
extern uint8_t right_line[MT9V03X_H]; // 右边线位置
extern unsigned char threshold;       // 二值化阈值

// 舵机参数设置
#define SERVO_MOTOR_PWM (PWM4_MODULE2_CHA_C30) // 定义主板上舵机对应引脚
#define SERVO_MOTOR_FREQ (200)                 // 定义主板上舵机频率  请务必注意范围 50-300
#define SERVO_MOTOR_L_MAX (112)                // 定义主板上舵机活动范围(左打方向的极限值) 角度 自行标定
#define SERVO_MOTOR_R_MAX (88)                 // 定义主板上舵机活动范围(右打方向的极限值) 角度 自行标定
#define SERVO_MOTOR_MID (100)                  // 定义舵机中值的角度（100是正的）
#define SERVO_DIR (SERVO_MOTOR_L_MAX > SERVO_MOTOR_R_MAX ? -1.f : 1.f) // 根据左右duty的大小自动决定舵机方向

extern float offset; // 定义偏离中线误差

// 循迹模块参数设置
#define TRACK_READPORT_0 (D14)
#define TRACK_READPORT_1 (D12)
#define TRACK_READPORT_2 (D15)
#define TRACK_OUTPUT (D13)

// 蓝色LED灯参数设置
// 低电平点亮
#define BLUE_LED_PIN (B9)

// 声光提示模块参数设置
#define SOUND_PIN_OUTPUT (B13) // 声音输出
#define SOUND_PIN_INPUT (B12)  // 声音输入

extern int64_t time_count; // 定义一个全局变量用于存储计时器的计数值

// 基础1变量
extern uint8_t control1_stop_flag;                // 基础1小车启停状态（0启动，1停止）
extern uint8_t control1_state;                    // 基础1小车状态（0停止前，1停止后，4等待声音启动）
extern volatile uint8_t control1_sound_triggered; // 声音启动锁存标志
extern uint64_t control1_stop_time;               // 基础1小车停止时间
extern uint64_t control1_back_time;               // 基础1小车停止后向后走的时间，单位ms
extern uint64_t control1_finish_time;             // 基础1小车第二次完全停止的时间，单位ms

// 以下宏在初步测试例程时不可更改，后面若需更改舵机算法可以自行更改
// ------------------ 舵机占空比计算方式 ------------------
//
// 舵机对应的 0-180 活动角度对应 控制脉冲的 0.5ms-2.5ms 高电平
//
// 那么不同频率下的占空比计算方式就是
// PWM_DUTY_MAX/(1000/freq)*(1+Angle/180) 在 50hz 时就是 PWM_DUTY_MAX/(1000/50)*(1+Angle/180)
//
// 那么 100hz 下 90度的打角 即高电平时间1.5ms 计算套用为
// PWM_DUTY_MAX/(1000/100)*(1+90/180) = PWM_DUTY_MAX/10*1.5
//
// ------------------ 舵机占空比计算方式 ------------------
#define SERVO_MOTOR_DUTY(x) ((float)PWM_DUTY_MAX / (1000.0 / (float)SERVO_MOTOR_FREQ) * (0.5 + (float)(x) / 90.0))

#if (SERVO_MOTOR_FREQ < 50 || SERVO_MOTOR_FREQ > 300)
#error "SERVO_MOTOR_FREQ ERROE!"
#endif

// ====================== 第一问核心参数 可调参数 ======================
#define FORWARD_SPEED 180  // 前进速度（原速度，稳定）
#define BACKWARD_SPEED 120 // 后退速度（降低！解决速度快跑偏）
#define SERVO_FORWARD 120  // 前进舵机修正幅度
#define SERVO_BACKWARD 60  // 后退舵机修正幅度（减半！解决角度过大）
#define STOP_TIME 1000     // 停止时间1秒(ms)
#define BACK_RUN_TIME 3000 // 后退运行时间3秒(ms)

// ====================== 任务二路径流程可调参数 ======================
// 四段直行时间（ms）
#define STRAIGHT1_TIME_MS 1700
#define STRAIGHT2_TIME_MS 1000
#define STRAIGHT3_TIME_MS 900
#define STRAIGHT4_TIME_MS 1100

// 三次左转90度时间（ms）
#define TURN1_TIME_MS 1100
#define TURN2_TIME_MS 1100
#define TURN3_TIME_MS 1000

// 行驶速度（基于 MOTOR_PWM_MAX 的百分比，1-100）
#define DRIVE_SPEED_PERCENT (9)
#define CONTROL2_DRIVE_SPEED_PERCENT (10)
