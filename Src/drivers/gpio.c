/*
 * gpio.c
 *
 *  Created on: Oct 21, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_GPIO_ENABLE

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "Library/Inc/drivers/gpio.h"
#include "Library/Inc/tool.h"

static GPIO_Info infos[] = CONFIG_GPIO_INFOS;

/**
 * 设置引脚电平
 * @param id 引脚编号
 * @param value 电平,0低电平,1高电平
 */
void GPIO_Set(uint8_t id, uint8_t value)
{
	if (id > TOOL_GET_ARRAY_LENGTH(infos) - 1) return;
	HAL_GPIO_WritePin(infos[id].port, infos[id].pin, value == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * 获取引脚电平
 * @param id 引脚编号
 * @return 电平,0低电平,1高电平
 */
uint8_t GPIO_Get(uint8_t id)
{
	if (id > TOOL_GET_ARRAY_LENGTH(infos) - 1) return 0;
	GPIO_PinState state = HAL_GPIO_ReadPin(infos[id].port, infos[id].pin);
	return state == GPIO_PIN_SET ? 1 : 0;
}

/**
 * GPIO初始化
 */
void GPIO_Init()
{

}

#endif

