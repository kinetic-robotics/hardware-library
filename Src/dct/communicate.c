/*
 * communicate.c
 *
 *  Created on: Nov 23, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DCT_ENABLE

#include "Library/Inc/algorithm/crc.h"
#include "Library/Inc/dct/pid.h"
#include "Library/Inc/dct/communicate.h"
#include "Library/Inc/drivers/usb.h"
#include "Library/Inc/tool.h"
#include <string.h>
#include "cmsis_os.h"

static uint8_t  flag = DCT_STATE_SOF; /* USB接收数据状态机 */
static uint8_t  cmdID = 0;            /* 接收到包的指令ID */
static uint16_t pdataLength = 0;      /* 这包声明的数据长度 */
static uint8_t  receivedData[DCT_PACKET_MAX_LENGTH] = {0}; /* 整个数据包暂存区 */
static uint16_t recvDataLength = 0;   /* 已经接收到的数据包长度 */

/**
 * 解析DCT协议数据
 * @param cmdID 指令ID
 * @param data 数据数组指针
 * @param dataLength 数据长度
 */
static void DCT_ParsedData(uint8_t cmdID, uint8_t* data, uint16_t dataLength)
{
	DCT_PID_ParsedData(cmdID, data, dataLength);
}

/**
 * 收到USB数据回调
 */
static void DCT_Communicate_USBRxCallback(uint8_t* dataBuffer, uint32_t dataLength)
{
	for(size_t i = 0;i<dataLength;i++) {
		uint8_t data = dataBuffer[i];
		receivedData[recvDataLength++] = data;
		switch(flag) {
			case DCT_STATE_SOF:
				if (data == DCT_SOF) {
					flag = DCT_STATE_CMD_ID;
				} else {
					/* 如果不是SOF就不开始记录内容 */
					recvDataLength = 0;
				}
				break;
			case DCT_STATE_CMD_ID:
				cmdID = data;
				flag = DCT_STATE_LENGTH_HIGH;
				break;
			case DCT_STATE_LENGTH_HIGH:
				pdataLength = data << 8;
				flag = DCT_STATE_LENGTH_LOW;
				break;
			case DCT_STATE_LENGTH_LOW:
				pdataLength |= data;
				/* 超长度数据直接丢弃该包 */
				if (pdataLength > DCT_PACKET_MAX_LENGTH - 4) {
					flag = DCT_STATE_SOF;
				} else {
					flag = DCT_STATE_DATA;
				}
				break;
			case DCT_STATE_DATA:
				if (recvDataLength == dataLength + 4) {
					flag = DCT_STATE_CRC8;
				}
				break;
			case DCT_STATE_CRC8:
				if (data == CRC_GetCRC8CheckSum(receivedData, recvDataLength - 1)) {
					DCT_ParsedData(cmdID, receivedData + 4, dataLength);
				}
				data = DCT_STATE_SOF;
				break;
		}
	}
}

/**
 * 将数据打包成DCT包并发送到电脑
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
void DCT_Communicate_Send(uint8_t cmdID, uint8_t* data, uint16_t dataLength)
{
	uint8_t buffer[dataLength + 4];
	if (dataLength > DCT_PACKET_MAX_LENGTH) return;
	buffer[0] = DCT_SOF;
	buffer[1] = cmdID;
	buffer[2] = dataLength >> 8;
	buffer[3] = dataLength & 0xFF;
	memcpy(buffer, data + 4, dataLength);
	buffer[dataLength + 4] = CRC_GetCRC8CheckSum(buffer, TOOL_GET_ARRAY_LENGTH(buffer) - 1);
	USB_Send(buffer, TOOL_GET_ARRAY_LENGTH(buffer));
}

/**
 * 调试与配置工具任务
 */
static void DCT_Communicate_Task()
{
	while(1) {
		DCT_PID_SendPacket();
		osDelay(10);
	}
}

/**
 * 调试与配置工具初始化
 */
void DCT_Communicate_Init()
{
	/* 注册USB接收回调 */
	USB_RegisterCallback(DCT_Communicate_USBRxCallback);
	/* 创建任务 */
	static osThreadId_t dctTaskHandle;
	const osThreadAttr_t dctTaskAttributes = {
			.name = "dctTask",
			.priority = (osPriority_t) osPriorityHigh,
			.stack_size = 128 * 4
	};
	dctTaskHandle = osThreadNew(DCT_Communicate_Task, NULL, &dctTaskAttributes);
	UNUSED(dctTaskHandle);
}

#endif
