/*
 * led.h
 *
 *  Created on: Nov 25, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_LED_H_
#define LIBRARY_LED_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_LED_ENABLE

typedef struct {
	uint32_t lastTick; /* 上一次改变状态的时间 */
	uint8_t  id;	   /* 引脚编号 */
	uint16_t delay;    /* 改变状态延时 */
	uint8_t  value;    /* 当前值 */
} LED_BlinkInfo;

void LED_Init();
void LED_Blink(LED_BlinkInfo* info);
void LED_BlinkInit(LED_BlinkInfo* info, uint8_t id, uint16_t delay);
void LED_On(uint8_t id);
void LED_Off(uint8_t id);

#endif
#endif /* INC_LED_H_ */
