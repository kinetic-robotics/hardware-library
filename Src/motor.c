/*
 * motor.c
 *
 *  Created on: Oct 19, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_MOTOR_ENABLE

#include <stdint.h>
#include "Library/Inc/motor.h"
#include "Library/Inc/tool.h"
#include "Configurations/library_config.h"
#include "Library/Inc/drivers/can.h"
#include "Library/Inc/drivers/pwm.h"
#include "Library/Inc/algorithm/ramp.h"

static Motor_Info infos[] = CONFIG_MOTOR_INFOS;

/**
 * 根据电机信息查找infos的index,找不到会返回0xFF
 * @param canNum CAN编号,可取值为CAN_1, CAN_2
 * @param id 电机ID
 * @param type 电机类型
 * @return 数组index
 */
static uint8_t Motor_SearchInfoIndexByInfo(uint8_t canNum, uint8_t id, uint8_t type)
{
	for(size_t i = 0;i<TOOL_GET_ARRAY_LENGTH(infos);i++) {
		if (infos[i].id == id && infos[i].canNum == canNum && (infos[i].type & type) != 0) {
			return i;
		}
	}
	return 0xFF;
}

/**
 * 根据电机类型和id寻找对应的发送编号
 */
static uint16_t Motor_GetMotorSendID(uint8_t type, uint8_t id)
{
	if (type == MOTOR_TYPE_RM3508 || type == MOTOR_TYPE_RM2006) {
		if (id < 5) {
			return 0x200;
		} else {
			return 0x1FF;
		}
	}
	if (type == MOTOR_TYPE_RM6020) {
		if (id < 5) {
			return 0x1FF;
		} else {
			return 0x2FF;
		}
	}
	return 0;
}

/**
 * 设置电机输出电流
 * @param id 电机ID
 * @param current 电流,无论是什么电机,范围为-10000-10000
 */
void Motor_Set(uint8_t id, int16_t current)
{
	if (id > TOOL_GET_ARRAY_LENGTH(infos) - 1) return;
	uint8_t canFlag = 0; /* 是否是CAN报文电机 */
	float sendCurrent = current;
	TOOL_ABS_LIMIT(sendCurrent, MOTOR_VIRTUAL_ABS_MAX);
	switch (infos[id].type) {
		case MOTOR_TYPE_RM3508:
			sendCurrent = sendCurrent * (MOTOR_TYPE_RM3508_ABS_MAX / MOTOR_VIRTUAL_ABS_MAX);
			canFlag = 1;
			break;
		case MOTOR_TYPE_RM2006:
			sendCurrent = sendCurrent * (MOTOR_TYPE_RM2006_ABS_MAX / MOTOR_VIRTUAL_ABS_MAX);
			canFlag = 1;
			break;
		case MOTOR_TYPE_RM6020:
			sendCurrent = sendCurrent * (MOTOR_TYPE_RM6020_ABS_MAX / MOTOR_VIRTUAL_ABS_MAX);
			canFlag = 1;
			break;
		case MOTOR_TYPE_BLHEILS:
			sendCurrent = sendCurrent * (MOTOR_TYPE_BLHEILS_MAX / MOTOR_VIRTUAL_ABS_MAX);
			/* 不能反转 */
			TOOL_LIMIT(sendCurrent, 0, MOTOR_TYPE_BLHEILS_MAX);
			break;
	}
	if (infos[id].state == 0) {
		sendCurrent = 0;
	}
	/* CAN报文电机或PWM电机处理 */
	if (canFlag == 1) {
		uint8_t data[2];
		data[0] = (int16_t)sendCurrent >> 8;
		data[1] = (int16_t)sendCurrent & 0xFF;
		uint16_t packetID = Motor_GetMotorSendID(infos[id].type, infos[id].id);
		if (packetID != 0) {
			CAN_SetOutput(infos[id].canNum, packetID, ((infos[id].id - 1) % 4) * 2, data, 2);
		}
	}
	if (infos[id].type == MOTOR_TYPE_BLHEILS){
		PWM_Set(infos[id].id, Ramp_Calc(&infos[id]._ramp, 50 + sendCurrent));
	}
	infos[id]._lastCurrent = sendCurrent;
}

/**
 * 接收到CAN回调
 */
