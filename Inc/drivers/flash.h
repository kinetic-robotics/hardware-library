/*
 * flash.h
 *
 *  Created on: Oct 23, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_FLASH_H_
#define LIBRARY_FLASH_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_FLASH_ENABLE

#include <stdint.h>

void Flash_Write(uint32_t address, void* data, uint32_t dataLength);
void Flash_Read(uint32_t address, void* data, uint32_t dataLength);
void Flash_ConfigAllDataLength(uint32_t size);
void Flash_Init();

#endif
#endif /* LIBRARY_FLASH_H_ */
