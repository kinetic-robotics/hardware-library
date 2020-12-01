/*
 * capacity.c
 *
 *  Created on: Nov 7, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_CAPACITY_ENABLE

#include "Library/Inc/capacity.h"
#include "Library/Inc/tool.h"
#include "Library/Inc/drivers/can.h"
#include <string.h>
#include "cmsis_os.h"
#include "timers.h"
#include "Library/Inc/led.h"

static Capacity_Info info;
static TimerHandle_t timeoutTimer; /* 超时定时器 */

/**
 * 发送目标功率到超级电容
 */
void Capacity_SetPower(float power)
{
	TOOL_LIMIT(power, 35, 135);
	uint16_t sendPower = power * 100;
	uint8_t data[2];
	data[0] = sendPower >> 8;
	data[1] = sendPower;
	CAN_SetOutput(CONFIG_CAPACITY_CAN_NUM, CAPACITY_CAN_SEND_ID, 0, data, 2);
}

/**
 * 接收到CAN回调
 */
static void Capacity_CANRxCallback(uint8_t canNum, uint16_t canID, uint8_t* data, uint8_t dataLength)
{
	/* 不处理不来自电容的CAN包 */
	if (canNum != CONFIG_CAPACITY_CAN_NUM || canID != CAPACITY_CAN_RECEIVE_ID) return;
	uint16_t parsedData[4];
	memcpy(parsedData, data, 8);
	info.inputVoltage = (float)parsedData[0] / 100;
	info.capVoltage   = (float)parsedData[1] / 100;
	info.inputCurrent = (float)parsedData[2] / 100;
	info.targetPower  = (float)parsedData[3] / 100;
	info.state = CAPACITY_OK;
	xTimerResetFromISR(timeoutTimer, NULL);
	LED_On(CONFIG_CAPACITY_LED);
}

/**
 * 超级电容模块超时处理
 */
void Capacity_Timeout(TimerHandle_t xTimer)
{
	memset(&info, 0, sizeof(info));
	LED_Off(CONFIG_CAPACITY_LED);
}

/**
 * 超级电容初始化
 */
void Capacity_Init()
{
	/* 超时定时器初始化 */
	timeoutTimer = xTimerCreate("Capacity_Timeout", CAPACITY_TIMEOUT, pdFALSE, 0, Capacity_Timeout);
	CAN_InitPacket(CONFIG_CAPACITY_CAN_NUM, CAPACITY_CAN_SEND_ID, 2, CONFIG_CAPACITY_HZ);
	CAN_RegisterCallback(&Capacity_CANRxCallback);
	/* 设置默认功率 */
	Capacity_SetPower(CONFIG_CAPACITY_DEFAULT_POWER);
}

/**
 * 获取超级电容模块数据
 * @return
 */
const Capacity_Info* Capacity_GetData()
{
	return &info;
}

#endif
