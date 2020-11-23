/*
 * ramp.h
 *
 *  Created on: Oct 22, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_ALGORITHM_RAMP_H_
#define LIBRARY_ALGORITHM_RAMP_H_

typedef struct {
  uint16_t count; 	  /* 斜坡计数 */
  uint16_t scale; 	  /* 经过多少次达到预定值 */
  float    everyInc;  /* 每次增加的值 */
  float    now;   	  /* 当前值 */
  float    lastTarget;/* 目标值 */
} Ramp_Info;

void Ramp_Init(Ramp_Info *ramp, int32_t scale, float now);
float Ramp_Calc(Ramp_Info *ramp, float target);
float Ramp_RunCalc(Ramp_Info *ramp);
void Ramp_Reset(Ramp_Info *ramp);

#endif /* LIBRARY_ALGORITHM_RAMP_H_ */
