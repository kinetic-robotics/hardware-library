/*
 * led.c
 *
 *  Created on: Nov 25, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_LED_ENABLE

#include "Library/Inc/led.h"
#include "Library/Inc/drivers/gpio.h"

/**
 * LED熄灭
 * @param id   引脚编号
 */
void LED_Off(uint8_t id)
{
	GPIO_Set(id, 1);
}

/**
 * LED常亮
 * @param id   引脚编号
 */
void LED_On(uint8_t id)
{
	GPIO_Set(id, 0);
}

/**
 * LED闪烁初始化
 * @param info LED闪烁结构体
 * @param id   引脚编号
 * @param delay延时
 */
void LED_BlinkInit(LED_BlinkInfo* info, uint8_t id, uint16_t delay)
{
	info->lastTick = 0;
	info->delay = delay;
	info->id = id;
}


/**
 * LED闪烁
 * @param info LED闪烁结构体
 */
void LED_Blink(LED_BlinkInfo* info)
{
	uint32_t tick = xTaskGetTickCount();
	if (tick - info->lastTick > info->delay) {
		if (info->value == 0) {
			LED_On(info->id);
			info->value = 1;
		} else {
			LED_Off(info->id);
			info->value = 0;
		}
		info->lastTick = tick;
	}
}

/**
 * LED初始化
 */
void LED_Init()
{

}

#endif
