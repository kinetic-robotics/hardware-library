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
 * 斜坡控制结构体初始化
 * @param ramp  斜坡数据结构体指针
 * @param scale 经过多少次达到预定值
 * @param now   现在的启动值
 */
void Ramp_Init(Ramp_Info *ramp, int32_t scale, float now)
{
	Ramp_Reset(ramp);
	ramp->scale = scale;
	ramp->now = now;
}

/**
 * 斜坡控制计算函数
 * @param ramp   斜坡数据结构体指针
 * @param target 目标值
 * @return 斜坡控制计算输出
 */
float Ramp_Calc(Ramp_Info *ramp, float target)
{
	if (ramp->lastTarget != target) {
		ramp->count = 0;
		ramp->lastTarget = target;
		ramp->everyInc = (ramp->lastTarget - ramp->now) / ramp->scale;
	}
	return Ramp_RunCalc(ramp);
}

/**
 * 斜坡控制计算函数,该函数只用于计算,不可修改目标值
 * @param ramp   斜坡数据结构体指针
 * @return 斜坡控制计算输出
 */
float Ramp_RunCalc(Ramp_Info *ramp)
{
	if (ramp->count++ >= ramp->scale) {
		ramp->count = ramp->scale;
	}
	ramp->now += ramp->everyInc;
	return ramp->now;
}
