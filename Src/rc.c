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

static RC_Info info;

static void RC_UARTRxCallback(uint8_t id, uint8_t* data, uint16_t dataLength)
{
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
	    memset(&info, 0, sizeof(info));
	    return ;
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
	info.lastReceiveTime = HAL_GetTick();
}

/**
 * 获取遥控器解析结果, 只允许读取, 请勿修改!
 * @return 信息指针
 */
const RC_Info* RC_GetData()
{
	return &info;
}

void RC_Task()
{
	while(1) {
		/* 遥控器超时 */
		if (HAL_GetTick() - info.lastReceiveTime > RC_TIMEOUT) {
			memset(&info, 0, sizeof(info));
			info.state = 1;
		}
		osDelay(10);
	}
}

void RC_Init()
{
	/* 刚启动时默认遥控器未连接 */
	info.state = 1;
	UART_RegisterCallback(&RC_UARTRxCallback);
	/* 创建任务 */
	static osThreadId_t rcTaskHandle;
	const osThreadAttr_t rcTaskAttributes = {
			.name = "rcTask",
			.priority = (osPriority_t) osPriorityHigh,
			.stack_size = 128 * 4
	};
	rcTaskHandle = osThreadNew(RC_Task, NULL, &rcTaskAttributes);
	UNUSED(rcTaskHandle);
}

#endif

