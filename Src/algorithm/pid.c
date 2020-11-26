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
#include "Library/Inc/tool.h"
#include "Library/Inc/algorithm/pid.h"

/**
 * PID 初始化函数
 * @param pid: PID 结构体
 * @param maxOut: 最大输出
 * @param intergralLimit: 积分限幅
 * @param kp 具体 PID 参数
 * @param ki 具体 PID 参数
 * @param kd 具体 PID 参数
 */
void PID_Create(PID_Info *pid, uint32_t maxOut, uint32_t intergralLimit, float kp, float ki, float kd)
{
	pid->integralLimit = intergralLimit;
	pid->maxOutput     = maxOut;

	pid->p = kp;
	pid->i = ki;
	pid->d = kd;
}

/**
 * PID 计算函数，使用位置式 PID 计算
 * @param pid PID 结构体
 * @param get 反馈数据
 * @param set 目标数据
 * @retval PID 计算输出
 */
float PID_Calc(PID_Info *pid, float get, float set)
{
	pid->get = get;
	pid->set = set;
	pid->err[PID_NOW] = set - get;
  
	pid->pout = pid->p * pid->err[PID_NOW];
	pid->iout += pid->i * pid->err[PID_NOW];
	pid->dout = pid->d * (pid->err[PID_NOW] - pid->err[PID_LAST]);

	TOOL_ABS_LIMIT(pid->iout, (float)pid->integralLimit);
	pid->out = pid->pout + pid->iout + pid->dout;
	TOOL_ABS_LIMIT(pid->out,  (float)pid->maxOutput);

	pid->err[PID_LAST]  = pid->err[PID_NOW];
  
	return pid->out;
}

/**
 * PID 参数复位函数
 * @param pid: PID 结构体
 * @param kp 具体 PID 参数
 * @param ki 具体 PID 参数
 * @param kd 具体 PID 参数
 */
void PID_Setup(PID_Info *pid, float kp, float ki, float kd)
{
	pid->p = kp;
	pid->i = ki;
	pid->d = kd;

	PID_Reset(pid);
}

/**
 * PID 参数复位函数
 * @param pid: PID 结构体
 */
void PID_Reset(PID_Info *pid)
{
	pid->pout = 0;
	pid->iout = 0;
	pid->dout = 0;
	pid->out  = 0;
}
