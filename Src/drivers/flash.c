/*
 * flash.c
 *
 *  Created on: Oct 28, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_FLASH_ENABLE

#include "stm32f4xx_hal.h"
#include "Library/Inc/tool.h"
#include "Library/Inc/drivers/flash.h"
#include "string.h"

/* flash总占用空间大小,用于一些地方动态分配空间 */
uint32_t allDataLength = 0;

/**
 * 配置FLASH的占用空间大小
 * @param size flash总占用空间
 */
void Flash_ConfigAllDataLength(uint32_t size)
{
	if (allDataLength > CONFIG_FLASH_SECTOR_SIZE) return;
	allDataLength = size;
}

/**
 * 写入FLASH存储块内容
 * @param address flash存储块偏移地址
 * @param data 数据指针
 * @param dataLength 数据长度
 */
void Flash_Write(uint32_t address, void* data, uint32_t dataLength)
{
	if (address + dataLength > CONFIG_FLASH_SECTOR_SIZE) return;
	/* 拷贝旧数据 */
	uint32_t allDataLengthWithPadding = allDataLength;
	if (allDataLength % 4 != 0) {
		allDataLengthWithPadding++;
	}
	uint8_t tmpData[allDataLengthWithPadding];
	memcpy(tmpData, (uint8_t*)CONFIG_FLASH_SECTOR_ADDRESS, allDataLength);
	/* 写入新数据,如果新数据长度超标则回截断 */
	memcpy(tmpData + address, data, dataLength);
	FLASH_EraseInitTypeDef eraseConfiguration;
	uint32_t _;
	eraseConfiguration.TypeErase = FLASH_TYPEERASE_SECTORS;
	eraseConfiguration.NbSectors = 1;
	eraseConfiguration.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	eraseConfiguration.Sector = CONFIG_FLASH_SECTOR_NUM;
	/* 清除错误 */
	/* 解锁FLASH */
	HAL_FLASH_Unlock();
	while (FLASH_WaitForLastOperation(50000U) != HAL_OK);
	/* 擦除存储扇区 */
	HAL_FLASHEx_Erase(&eraseConfiguration, &_);
	/* 写入数据 */
	//__HAL_CONFIG_FLASH_CLEAR_FLAG(CONFIG_FLASH_FLAG_EOP | CONFIG_FLASH_FLAG_OPERR | CONFIG_FLASH_FLAG_WRPERR | CONFIG_FLASH_FLAG_PGAERR | CONFIG_FLASH_FLAG_PGPERR | CONFIG_FLASH_FLAG_PGSERR | CONFIG_FLASH_FLAG_RDERR);
	for (size_t i = 0;i<allDataLengthWithPadding / 4;i++) {
		uint32_t writeData;
		memcpy(&writeData, (uint32_t*)&tmpData[4*i], 4);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CONFIG_FLASH_SECTOR_ADDRESS + 4 * i, writeData);
	}
	/* 锁定FLASH */
	HAL_FLASH_Lock();
}

/**
 * 读取FLASH存储块内容
 * @param address flash存储块偏移地址
 * @param data 获取数据的指针
 * @param dataLength 最大数据长度
 * @return 实际数据长度
 */
void Flash_Read(uint32_t address, void* data, uint32_t dataLength)
{
	if (address + dataLength > CONFIG_FLASH_SECTOR_SIZE) return;
	memcpy(data, (void*)(address + CONFIG_FLASH_SECTOR_ADDRESS), dataLength);
}

/**
 * FLASH初始化
 */
void Flash_Init()
{

}

#endif

