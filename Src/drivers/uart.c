/*
 * uart.c
 *
 *  Created on: Oct 20, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_UART_ENABLE

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "Library/Inc/library.h"
#include "Library/Inc/tool.h"
#include "Library/Inc/drivers/uart.h"

static UART_Info infos[] = CONFIG_UART_INFOS;
/* 回调列表 */
static UART_RxCallback callbacks[8];
static uint8_t callbacksLength = 0;

/**
 * 根据UART_HandleTypeDef查找infos的index,找不到会返回0xFF
 * @param instance
 * @return 数组index
 */
static uint8_t UART_SearchIndexByHandle(UART_HandleTypeDef* handle)
{
	for(size_t i = 0;i<TOOL_GET_ARRAY_LENGTH(infos);i++) {
		if (infos[i].huart == handle) {
			return i;
		}
	}
	return 0xFF;
}

/**
 * 串口发送函数,默认采用轮询模式
 * @param id
 * @param data 数据指针
 * @param dataLength 数据长度
 */
void UART_Send(uint8_t id, uint8_t* data, uint8_t dataLength)
{

	if(id > TOOL_GET_ARRAY_LENGTH(infos) - 1) return;
	HAL_UART_Transmit(infos[id].huart, data, dataLength, HAL_MAX_DELAY);
}

/**
 * 注册接收到串口信息回调
 * @param callback 回调函数,形式如void cb(uint8_t id, uint8_t* data, uint16_t dataLength)
 */
void UART_RegisterCallback(UART_RxCallback callback)
{
	if (callbacksLength > TOOL_GET_ARRAY_LENGTH(callbacks) - 1) {
		Library_Error();
		return;
	}
	callbacks[callbacksLength] = callback;
	callbacksLength++;
}

/**
 * UART接收中断
 */
void UART_RX_IT(UART_HandleTypeDef* huart)
{
	if (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE) != RESET) {
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		/* 未注册串口不予处理 */
		uint8_t index = UART_SearchIndexByHandle(huart);
		if (index == 0xFF) return;
		uint16_t length = infos[index].bufferLength - huart->hdmarx->Instance->NDTR;
		for(size_t i = 0;i < callbacksLength;i++) {
			(*callbacks[i])(index, infos[index].recvBuffer, length);
		}
		HAL_UART_DMAStop(huart);
		HAL_UART_Receive_DMA(huart, infos[index].recvBuffer, infos[index].bufferLength);
	}
}

/**
 * 串口初始化
 */
void UART_Init()
{
	for(size_t i = 0;i<TOOL_GET_ARRAY_LENGTH(infos);i++) {
		__HAL_UART_ENABLE_IT(infos[i].huart, UART_IT_IDLE);
		if(HAL_UART_Receive_DMA(infos[i].huart, infos[i].recvBuffer, infos[i].bufferLength) != HAL_OK) {
			Library_Error();
		}
	}
}

#endif
