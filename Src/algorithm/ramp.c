/***************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

/************************** (C) COPYRIGHT 2018 DJI *************************
 * @update
 * @history
 * Version     Date              Author           Modification
 * V1.0.0      January-15-2018   ric.luo
 * @verbatim
 ************************** (C) COPYRIGHT 2018 DJI *************************/

#include <stdint.h>
#include "Library/Inc/algorithm/ramp.h"

/**
 * 斜坡重置函数
 */
void Ramp_Reset(Ramp_Info *ramp)
{
	ramp->count = 0;
}

/**
 * 斜坡配置函数,会同时重置斜坡参数
 * @param ramp  斜坡数据结构体指针
 * @param scale 经过多少次达到预定值
 */
void Ramp_Setup(Ramp_Info *ramp, int32_t scale)
{
	Ramp_Reset(ramp);
	ramp->scale = scale;
}

/**
 * 斜坡控制结构体初始化
 * @param ramp  斜坡数据结构体指针
 * @param scale 经过多少次达到预定值
 */
void Ramp_Init(Ramp_Info *ramp, int32_t scale)
{
	Ramp_Setup(ramp, scale);
}

/**
 * 斜坡控制计算函数
 * @param ramp 斜坡数据结构体指针
 * @retval     斜坡控制计算输出
 */
float Ramp_Calc(Ramp_Info *ramp)
{
	if (ramp->scale <= 0)
		return 0;
  
	if (ramp->count++ >= ramp->scale)
		ramp->count = ramp->scale;
  
	return ramp->count / ((float)ramp->scale);
}
