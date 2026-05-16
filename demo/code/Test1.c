#include "Test1.h"

// 声光显示、控制模块

void light_sound_init()
{
    gpio_init(BLUE_LED_PIN, GPO, 1, GPO_PUSH_PULL);     // 蓝色LED初始化为推挽输出模式
    gpio_set_level(BLUE_LED_PIN, GPIO_HIGH);            // 默认熄灭，低电平点亮
    gpio_init(SOUND_PIN_OUTPUT, GPO, 1, GPO_PUSH_PULL); // 声音输出引脚初始化为推挽输出模式
    gpio_init(SOUND_PIN_INPUT, GPI, 1, GPI_PULL_UP);    // 声音输入引脚初始化为上拉输入模式
}

void my_key_init()
{
    gpio_init(B31, GPI, 0, GPI_PULL_DOWN); // B31引脚初始化为上拉输入模式
    gpio_init(B30, GPI, 0, GPI_PULL_DOWN); // B30引脚初始化为上拉输入模式
    gpio_init(B29, GPI, 0, GPI_PULL_DOWN); // B29引脚初始化为上拉输入模式
}