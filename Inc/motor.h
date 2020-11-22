/*
 * motor.h
 *
 *  Created on: Oct 20, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_MOTOR_H_
#define LIBRARY_MOTOR_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_MOTOR_ENABLE

#include "cmsis_os.h"
#include "semphr.h"
#include "Library/Inc/algorithm/ramp.h"

#define MOTOR_FILTER_BUFFER_SIZE 5

/* 电机类型 */
#define MOTOR_TYPE_RM3508 1
#define MOTOR_TYPE_RM2006 2
#define MOTOR_TYPE_RM6020 3
#define MOTOR_TYPE_RM2312 4

/* 电机电流范围 */
/* 虚拟最大电流范围 */
#define MOTOR_VIRTUAL_ABS_MAX     	 10000.0f
/* RM3508电机最大电流范围绝对值 */
#define MOTOR_TYPE_RM3508_ABS_MAX 	 16384.0f
/* RM2006电机最大电流范围绝对值 */
#define MOTOR_TYPE_RM2006_ABS_MAX 	 10000.0f
/* RM6020电机最大电流范围绝对值 */
#define MOTOR_TYPE_RM6020_ABS_MAX 	 30000.0f
/* RM2312电机最大电流范围绝对值 */
#define MOTOR_TYPE_RM2312_ABS_MAX 	 60.0f

/* RM2312电机中值 */
#define MOTOR_TYPE_RM2312_MID 	  	 0.0f

/* RM2312电机控制斜坡次数 */
#define MOTOR_TYPE_RM2312_RAMP_SCALE 300

typedef struct {
	uint8_t  canNum;      /* CAN编号,可取值为CAN_1,CAN_2 */
	uint8_t  id;          /* 电机ID */
	uint8_t  state;       /* 电机状态,0为禁用,1为启动 */
	uint8_t  type;        /* 电机类型,可取值为MOTOR_TYPE_ */
	uint8_t  enableOffset;/* 是否将电机发送的前50帧作为offset,0为禁用,1为启动 */
	/* 以下是电机电调直接回传的数据 */

	uint16_t ecd;         /* 电机的编码器数值 */
	uint16_t lastEcd;     /* 上一次电机的编码器数值 */
	int16_t  speedRpm;    /* 电机的转速值 */

	/* 以下是计算出来的电机相关数据 */
	int32_t  roundCount;  /* 电机旋转的总圈数 */
	int32_t  totalEcd;    /* 电机旋转的总编码器数值 */
	int32_t  totalAngle;  /* 电机旋转的总角度 */

	/* 以下电机计算相关数据时的中间变量，可以忽略 */
	uint16_t _offsetEcd;
	uint32_t _msgCount;
	int32_t  _ecdRawRate;
	int32_t  _rateBuf[MOTOR_FILTER_BUFFER_SIZE];
	uint8_t  _bufCut;
	int32_t  _filterRate;
	int32_t  _lastCurrent; /* 上次传入参数电流,用于判定电流是否更新 */
	Ramp_Info _ramp;       /* 斜坡 */

	SemaphoreHandle_t lock; /* 锁 */
} Motor_Info;

void Motor_Set(uint8_t id, int16_t current);
void Motor_Init();
const Motor_Info* Motor_GetMotorData(uint8_t id);
void Motor_on();
void Motor_off();

#endif
#endif /* LIBRARY_MOTOR_H_ */
