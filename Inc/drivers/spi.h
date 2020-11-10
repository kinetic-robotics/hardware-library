/*
 * spi.h
 *
 *  Created on: Oct 21, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_DRIVERS_SPI_H_
#define LIBRARY_DRIVERS_SPI_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_SPI_ENABLE

typedef struct {
	SPI_HandleTypeDef* hspi;                     /* UART对象指针 */
} SPI_Info;

/* 超时没有时间限制 */
#define SPI_INFINITE_TIMEOUT 0xFFFFFFFFU

void SPI_Transmit(uint8_t id, uint8_t *pData, uint16_t size, uint32_t timeout);
void SPI_Receive(uint8_t id, uint8_t *pData, uint16_t size, uint32_t timeout);
void SPI_TransmitReceive(uint8_t id, uint8_t *pTxData, uint8_t *pRxData, uint16_t size, uint32_t timeout);
void SPI_Init();

#endif
#endif /* LIBRARY_DRIVERS_SPI_H_ */
