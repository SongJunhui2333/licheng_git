#include "track.h"
#include "Servo.h"

uint8_t x1 = 0;
uint8_t x2 = 0;
uint8_t x3 = 0;
uint8_t x4 = 0;

void Track_Init(void)
{
    gpio_init(TRACK_X1, GPI, 1, GPI_PULL_UP);
    gpio_init(TRACK_X2, GPI, 1, GPI_PULL_UP);
    gpio_init(TRACK_X3, GPI, 1, GPI_PULL_UP);
    gpio_init(TRACK_X4, GPI, 1, GPI_PULL_UP);
}

void Track_GPIO_test(void)
{
    x1 = gpio_get_level(TRACK_X1);
    x2 = gpio_get_level(TRACK_X2);
    x3 = gpio_get_level(TRACK_X3);
    x4 = gpio_get_level(TRACK_X4);

    tft180_show_int(0, 32, x1, 1);
    tft180_show_int(0, 48, x2, 1);
    tft180_show_int(0, 64, x3, 1);
    tft180_show_int(0, 80, x4, 1);
}

void CONTRAL1(void)
{
    // 仅在“正常前进”状态下检测十字（x3 && x2）触发停车等待
    if (control1_state == 0)
    {
        if (x3 == GPIO_HIGH && x2 == GPIO_HIGH)
        {
            Servo_Ctrl(SERVO_MOTOR_MID);
            control1_stop_flag = 1;                    // 标记检测到停止信号（可用于UI显示）
            control1_state = 1;                        // 进入停车等待状态
            control1_stop_time = timer_get(GPT_TIM_1); // 记录停车时间（ms）
        }
    }
}