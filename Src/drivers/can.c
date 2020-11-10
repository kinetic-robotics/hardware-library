/*
 * can.c
 *
 *  Created on: Oct 19, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_CAN_ENABLE

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "string.h"
#include "Library/Inc/tool.h"
#include "Library/Inc/library.h"
#include "Library/Inc/drivers/can.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

/* 最多8个允许定频发送的 */
static CAN_SentFrame sendFrames[8];
static uint8_t sendFramesLength = 0;
/* 回调列表 */
static CAN_RxCallback callbacks[8];
static uint8_t callbacksLength = 0;

/**
 * 根据CAN帧信息查找发送定频报文的数组的index,找不到会返回0xFF
 * @param canNum CAN编号,可取值为CAN_1, CAN_2
 * @param canID 帧ID
 * @return 数组index
 */
static uint8_t CAN_SearchSendFrameIndexByID(uint8_t canNum, uint16_t canID)
{
	for(size_t i = 0;i<sendFramesLength;i++) {
		if (sendFrames[i].canID == canID && sendFrames[i].canNum == canNum) {
			return i;
		}
	}
	return 0xFF;
}

/**
 * 初始化即将发送的CAN帧
 * @param canNum CAN编号,可取值为CAN_1, CAN_2
 * @param canID 帧ID
 * @param dataLength 帧长度
 * @param hz 发送频率,不可大于200hz
 * @return 数组index
 */
uint8_t CAN_InitPacket(uint8_t canNum, uint16_t canID, uint8_t dataLength, uint16_t hz)
{
	uint8_t index = CAN_SearchSendFrameIndexByID(canNum, canID);
	if (index == 0xFF) {
		/* 超出长度了 */
		if (sendFramesLength > TOOL_GET_ARRAY_LENGTH(sendFrames) - 1) {
			Library_Error();
			return 0xFF;
		}
		sendFrames[sendFramesLength].canNum = canNum;
		sendFrames[sendFramesLength].canID = canID;
		sendFrames[sendFramesLength].dataLength = dataLength;
		sendFrames[sendFramesLength]._interval  = 1000 / hz;
		index = sendFramesLength;
		sendFramesLength++;
	}
	return index;
}

/**
 * 发送1次CAN报文
 * @param canNum CAN编号,可取值为CAN_1, CAN_2
 * @param canID 帧ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
void CAN_Send(uint8_t canNum, uint16_t canID, uint8_t* data, uint8_t dataLength)
{
	CAN_TxHeaderTypeDef canHeader;
	canHeader.StdId = canID;
	canHeader.ExtId = 0;
	canHeader.IDE = CAN_ID_STD;
	canHeader.RTR = CAN_RTR_DATA;
	canHeader.DLC = dataLength;
	CAN_HandleTypeDef* hcan = canNum == CAN_1 ? &hcan1 : &hcan2;
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 0) {
		osDelay(1);
	}
	HAL_CAN_AddTxMessage(hcan, &canHeader, data, (uint32_t *)CAN_TX_MAILBOX0);
}

/**
 * 设置CAN报文输出数组
 * @param canNum CAN编号,可取值为CAN_1, CAN_2
 * @param canID 帧ID
 * @param arrIndex 数组序号
 * @param value 数据指针
 * @param size 数据长度
 */
void CAN_SetOutput(uint8_t canNum, uint16_t canID, uint8_t arrIndex, uint8_t* value, uint8_t size)
{
	uint8_t index = CAN_SearchSendFrameIndexByID(canNum, canID);
	if (index == 0xFF) return;
	memcpy(
			&sendFrames[index].data[arrIndex],
			value,
			size
		  );
}

/**
 * 注册接收到CAN信息回调
 * @param callback 回调函数,形式如void cb(uint8_t id, uint8_t* data, uint16_t dataLength)
 */
void CAN_RegisterCallback(CAN_RxCallback callback)
{
	if (callbacksLength > TOOL_GET_ARRAY_LENGTH(callbacks) - 1) {
		Library_Error();
		return;
	}
	callbacks[callbacksLength] = callback;
	callbacksLength++;
}

/**
 * CAN接收中断
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t canNum = hcan == &hcan1 ? CAN_1: CAN_2;
	uint32_t count=HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0);
	for(size_t i = 0;i<count;i++) {
		CAN_RxHeaderTypeDef canHeader;
		uint8_t canData[8];
		HAL_StatusTypeDef ret=HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &canHeader, canData);
	    if(ret == HAL_OK) {
	    	for(size_t i = 0;i < callbacksLength;i++) {
	    		callbacks[i](canNum, canHeader.StdId, canData, canHeader.DLC);
	    	}
	    }
	}
}

/**
 * CAN接收FIFO满中断,理论上是不会调用到的....
 */
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
	HAL_CAN_RxFifo0MsgPendingCallback(hcan);
}

/* CAN任务
 */
static void CAN_Task()
{
	while(1) {
		uint32_t now = HAL_GetTick();
		for(size_t i = 0;i<sendFramesLength;i++) {
			if (now - sendFrames[i]._lastTick > sendFrames[i]._interval) {
				CAN_Send(sendFrames[i].canNum, sendFrames[i].canID, sendFrames[i].data, sendFrames[i].dataLength);
				sendFrames[i]._lastTick = now;
			}
		}
		osDelay(5);
	}
}

/**
 * CAN初始化
 */
void CAN_Init()
{
	static osThreadId_t canTaskHandle;
	const osThreadAttr_t canTaskAttributes = {
	  .name = "canTask",
	  .priority = (osPriority_t) osPriorityHigh,
	  .stack_size = 128 * 4
	};
	canTaskHandle = osThreadNew(CAN_Task, NULL, &canTaskAttributes);
	UNUSED(canTaskHandle);
	/* CAN过滤器初始化 */
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.SlaveStartFilterBank = 14;
	sFilterConfig.FilterActivation = ENABLE;
	/* 初始化CAN1 */
	if ( HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK ||
		 HAL_CAN_Start(&hcan1)                        != HAL_OK ||
	     HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING)) {
		Library_Error();
	}
	/* 初始化CAN2 */
	sFilterConfig.FilterBank = 14;
	if ( HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK ||
		 HAL_CAN_Start(&hcan2)                        != HAL_OK ||
		 HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING)) {
		Library_Error();
	}
}

#endif
