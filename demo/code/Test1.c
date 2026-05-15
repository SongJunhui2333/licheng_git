#include "Test1.h"

void light_sound_init()
{
    gpio_init(SOUND_PIN_OUTPUT, GPO, 1, GPO_PUSH_PULL); // 声音输出引脚初始化为推挽输出模式
    gpio_init(SOUND_PIN_INPUT, GPI, 0, GPI_PULL_UP);    // 声音输入引脚初始化为上拉输入模式
}