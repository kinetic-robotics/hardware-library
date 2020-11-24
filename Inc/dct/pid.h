/*
 * pid.h
 *
 *  Created on: Nov 23, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_DCT_PID_H_
#define LIBRARY_DCT_PID_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_DCT_ENABLE

#include "Library/Inc/algorithm/pid.h"

/* DCT发送PID数据指令ID */
#define DCT_PID_CMD_ID 0xA1

typedef struct {
	char*	  name; /* DCT名称 */
	PID_Info* pid;  /* PID结构体指针 */
} DCT_PID_Info;

void DCT_PID_Show(char* string, PID_Info* pid);
void DCT_PID_SendInitPacket();
void DCT_PID_SendPacket();

#endif
#endif /* INC_DCT_PID_H_ */
