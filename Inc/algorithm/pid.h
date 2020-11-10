/*
 * pid.h
 *
 *  Created on: Oct 22, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_ALGORITHM_PID_H_
#define LIBRARY_ALGORITHM_PID_H_

/* 保存PID信息的数组序号常量 */
#define PID_LAST 0
#define PID_NOW 1

/**
 * PID 结构体
 */
typedef struct {
  /* p、i、d参数 */
  float p;
  float i;
  float d;

  /* 目标值、反馈值、误差值 */
  float set;
  float get;
  float err[2];

  /* p、i、d各项计算出的输出 */
  float pout;
  float iout;
  float dout;

  /* pid公式计算出的总输出 */
  float out;

  /* pid最大输出限制  */
  uint32_t maxOutput;

  /* pid积分输出项限幅 */
  uint32_t integralLimit;

} PID_Info;

/* 通过配置文件创建PID
 * @param name PID名称,需要在Config中存在CONFIG_PID_PID名称_xxx的参数
 * @return PID结构体
 * @note CONFIG_PID_PID名称_P                 PID中的P
 * 		 CONFIG_PID_PID名称_P                 PID中的I
 * 		 CONFIG_PID_PID名称_D                 PID中的D
 * 		 CONFIG_PID_PID名称_INTERGRAL_LIMIT   PID中的积分限幅
 * 		 CONFIG_PID_PID名称_MAX_OUTPUT        PID中的最大输出
 */
#define PID_CREATE_FROM_CONFIG(name, pid) \
	PID_Create(pid, CONFIG_PID_##name##_MAX_OUTPUT, CONFIG_PID_##name##_INTERGRAL_LIMIT, CONFIG_PID_##name##_P, CONFIG_PID_##name##_I, CONFIG_PID_##name##_D)


void PID_Create(PID_Info *pid, uint32_t maxOut, uint32_t intergralLimit, float kp, float ki, float kd);
float PID_Calc(PID_Info *pid, float get, float set);
void PID_Setup(PID_Info *pid, float kp, float ki, float kd);
void PID_Reset(PID_Info *pid);

#endif /* LIBRARY_ALGORITHM_PID_H_ */
