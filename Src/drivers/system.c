/*
 * system.c
 *
 *  Created on: Oct 22, 2020
 *      Author: xqe2011
 */

#include "Library/Inc/drivers/system.h"
#include "stm32f4xx_hal.h"

/**
 * 延时函数
 * @param delay 延时值,单位为毫秒
 */
void System_Delay(uint32_t delay)
{
	HAL_Delay(delay);
}

/**
 * 系统初始化
 */
void System_Init()
{

}