static void Motor_CANRxCallback(uint8_t canNum, uint16_t canID, uint8_t* data, uint8_t dataLength)
{
	/* 解析该报文对应的电机编号,默认是3508和2006电机 */
	uint16_t id = canID - 0x200;
	/* 6020电机 */
	if (canID > 0x208) {
		id = canID - 0x204;
	}
	/* 未注册电机不予处理 */
	uint8_t index = Motor_SearchInfoIndexByInfo(canNum, id, MOTOR_TYPE_RM3508 | MOTOR_TYPE_RM2006);
	if (index == 0xFF) {
		/* 考虑到6020电机的1-4号对应的是3508电机报文的5-8号,所以加一个处理 */
		index = Motor_SearchInfoIndexByInfo(canNum, canID - 0x204, MOTOR_TYPE_RM6020);
		if (index == 0xFF) return;
	}
	Motor_Info* ptr = &infos[index];
	if (ptr->_msgCount < 50) {
		ptr->_msgCount++;
		ptr->ecd = (uint16_t)(data[0] << 8 | data[1]);
		ptr->_offsetEcd = ptr->ecd;
		return;
	}
	int32_t tempSum = 0;
	ptr->lastEcd = ptr->ecd;
	ptr->ecd = (uint16_t)(data[0] << 8 | data[1]);
	ptr->speedRpm = (int16_t)(data[2] << 8 | data[3]);
	if (ptr->ecd - ptr->lastEcd > 5000) {
	    ptr->roundCount--;
	    ptr->_ecdRawRate = ptr->ecd - ptr->lastEcd - 8192;
	} else if (ptr->ecd - ptr->lastEcd < -5000) {
	    ptr->roundCount++;
	    ptr->_ecdRawRate = ptr->ecd - ptr->lastEcd + 8192;
	} else {
	    ptr->_ecdRawRate = ptr->ecd - ptr->lastEcd;
	}
	ptr->totalEcd = ptr->roundCount * 8192 + ptr->ecd - ptr->_offsetEcd;
	ptr->totalAngle = ptr->totalEcd * 360 / 8192;
	ptr->_rateBuf[ptr->_bufCut++] = ptr->_ecdRawRate;
	if (ptr->_bufCut >= MOTOR_FILTER_BUFFER_SIZE) {
	    ptr->_bufCut = 0;
	}
	for (uint8_t i = 0; i < MOTOR_FILTER_BUFFER_SIZE; i++) {
	    tempSum += ptr->_rateBuf[i];
	}
	ptr->_filterRate = (int32_t)(tempSum/MOTOR_FILTER_BUFFER_SIZE);
}

/**
 * 电机启用
 */
void Motor_on()
{
	for(size_t i = 0;i<TOOL_GET_ARRAY_LENGTH(infos);i++) {
		infos[i].state = 1;
	}
}

/**
 * 电机禁用
 */
void Motor_off()
{
	for(size_t i = 0;i<TOOL_GET_ARRAY_LENGTH(infos);i++) {
		infos[i].state = 0;
		Motor_Set(i, 0);
	}
}

/**
 * 电机控制任务,一般来说,只用于PWM电机,CAN电机无需开启本任务
 */
void Motor_Task()
{
	while(1) {
		for (size_t i = 0;i<TOOL_GET_ARRAY_LENGTH(infos);i++) {
			/* BLHEILS电机要求斜坡启动 */
			if (infos[i].type == MOTOR_TYPE_BLHEILS) {
				PWM_Set(infos[i].id, Ramp_RunCalc(&infos[i]._ramp));
			}
		}
		osDelay(10);
	}
}

/**
 * 电机初始化
 */
void Motor_Init()
{
	Motor_off();
	/* 注册必要的定频发送报文 */
	for (size_t i = 0;i<TOOL_GET_ARRAY_LENGTH(infos);i++) {
		uint16_t packetID = Motor_GetMotorSendID(infos[i].type, infos[i].id);
		if (packetID != 0) {
			CAN_InitPacket(infos[i].canNum, packetID, 8, CONFIG_MOTOR_CAN_HZ);
		}
		/* 解锁BLHEILS,初始化斜坡控制 */
		if (infos[i].type == MOTOR_TYPE_BLHEILS){
			Ramp_Init(&infos[i]._ramp, MOTOR_TYPE_BLHEILS_RAMP_SCALE, 50);
			PWM_Set(infos[i].id, 50);
		}
	}
	/* 注册CAN接收回调 */
	CAN_RegisterCallback(&Motor_CANRxCallback);
	Motor_on();
	static osThreadId_t motorTaskHandle;
	const osThreadAttr_t motorTaskAttributes = {
			.name = "motorTask",
			.priority = (osPriority_t) osPriorityHigh,
			.stack_size = 128 * 4
	};
	motorTaskHandle = osThreadNew(Motor_Task, NULL, &motorTaskAttributes);
	UNUSED(motorTaskHandle);
}

/**
 * 根据电机ID获取电机信息指针, 只允许读取, 请勿修改!
 * @param id 电机id
 * @return 信息指针,如果电机不存在,则返回0
 */
const Motor_Info* Motor_GetMotorData(uint8_t id)
{
	if (id > TOOL_GET_ARRAY_LENGTH(infos) - 1) return (const Motor_Info*)0;
	return &infos[id];
}

#endif
