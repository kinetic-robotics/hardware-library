/*
 * spi.c
 *
 *  Created on: Oct 21, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_SPI_ENABLE

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "Library/Inc/drivers/spi.h"
#include "Library/Inc/tool.h"

static SPI_Info infos[] = CONFIG_SPI_INFOS;

/**
 * SPI发送
 * @param id SPI编号
 * @param pData 发送数据指针
 * @param size 发送数据长度
 * @param timeout 超时时间,可取SPI_INFINITE_TIMEOUT不超时
 */
void SPI_Transmit(uint8_t id, uint8_t *pData, uint16_t size, uint32_t timeout)
{
	if (id > TOOL_GETARRLEN(infos) - 1) return;
	HAL_SPI_Transmit(infos[id].hspi, pData, size, timeout);
}

/**
 * SPI接收
 * @param id SPI编号
 * @param pData 接收数据指针
 * @param size 接收数据长度
 * @param timeout 超时时间,可取SPI_INFINITE_TIMEOUT不超时
 */
void SPI_Receive(uint8_t id, uint8_t *pData, uint16_t size, uint32_t timeout)
{
	if (id > TOOL_GETARRLEN(infos) - 1) return;
	HAL_SPI_Receive(infos[id].hspi, pData, size, timeout);
}

/**
 * SPI发送并接收
 * @param id SPI编号
 * @param pTxData
 * @param pRxData 发送数据指针
 * @param size 发送数据长度
 * @param timeout 超时时间,可取SPI_INFINITE_TIMEOUT不超时
 */
void SPI_TransmitReceive(uint8_t id, uint8_t *pTxData, uint8_t *pRxData, uint16_t size, uint32_t timeout)
{
	if (id > TOOL_GETARRLEN(infos) - 1) return;
	HAL_SPI_TransmitReceive(infos[id].hspi, pTxData, pRxData, size, timeout);
}

void SPI_Init()
{

}

#endif
