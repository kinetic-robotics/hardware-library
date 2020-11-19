/*
 * usart.h
 *
 *  Created on: Oct 20, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_DRIVERS_UART_H_
#define LIBRARY_DRIVERS_UART_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_UART_ENABLE

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "Configurations/library_config.h"

typedef struct {
	UART_HandleTypeDef* huart;                     /* UART对象指针 */
	uint8_t* recvBuffer;                           /* 接收缓冲区 */
	uint16_t bufferLength;                         /* 接收缓冲区大小 */
} UART_Info;

/**
 * 用户回调函数
 * @param id
 * @param data 数据指针
 * @param dataLength 数据长度
 */
typedef void (*UART_RxCallback)(uint8_t id, uint8_t* data, uint16_t dataLength);

void UART_Send(uint8_t id, uint8_t* data, uint8_t dataLength);
void UART_RegisterCallback(UART_RxCallback callback);
void UART_Init();
void UART_RX_IT(UART_HandleTypeDef* huart);

#endif
#endif /* LIBRARY_DRIVERS_UART_H_ */
