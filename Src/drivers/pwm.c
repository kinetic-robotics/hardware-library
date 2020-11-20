/*
 * pwm.c
 *
 *  Created on: Nov 7, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_PWM_ENABLE

#include "Library/Inc/tool.h"
#include "Library/Inc/drivers/pwm.h"
#include "stm32f4xx_hal.h"

static PWM_Info infos[] = CONFIG_PWM_INFOS;

/**
 * 设置PWM输出
 * @param id PWM引脚编号
 * @param pulse 占空比,为0-100的数
 */
void PWM_Set(uint8_t id, float pulse)
{
	if (id > TOOL_GET_ARRAY_LENGTH(infos) - 1) return;
	TOOL_LIMIT(pulse, 0, 100);
	__HAL_TIM_SetCompare(infos[id].timer, infos[id].channel, 1000 - pulse * 10);
}

/**
 * PWM初始化
 */
void PWM_Init()
{
	for(size_t i = 0;i<TOOL_GET_ARRAY_LENGTH(infos);i++) {
		HAL_TIM_PWM_Start(infos[i].timer, infos[i].channel);
		PWM_Set(i, 0);
	}
}

#endif
