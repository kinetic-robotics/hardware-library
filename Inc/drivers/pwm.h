/*
 * pwm.h
 *
 *  Created on: Nov 7, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_DRIVERS_PWM_H_
#define LIBRARY_DRIVERS_PWM_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_PWM_ENABLE

typedef struct {
	TIM_HandleTypeDef* timer;      /* 定时器指针 */
	HAL_TIM_ActiveChannel channel; /* PWM输出通道 */
} PWM_Info;

void PWM_Set(uint8_t id, float pulse);
void PWM_Init();

#endif
#endif /* LIBRARY_DRIVERS_PWM_H_ */
