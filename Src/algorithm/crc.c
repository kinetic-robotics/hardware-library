/*
 * crc.c
 *
 *  Created on: Nov 19, 2020
 *      Author: xqe2011
 */

#include <stdint.h>
#include "Library/Inc/algorithm/crc.h"
#include "Configurations/library_config.h"

static const uint8_t  CRC8_Table[]  = CRC_CRC8_TABLE; /* 查表法算CRC8 */
static const uint16_t CRC16_Table[] = CRC_CRC16_TABLE;/* 查表法算CRC16 */

/**
 * 计算CRC8校验和,需要提供初始值
 * @param data 数据指针
 * @param dataLength 数据长度
 * @param initValue CRC8初始值
 * @return CRC8校验和
 */
uint8_t CRC_GetCRC8CheckSumWithInitValue(uint8_t* data, uint32_t dataLength, uint8_t initValue)
{
	uint8_t index;
	uint8_t value = initValue;
	while (dataLength--) {
		index = value ^ (*data++);
		value = CRC8_Table[index];
	}
	return value;
}

/**
 * 计算CRC8校验和,使用默认初始值
 * @param data 数据指针
 * @param dataLength 数据长度
 * @return CRC8校验和
 */
uint8_t CRC_GetCRC8CheckSum(uint8_t* data, uint32_t dataLength)
{
	return CRC_GetCRC8CheckSumWithInitValue(data, dataLength, CONFIG_CRC_CRC8_DEFAULT_INIT_VALUE);
}

/**
 * 计算CRC16校验和,需要提供初始值
 * @param data 数据指针
 * @param dataLength 数据长度
 * @param initValue CRC16初始值
 * @return CRC16校验和
 */
uint16_t CRC_GetCRC16CheckSumWithInitValue(uint8_t* data, uint32_t dataLength, uint16_t initValue)
{
	uint16_t value = initValue;
	while(dataLength--) {
		value = (value >> 8) ^ CRC16_Table[(value ^ (*data++)) & 0x00ff];
	}
	return value;
}

/**
 * 计算CRC16校验和,使用默认初始值
 * @param data 数据指针
 * @param dataLength 数据长度
 * @param initValue CRC16初始值
 * @return CRC16校验和
 */
uint16_t CRC_GetCRC16CheckSum(uint8_t* data, uint32_t dataLength)
{
	return CRC_GetCRC16CheckSumWithInitValue(data, dataLength, CONFIG_CRC_CRC16_DEFAULT_INIT_VALUE);
}
