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

/* DCT的SOF */
#define DCT_SOF 0xAA

/* DCT协议状态机 */
#define DCT_STATE_SOF         0
#define DCT_STATE_CMD_ID      1
#define DCT_STATE_LENGTH_HIGH 2
#define DCT_STATE_LENGTH_LOW  3
#define DCT_STATE_DATA  	  4
#define DCT_STATE_CRC8  	  5

void DCT_Communicate_Init();
void DCT_Communicate_Send(uint8_t cmdID, uint8_t* data, uint16_t dataLength);

#endif
#endif /* INC_DCT_COMMUNICATE_H_ */
