/*
 * can.h
 *
 *  Created on: Oct 19, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_DRIVERS_CAN_H_
#define LIBRARY_DRIVERS_CAN_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_CAN_ENABLE

#include "stm32f4xx_hal.h"
/* CAN编号 */
#define CAN_1 0
#define CAN_2 1

typedef struct {
	uint8_t canNum;     /* CAN编号,可取值为CAN_1,CAN_2 */
	uint16_t canID;     /* 帧ID */
	uint8_t data[8];    /* 帧内容 */
	uint8_t dataLength; /* 帧内容长度 */
} CAN_SentFrame;

/**
 * 用户回调函数
 * @param id
 * @param data 数据指针
 * @param dataLength 数据长度
 */
typedef void (*CAN_RxCallback)(uint8_t canNum, uint16_t canID, uint8_t* data, uint8_t dataLength);

void CAN_Init();
void CAN_RegisterCallback(CAN_RxCallback callback);
void CAN_Send(uint8_t canNum, uint16_t canID, uint8_t* data, uint8_t dataLength);
void CAN_SetOutput(uint8_t canNum, uint16_t canID, uint8_t arrIndex, uint8_t* value, uint8_t size);
uint8_t CAN_InitPacket(uint8_t canNum, uint16_t canID, uint8_t dataLength);

#endif
#endif /* LIBRARY_DRIVERS_CAN_H_ */
