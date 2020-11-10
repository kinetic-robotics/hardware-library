/*
 * ramp.h
 *
 *  Created on: Oct 22, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_ALGORITHM_RAMP_H_
#define LIBRARY_ALGORITHM_RAMP_H_

typedef struct {
  int32_t count; /* 斜坡计数 */
  int32_t scale; /* 经过多少次达到预定值 */
} Ramp_Info;

void Ramp_Setup(Ramp_Info *ramp, int32_t scale);
void Ramp_Init(Ramp_Info *ramp, int32_t scale);
float Ramp_Calc(Ramp_Info *ramp);
void Ramp_Reset(Ramp_Info *ramp);

#endif /* LIBRARY_ALGORITHM_RAMP_H_ */
