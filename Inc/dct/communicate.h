/*
 * communicate.h
 *
 *  Created on: Nov 23, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_DCT_COMMUNICATE_H_
#define LIBRARY_DCT_COMMUNICATE_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_DCT_ENABLE

/* DCT协议状态机 */
#define DCT_STATE_SOF    0
#define DCT_STATE_CMD_ID 1
#define DCT_STATE_LENGTH 2
#define DCT_STATE_DATA   3
#define DCT_STATE_CRC8   4

void DCT_Communicate_Init();

#endif
#endif /* INC_DCT_COMMUNICATE_H_ */
