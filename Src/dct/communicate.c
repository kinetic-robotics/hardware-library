/*
 * communicate.c
 *
 *  Created on: Nov 23, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DCT_ENABLE

#include "Library/Inc/dct/communicate.h"

static uint8_t flag = DCT_STATE_SOF; /* USB接收数据状态机 */

/**
 * 收到USB数据回调
 */
static void DCT_Communicate_USBRxCallback(uint8_t* data, uint16_t dataLength)
{
	for(size_t i = 0;i<dataLength;i++) {
		switch(flag) {
			case DCT_STATE_SOF:
				break;
			case DCT_STATE_CMD_ID:
				break;
			case DCT_STATE_LENGTH:
				break;
			case DCT_STATE_DATA:
				break;
			case DCT_STATE_CRC8:
				break;
		}
	}
}

/**
 * 调试与配置工具初始化
 */
void DCT_Communicate_Init()
{

}

#endif
