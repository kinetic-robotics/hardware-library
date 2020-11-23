/*
 * pid.c
 *
 *  Created on: Nov 23, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DCT_ENABLE

#include "Library/Inc/tool.h"
#include <Library/Inc/dct/pid.h>
#include "Library/Inc/algorithm/pid.h"

static DCT_PID_Info infos[CONFIG_DCT_PID_MAX_LENGTH]; /* PID结构体信息 */
static uint8_t pidLength = 0;						   	  /* 当前PID结构体数组长度 */

/**
 * 通过上位机显示PID参数
 * @param name 名称,将显示在DCT上
 * @param pid  PID结构体指针
 */
void DCT_PID_Show(char* name, PID_Info* pid)
{
	#ifndef CONFIG_DCT_PID_ENABLE
		return;
	#endif
	if (pidLength > TOOL_GET_ARRAY_LENGTH(infos) - 1) return;
	infos[pidLength].name = name;
	infos[pidLength].pid  = pid;
	pidLength++;
}

/**
 * 发送PID数据包
 */
void DCT_PID_SendPacket()
{

}

/**
 * 生成PID初始化数据包内容
 * @param data 要写入数据的头指针
 * @param dataLength 最大长度
 * @return length 实际写入长度
 */
uint16_t DCT_PID_GenerateInitPacket(uint8_t data, uint16_t dataLength)
{
	return 0;
}

#endif
