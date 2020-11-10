/*
 * storage.h
 *
 *  Created on: Oct 28, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_STORAGE_H_
#define LIBRARY_STORAGE_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_STORAGE_ENABLE

#include <stdint.h>

typedef struct {
	uint32_t _address;  /* 计算出来的内存地址, */
	uint32_t size;      /* 大小 */
} Storage_Info;

void Storage_Write(uint32_t id, void* data, uint32_t dataLength);
uint32_t Storage_Read(uint32_t id, void* data, uint32_t dataLength);
void Storage_Init();

#endif
#endif /* LIBRARY_STORAGE_H_ */
