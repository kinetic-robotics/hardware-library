/*
 * storage.c
 *
 *  Created on: Oct 28, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_STORAGE_ENABLE

#include "Library/Inc/drivers/flash.h"
#include "Library/Inc/storage.h"
#include "Library/Inc/tool.h"
#include "Library/Inc/library.h"
#include "Configurations/library_config.h"

static Storage_Info infos[] = CONFIG_STORAGE_INFOS;
/* 所有数据长度 */
static uint32_t allDataLength = 0;

/**
 * 写入内容
 * @param id flash存储块编号
 * @param data 数据指针
 * @param dataLength 数据长度
 */
void Storage_Write(uint32_t id, void* data, uint32_t dataLength)
{
	if (id > TOOL_GETARRLEN(infos) - 1) return;
	Flash_Write(infos[id]._address, data, dataLength);
}

/**
 * 存储内容
 * @param id 数据编号
 * @param data 获取数据的指针
 * @param dataLength 获取数据的长度
 * @return 实际读取数据长度
 */
uint32_t Storage_Read(uint32_t id, void* data, uint32_t dataLength)
{
	if (id > TOOL_GETARRLEN(infos) - 1) return 0;
	uint32_t realDataLength = dataLength > infos[id].size ? infos[id].size : dataLength;
	Flash_Read(infos[id]._address, data, realDataLength);
	return realDataLength;
}

/**
 * 存储模块初始化
 */
void Storage_Init()
{
	uint32_t startAddress = 0;
	/* 计算所有存储块的地址 */
	for (size_t i = 0;i<TOOL_GETARRLEN(infos);i++) {
		infos[i]._address = startAddress;
		startAddress += infos[i].size;
		allDataLength += infos[i].size;
	}
	/* 越界了 */
	if (allDataLength > CONFIG_FLASH_SECTOR_SIZE) {
		Library_Error();
	}
	Flash_ConfigAllDataLength(allDataLength);
}

#endif
