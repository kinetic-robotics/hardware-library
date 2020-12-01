/*
 * rc.c
 *
 *  Created on: Oct 21, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_RC_ENABLE

#include <stdint.h>
#include "Library/Inc/drivers/uart.h"
#include "Library/Inc/rc.h"
#include "stdlib.h"
#include "string.h"
#include "Library/Inc/led.h"
#include "timers.h"

static RC_Info info;
static TimerHandle_t timeoutTimer; /* 超时定时器 */

/**
 * 遥控器失去连接处理
 */
static void RC_Timeout(TimerHandle_t xTimer)
{
	memset(&info, 0, sizeof(info));
	LED_Off(CONFIG_RC_LED);
}

static void RC_UARTRxCallback(uint8_t id, uint8_t* data, uint16_t dataLength)
{
	if (id != CONFIG_RC_UART) return;
	/* 遥控器包异常 */
	if (dataLength != RC_DBUS_FRAME_SIZE) {
		return;
	}
	/* 下面是正常遥控器数据的处理 */
	info.ch1 = (data[0] | data[1] << 8) & 0x07FF;
	info.ch1 -= 1024;
	info.ch2 = (data[1] >> 3 | data[2] << 5) & 0x07FF;
	info.ch2 -= 1024;
	info.ch3 = (data[2] >> 6 | data[3] << 2 | data[4] << 10) & 0x07FF;
	info.ch3 -= 1024;
	info.ch4 = (data[4] >> 1 | data[5] << 7) & 0x07FF;
	info.ch4 -= 1024;

	/* 防止遥控器零点有偏差 */
	if(info.ch1 <= 5 && info.ch1 >= -5) {
	    info.ch1 = 0;
	}
	if(info.ch2 <= 5 && info.ch2 >= -5) {
	    info.ch2 = 0;
	}
	if(info.ch3 <= 5 && info.ch3 >= -5) {
	    info.ch3 = 0;
	}
	if(info.ch4 <= 5 && info.ch4 >= -5) {
	    info.ch4 = 0;
	}

	/* 拨杆值获取 */
	info.sw1 = ((data[5] >> 4) & 0x000C) >> 2;
	info.sw2 = (data[5] >> 4) & 0x0003;

	/* 遥控器异常值处理，函数直接返回 */
	if ((abs(info.ch1) > 660) || \
	    (abs(info.ch2) > 660) || \
	    (abs(info.ch3) > 660) || \
	    (abs(info.ch4) > 660))
	{
		RC_Timeout(timeoutTimer);
	    return;
	}

	/* 鼠标移动速度获取 */
	info.mouse.x = data[6] | (data[7] << 8);
	info.mouse.y = data[8] | (data[9] << 8);

	/* 鼠标左右按键键值获取 */
	info.mouse.l = data[12];
	info.mouse.r = data[13];

	/* 键盘按键键值获取 */
	info.kb.keyCode = data[14] | data[15] << 8;

	/* 遥控器左侧上方拨轮数据获取，和遥控器版本有关，有的无法回传此项数据 */
	info.wheel = data[16] | data[17] << 8;
	info.wheel -= 1024;

	info.state = RC_OK;
	xTimerResetFromISR(timeoutTimer, NULL);
	LED_On(CONFIG_RC_LED);
}

/**
 * 获取遥控器解析结果, 只允许读取, 请勿修改!
 * @return 信息指针
 */
const RC_Info* RC_GetData()
{
	return &info;
}

void RC_Init()
{
	/* 超时定时器初始化 */
	timeoutTimer = xTimerCreate("RC_Timeout", RC_TIMEOUT, pdFALSE, 0, RC_Timeout);
	UART_RegisterCallback(&RC_UARTRxCallback);
}

#endif

