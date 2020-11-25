/*
 * pid.c
 *
 *  Created on: Nov 23, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DCT_ENABLE

#include "Library/Inc/library.h"
#include "Library/Inc/tool.h"
#include "Library/Inc/dct/communicate.h"
#include "Library/Inc/dct/pid.h"
#include "Library/Inc/algorithm/pid.h"
#include <string.h>

static DCT_PID_Info infos[CONFIG_DCT_PID_MAX_LENGTH]; /* PID结构体信息 */
static uint8_t pidLength = 0;						  /* 当前PID结构体数组长度 */

/**
 * 通过上位机显示PID参数
 * @param name 名称,将显示在DCT上
 * @param pid  PID结构体指针
 */
void DCT_PID_Show(char* name, PID_Info* pid)
{
	#ifndef CONFIG_DCT_PID_ENABLE
		return;
	#endif
	if (pidLength > TOOL_GET_ARRAY_LENGTH(infos) - 1) return;
	infos[pidLength].name = name;
	infos[pidLength].pid  = pid;
	pidLength++;
	DCT_PID_SendInitPacket();
}

/**
 * 发送PID数据包
 */
void DCT_PID_SendPacket()
{
	if (pidLength == 0) return;
	uint8_t buffer[pidLength * (sizeof(infos[0].pid->get) + sizeof(infos[0].pid->set))];
	uint32_t nowOffset  = 0; /* 当前数组偏移 */
	uint8_t  typeLength = 0; /* 类型长度 */
	for (size_t i = 0;i<pidLength;i++) {
		typeLength = sizeof(infos[0].pid->get);
		memcpy(buffer + nowOffset, &infos[i].pid->get, typeLength);
		nowOffset += typeLength;

		typeLength = sizeof(infos[0].pid->set);
		memcpy(buffer + nowOffset, &infos[i].pid->set, typeLength);
		nowOffset += typeLength;
	}
	DCT_Communicate_Send(DCT_PID_CMD_ID, buffer, TOOL_GET_ARRAY_LENGTH(buffer));
}

/**
 * 发送PID初始化数据包
 */
void DCT_PID_SendInitPacket()
{
	if (pidLength == 0) return;
	uint32_t dataLength = 0;
	for (size_t i = 0;i<pidLength;i++) {
		dataLength += sizeof(infos[0].pid->p);
		dataLength += sizeof(infos[0].pid->i);
		dataLength += sizeof(infos[0].pid->d);
		dataLength += sizeof(infos[0].pid->maxOutput);
		dataLength += sizeof(infos[0].pid->integralLimit);
		dataLength += strlen(infos[0].name);
	}
	/* 超过长度了 */
	if (dataLength > 0xFFFF) {
		Library_Error();
		return;
	}
	uint8_t buffer[dataLength];
	uint32_t nowOffset  = 0; /* 当前数组偏移 */
	uint8_t  typeLength = 0; /* 类型长度 */
	for (size_t i = 0;i<pidLength;i++) {
		typeLength = strlen(infos[0].name);
		memcpy(buffer + nowOffset, infos[i].name, typeLength);
		nowOffset += typeLength;

		typeLength = sizeof(infos[0].pid->p);
		memcpy(buffer + nowOffset, &infos[i].pid->p, typeLength);
		nowOffset += typeLength;

		typeLength = sizeof(infos[0].pid->i);
		memcpy(buffer + nowOffset, &infos[i].pid->i, typeLength);
		nowOffset += typeLength;

		typeLength = sizeof(infos[0].pid->d);
		memcpy(buffer + nowOffset, &infos[i].pid->d, typeLength);
		nowOffset += typeLength;

		typeLength = sizeof(infos[0].pid->maxOutput);
		memcpy(buffer + nowOffset, &infos[i].pid->maxOutput, typeLength);
		nowOffset += typeLength;

		typeLength = sizeof(infos[0].pid->integralLimit);
		memcpy(buffer + nowOffset, &infos[i].pid->integralLimit, typeLength);
		nowOffset += typeLength;
	}
	DCT_Communicate_Send(DCT_PID_CMD_ID, buffer, TOOL_GET_ARRAY_LENGTH(buffer));
}

/**
 * 解析上位机发送的数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
void DCT_PID_ParsedData(uint8_t cmdID, uint8_t* data, uint16_t dataLength)
{
	/* 请求PID参数包 */
	if (cmdID == DCT_PID_REQUEST_CMD_ID && dataLength == 0) {
		DCT_PID_SendInitPacket();
	}
}

#endif
