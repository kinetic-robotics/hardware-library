/*
 * capacity.h
 *
 *  Created on: Nov 7, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_CAPACITY_H_
#define LIBRARY_CAPACITY_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_CAPACITY_ENABLE

/* 电容状态正常 */
#define CAPACITY_OK 1

typedef struct {
	float inputVoltage; /* 输入电压 */
	float capVoltage;   /* 电容电压 */
	float inputCurrent; /* 输入电流 */
	float targetPower;  /* 目标功率 */
	uint8_t state;      /* 电容状态 */
} Capacity_Info;

/* 超级电容模块CAN接收ID */
#define CAPACITY_CAN_RECEIVE_ID 0x211

/* 超级电容模块CAN发送ID */
#define CAPACITY_CAN_SEND_ID 0x210

/* 超级电容超时 */
#define CAPACITY_TIMEOUT 300

void Capacity_SetPower(float power);
void Capacity_Init();
const Capacity_Info* Capacity_GetData();

#endif
#endif /* CAPACITY_H_ */
