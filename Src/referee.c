/*
 * referee.c
 *
 *  Created on: Nov 11, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_REFEREE_ENABLE

#include <stdint.h>
#include "Library/Inc/tool.h"
#include "Library/Inc/referee.h"
#include "Library/Inc/drivers/uart.h"
#include "Library/Inc/algorithm/crc.h"
#include "cmsis_os.h"
#include <string.h>

static QueueHandle_t uartQueue;/* 串口接收队列 */
static Referee_Info  info;     /* 裁判系统数据结构体 */
static Referee_Callback callbacks[CONFIG_REFEREE_CALLBACKS_MAX];    /* 回调函数信息 */
static uint8_t callbacksLength = 0;		                            /* 回调函数数组长度 */
static void Referee_RegisterCallback(uint8_t type, void* callback); /* 提前声明工具函数 */

/* ---------------------------------以下开始裁判系统各个类型数据包的解析的调用回调,根据2020版本规则V1.1协议编写--------------------------------- */

/**
 * 裁判系统数据解析, 比赛状态数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedGameState(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_GAME_STATUS && dataLength != 3) return;
	info.gameType  = data[0] & 0xF;
	info.gameStage = data[0] >> 4;
	memcpy(&info.gameRemainTime, &data[1], 2);
}

/**
 * 裁判系统数据解析, 比赛结果数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedGameResult(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_GAME_RESULT && dataLength != 1) return;
	for (size_t i = 0;i<callbacksLength;i++) {
		if (callbacks[i].type == REFEREE_CALLBACK_TYPE_GAME_RESULT) {
			((Referee_ReceivedGameResultCallback)callbacks[i].callback)(*data);
		}
	}
}

/**
 * 裁判系统数据解析, 机器人血量数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedHP(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_HP && dataLength != 32) return;
	uint8_t* ourData;
	uint8_t* enemyData;
	if (info.robotGroup == REFEREE_ROBOT_GROUP_RED) {
		ourData = data;
		enemyData = data + 16;
	} else {
		ourData = data + 16;
		enemyData = data;
	}
	memcpy(&info.our1HP, &ourData[0], 2);
	memcpy(&info.our2HP, &ourData[2], 2);
	memcpy(&info.our3HP, &ourData[4], 2);
	memcpy(&info.our4HP, &ourData[6], 2);
	memcpy(&info.our5HP, &ourData[8], 2);
	memcpy(&info.our7HP, &ourData[10], 2);
	memcpy(&info.ourOutpostHP, &ourData[12], 2);
	memcpy(&info.ourBaseHP, &ourData[14], 2);

	memcpy(&info.enemy1HP, &enemyData[0], 2);
	memcpy(&info.enemy2HP, &enemyData[2], 2);
	memcpy(&info.enemy3HP, &enemyData[4], 2);
	memcpy(&info.enemy4HP, &enemyData[6], 2);
	memcpy(&info.enemy5HP, &enemyData[8], 2);
	memcpy(&info.enemy7HP, &enemyData[10], 2);
	memcpy(&info.enemyOutpostHP, &enemyData[12], 2);
	memcpy(&info.enemyBaseHP, &enemyData[14], 2);
}

/**
 * 裁判系统数据解析, 飞镖发射状态数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedDartStatus(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_DART_STATUS && dataLength != 3) return;
	for (size_t i = 0;i<callbacksLength;i++) {
		if (callbacks[i].type == REFEREE_CALLBACK_TYPE_DART_STATUS) {
			((Referee_ReceivedDartStatusCallback)callbacks[i].callback)(data[0], (data[1] << 8) | data[2]);
		}
	}
}

/**
 * 裁判系统数据解析, 人工智能挑战赛加成与惩罚区状态
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedICRABuffStatus(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_ICRA_BUFF_STATUS && dataLength != 3) return;
	info.f1Status     = TOOL_GET_BIT(data[0], 0);
	info.f1BuffStatus = (data[0] >> 1) & 0x3;
	info.f2Status     = TOOL_GET_BIT(data[0], 4);
	info.f2BuffStatus = (data[0] >> 5) & 0x3;
	info.f3Status     = TOOL_GET_BIT(data[0], 8);
	info.f3BuffStatus = (data[0] >> 9) & 0x3;
	info.f4Status     = TOOL_GET_BIT(data[0], 12);
	info.f4BuffStatus = (data[0] >> 13) & 0x3;
	info.f5Status     = TOOL_GET_BIT(data[0], 16);
	info.f5BuffStatus = (data[0] >> 17) & 0x3;
	info.f6Status     = TOOL_GET_BIT(data[0], 20);
	info.f6BuffStatus = (data[0] >> 21) & 0x3;
}

/**
 * 裁判系统数据解析, 场地事件数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedEvent(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_EVENT && dataLength != 4) return;
	info.parkingState = data[0] & 0x03;
	info.energySmallState = TOOL_GET_BIT(data[0], 2);
	info.energyBigState   = TOOL_GET_BIT(data[0], 3);
	info.shieldState      = TOOL_GET_BIT(data[0], 4);
}

/**
 * 裁判系统数据解析, 补给站动作数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedSupplyAction(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_DART_STATUS && dataLength != 4) return;
	for (size_t i = 0;i<callbacksLength;i++) {
		if (callbacks[i].type == REFEREE_CALLBACK_TYPE_DART_STATUS) {
			((Referee_ReceivedSupplyActionCallback)callbacks[i].callback)(data[1] > 100 ? REFEREE_ROBOT_GROUP_BLUE : REFEREE_ROBOT_GROUP_RED, data[1] > 100 ? data[1] - 100 : data[1], data[0], data[2], data[3]);
		}
	}
}


/**
 * 裁判系统数据解析, 裁判警告数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedWarning(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_WARNING && dataLength != 2) return;
	for (size_t i = 0;i<callbacksLength;i++) {
		if (callbacks[i].type == REFEREE_CALLBACK_TYPE_WARNING) {
			((Referee_ReceivedWarningCallback)callbacks[i].callback)(data[0], data[1]);
		}
	}
}

/**
 * 裁判系统数据解析, 飞镖发射口倒计时数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedDartRemainTime(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_DART_REMAIN_TIME && dataLength != 1) return;
	info.dartRemainTime = data[0];
}

/**
 * 裁判系统数据解析, 比赛机器人状态数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedRobotStatus(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_ROBOT_STATUS && dataLength != 18) return;
	if (data[0] > 100) {
		info.robotID = data[0] - 100;
		info.robotGroup = REFEREE_ROBOT_GROUP_BLUE;
	} else {
		info.robotID = data[0];
		info.robotGroup = REFEREE_ROBOT_GROUP_RED;
	}
	info.robotLevel = data[1];
	memcpy(&info.remainHP, &data[2], 2);
	memcpy(&info.maxHP, &data[4], 2);
	memcpy(&info.shooter17CoolingHeatRate, &data[6], 2);
	memcpy(&info.shooter17MaxHeat, &data[8], 2);
	memcpy(&info.shooter42CoolingHeatRate, &data[10], 2);
	memcpy(&info.shooter42MaxHeat, &data[12], 2);
	info.shooter17MaxSpeed = data[14];
	info.shooter42MaxSpeed = data[15];
	info.chassisMaxPower   = data[16];
	info.isGimbalEnable    = data[17] & 0x01;
	info.isChassisEnable   = (data[17] >> 1) & 0x01;
	info.isShooterEnable   = (data[17] >> 2) & 0x01;
}

/**
 * 裁判系统数据解析, 实时功率热量数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedHeat(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_HEAT && dataLength != 16) return;
	info.chassisOutputVoltage = (data[0] << 8 | data[1]) / 1000;
	info.chassisOutputCurrent = (data[2] << 8 | data[3]) / 1000;
	memcpy(&info.chassisOutputPower, &data[4], 4);
	memcpy(&info.chassisPowerBuffer, &data[8], 2);
	memcpy(&info.shooter17Heat, &data[10], 2);
	memcpy(&info.shooter42Heat, &data[12], 2);
	memcpy(&info.shooterMobile17Heat, &data[14], 2);
}

/**
 * 裁判系统数据解析, 机器人位置数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedPosition(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_POSITION && dataLength != 16) return;
	memcpy(&info.x, &data[0], 4);
	memcpy(&info.y, &data[4], 4);
	memcpy(&info.z, &data[8], 4);
	memcpy(&info.shooterYaw, &data[12], 4);
}

/**
 * 裁判系统数据解析, 机器人增益数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedBuff(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_BUFF && dataLength != 1) return;
	info.isGotComplementHPBuff      = TOOL_GET_BIT(data[0], 0);
	info.isGotCoolingHeatFasterBuff = TOOL_GET_BIT(data[0], 1);
	info.isGotDefenseBuff           = TOOL_GET_BIT(data[0], 2);
	info.isGotAttackBuff            = TOOL_GET_BIT(data[0], 3);
}

/**
 * 裁判系统数据解析, 空中机器人能量数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedDroneEnergy(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_DRONE_ENERGY && dataLength != 3) return;
	memcpy(&info.droneEnergyPoint, &data[0], 2);
	info.droneRemainAttackTime = data[2];
}

/**
 * 裁判系统数据解析, 伤害数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedHurt(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_HURT && dataLength != 1) return;
	for (size_t i = 0;i<callbacksLength;i++) {
		if (callbacks[i].type == REFEREE_CALLBACK_TYPE_HURT) {
			((Referee_ReceivedHurtCallback)callbacks[i].callback)(data[0] >> 4, data[0] & 0xF);
		}
	}
}

/**
 * 裁判系统数据解析, 实时射击数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedShoot(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_SHOOT && dataLength != 6) return;
	float speed = 0;
	memcpy(&speed, data + 2, 4);
	for (size_t i = 0;i<callbacksLength;i++) {
		if (callbacks[i].type == REFEREE_CALLBACK_TYPE_SHOOT) {
			((Referee_ReceivedShootCallback)callbacks[i].callback)(data[0], data[1], speed);
		}
	}
}

/**
 * 裁判系统数据解析, 子弹剩余发射数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedRemainBullet(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_REMAIN_BULLET && dataLength != 2) return;
	memcpy(&info.remainingBullet, &data[0], 2);
}

/**
 * 裁判系统数据解析, 机器人RFID状态数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedRFID(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_RFID && dataLength != 4) return;
	info.rfidBase 		 = TOOL_GET_BIT(data[0], 0);
	info.rfidHightland 	 = TOOL_GET_BIT(data[0], 1);
	info.rfidEnergy 	 = TOOL_GET_BIT(data[0], 2);
	info.rfidSlope 		 = TOOL_GET_BIT(data[0], 3);
	info.rfidOutpost 	 = TOOL_GET_BIT(data[0], 4);
	info.rfidResource 	 = TOOL_GET_BIT(data[0], 5);
	info.rfidHP 		 = TOOL_GET_BIT(data[0], 6);
	info.rfidEngineering = TOOL_GET_BIT(data[0], 7);
	info.rfidF1   		 = TOOL_GET_BIT(data[0], 26);
	info.rfidF2 		 = TOOL_GET_BIT(data[0], 27);
	info.rfidF3 		 = TOOL_GET_BIT(data[0], 28);
	info.rfidF4 		 = TOOL_GET_BIT(data[0], 29);
	info.rfidF5 		 = TOOL_GET_BIT(data[0], 30);
	info.rfidF6 		 = TOOL_GET_BIT(data[0], 31);
}

/**
 * 裁判系统数据解析, 飞镖机器人客户端指令数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedDartCMD(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_DART_CMD && dataLength != 12) return;
	info.dartOpeningStatus = data[0];
	info.dartTarget 	   = data[1];
	memcpy(&info.dartChangeTargetTime, &data[2], 2);
	info.dartFirstSpeed    = data[4];
	info.dartSecondSpeed   = data[5];
	info.dartThirdSpeed    = data[6];
	info.dartFourthSpeed   = data[7];
	memcpy(&info.dartLastLaunchRemainTime, &data[8], 2);
	memcpy(&info.dartLastOperateLaunchTime, &data[10], 2);
}

/**
 * 裁判系统数据解析, 机器人间交互数据
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedInteractive(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	if (cmdID == REFEREE_CMD_INTERACTIVE && dataLength > 6) return;
	/* 忽略接受者不是自己的信息 */
	uint16_t robotID = info.robotGroup == REFEREE_ROBOT_GROUP_RED ? 0 : 100;
	robotID += info.robotID;
	if (((data[4] << 8) | data[5]) != robotID) return;
	for (size_t i = 0;i<callbacksLength;i++) {
		if (callbacks[i].type == REFEREE_CALLBACK_TYPE_INTERACTIVE) {
			((Referee_ReceivedInteractiveCallback)callbacks[i].callback)((data[0] << 8) | data[1], (data[2] << 8) | data[3], data + 6, dataLength - 6);
		}
	}
}

/**
 * 裁判系统数据解析
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
static void Referee_ParsedData(uint16_t cmdID, uint8_t* data, uint16_t dataLength)
{
	Referee_ParsedGameState(cmdID, data, dataLength);
	Referee_ParsedGameResult(cmdID, data, dataLength);
	Referee_ParsedHP(cmdID, data, dataLength);
	Referee_ParsedDartStatus(cmdID, data, dataLength);
	Referee_ParsedICRABuffStatus(cmdID, data, dataLength);
	Referee_ParsedEvent(cmdID, data, dataLength);
	Referee_ParsedSupplyAction(cmdID, data, dataLength);
	Referee_ParsedWarning(cmdID, data, dataLength);
	Referee_ParsedDartRemainTime(cmdID, data, dataLength);
	Referee_ParsedRobotStatus(cmdID, data, dataLength);
	Referee_ParsedHeat(cmdID, data, dataLength);
	Referee_ParsedPosition(cmdID, data, dataLength);
	Referee_ParsedBuff(cmdID, data, dataLength);
	Referee_ParsedDroneEnergy(cmdID, data, dataLength);
	Referee_ParsedHurt(cmdID, data, dataLength);
	Referee_ParsedShoot(cmdID, data, dataLength);
	Referee_ParsedRemainBullet(cmdID, data, dataLength);
	Referee_ParsedRFID(cmdID, data, dataLength);
	Referee_ParsedDartCMD(cmdID, data, dataLength);
	Referee_ParsedInteractive(cmdID, data, dataLength);
	for (size_t i = 0;i<callbacksLength;i++) {
		if (callbacks[i].type == REFEREE_CALLBACK_TYPE_ALL) {
			((Referee_ReceivedMessageCallback)callbacks[i].callback)(cmdID, data, dataLength);
		}
	}
}

/**
 * 注册当收到裁判系统发送数据时的回调
 * @param callback 回调
 */
void Referee_RegisterReceivedMessagesCallback(Referee_ReceivedMessageCallback callback)
{
	Referee_RegisterCallback(REFEREE_CALLBACK_TYPE_ALL, (void*)callback);
}

/**
 * 注册当收到比赛结果数据的回调
 * @param callback 回调
 */
void Referee_RegisterReceivedGameResultCallback(Referee_ReceivedGameResultCallback callback)
{
	Referee_RegisterCallback(REFEREE_CALLBACK_TYPE_GAME_RESULT, (void*)callback);
}

/**
 * 注册当收到飞镖发射状态数据的回调
 * @param callback 回调
 */
void Referee_RegisterReceivedDartStatusCallback(Referee_ReceivedDartStatusCallback callback)
{
	Referee_RegisterCallback(REFEREE_CALLBACK_TYPE_DART_STATUS, (void*)callback);
}

/**
 * 注册当收到场地补给站动作标识数据的回调
 * @param callback 回调
 */
void Referee_RegisterReceivedSupplyActionCallback(Referee_ReceivedSupplyActionCallback callback)
{
	Referee_RegisterCallback(REFEREE_CALLBACK_TYPE_SUPPLY_ACTION, (void*)callback);
}

/**
 * 注册当收到裁判警告数据的回调
 * @param callback 回调
 */
void Referee_RegisterReceivedWarningCallback(Referee_ReceivedWarningCallback callback)
{
	Referee_RegisterCallback(REFEREE_CALLBACK_TYPE_WARNING, (void*)callback);
}

/**
 * 注册当收到伤害状态数据的回调
 * @param callback 回调
 */
void Referee_RegisterReceivedHurtCallback(Referee_ReceivedHurtCallback callback)
{
	Referee_RegisterCallback(REFEREE_CALLBACK_TYPE_HURT, (void*)callback);
}

/**
 * 注册当收到实时射击数据的回调
 * @param callback 回调
 */
void Referee_RegisterReceivedShootCallback(Referee_ReceivedShootCallback callback)
{
	Referee_RegisterCallback(REFEREE_CALLBACK_TYPE_SHOOT, (void*)callback);
}

/**
 * 注册当收到机器人间交互数据的回调
 * @param callback 回调
 */
void Referee_RegisterReceivedInteractiveCallback(Referee_ReceivedInteractiveCallback callback)
{
	Referee_RegisterCallback(REFEREE_CALLBACK_TYPE_INTERACTIVE, (void*)callback);
}

/* ---------------------------------以上结束裁判系统各个类型数据包的解析和调用回调,根据2020版本规则V1.1协议编写--------------------------------- */

/**
 * 注册回调
 * @param type 回调类型,参考REFEREE_CALLBACK_TYPE宏
 * @param callback 回调函数
 */
static void Referee_RegisterCallback(uint8_t type, void* callback)
{
	if (callbacksLength > TOOL_GET_ARRAY_LENGTH(callbacks) - 1) return;
	callbacks[callbacksLength].type = type;
	callbacks[callbacksLength].callback = callback;
	callbacksLength++;
}

/**
 * 串口接收回调
 * @param id 串口ID
 * @param data 数据
 * @param dataLength 长度
 */
static void Referee_UARTRxCallback(uint8_t id, uint8_t* data, uint16_t dataLength)
{
	if (id != CONFIG_REFEREE_UART_ID) return;
	/* 为了高效,不在中断里解析数据 */
	for (size_t i = 0;i<dataLength;i++) {
		xQueueSendFromISR(uartQueue, &data[i], NULL);
	}
}

/**
 * 裁判系统通信任务
 */
static void Referee_Task()
{
	/* 裁判系统状态机 */
	uint8_t flag = REFEREE_STATE_HEADER_SOF;
	/* 头部解析出来的数据长度 */
	uint16_t dataLength = 0;
	/* 头部解析出来的指令ID */
	uint16_t cmdID = 0;
	/* 已经接收的数据长度 */
	uint16_t receivedDataLength = 0;
	/* 尾部CRC */
	uint16_t tailCRC = 0;
	/* 整个数据包暂存区 */
	uint8_t recvData[CONFIG_REFEREE_DATA_MAX_LENGTH] = {0};
	while(1) {
		uint8_t data = 0;
		if (xQueueReceive(uartQueue, &data, portMAX_DELAY) == pdTRUE) {
			/* 裁判系统数据处理函数 */
			recvData[receivedDataLength++] = data;
			switch (flag) {
				case REFEREE_STATE_HEADER_SOF:
					/* 初始化状态机变量 */
					dataLength = 0;
					cmdID = 0;
					receivedDataLength = 0;
					tailCRC = 0;
					if (data == REFEREE_HEADER_SOF) {
						flag = REFEREE_STATE_HEADER_LENGTH_HIGH;
					}
					break;
				case REFEREE_STATE_HEADER_LENGTH_HIGH:
					dataLength  = data << 8;
					flag = REFEREE_STATE_HEADER_LENGTH_LOW;
					break;
				case REFEREE_STATE_HEADER_LENGTH_LOW:
					dataLength |= data;
					flag = REFEREE_STATE_HEADER_CRC;
					break;
				case REFEREE_STATE_HEADER_CRC:
					if (CRC_GetCRC8CheckSum(recvData, 5) == data) {
						flag = REFEREE_STATE_CMD_ID_HIGH;
					} else {
						flag = REFEREE_STATE_HEADER_SOF;
					}
					break;
				case REFEREE_STATE_CMD_ID_HIGH:
					cmdID  = data << 8;
					flag = REFEREE_STATE_CMD_ID_HIGH;
					break;
				case REFEREE_STATE_CMD_ID_LOW:
					cmdID |= data;
					flag = REFEREE_STATE_HEADER_LENGTH_LOW;
					break;
				case REFEREE_STATE_DATA:
					if (receivedDataLength == dataLength + 7) {
						flag = REFEREE_STATE_TAIL_CRC_HIGH;
					}
					break;
				case REFEREE_STATE_TAIL_CRC_HIGH:
					tailCRC  = data << 8;
					flag = REFEREE_STATE_TAIL_CRC_LOW;
					break;
				case REFEREE_STATE_TAIL_CRC_LOW:
					tailCRC |= data;
					if (CRC_GetCRC16CheckSum(recvData, dataLength + 9) == tailCRC) {
						/* 只传递数据段 */
						Referee_ParsedData(cmdID, recvData + 7, receivedDataLength - 2);
					}
					flag = REFEREE_STATE_HEADER_SOF;
					break;
			}
		}
	}
}

/**
 * 裁判系统通信初始化
 */
void Referee_Init()
{
	/* 创建消息队列 */
	uartQueue = xQueueCreate(CONFIG_REFEREE_MSGS_LENGTH, sizeof(uint8_t));
	/* 创建任务 */
	static osThreadId_t refereeTaskHandle;
	const osThreadAttr_t refereeTaskAttributes = {
			.name = "refereeTask",
			.priority = (osPriority_t) osPriorityHigh7,
			.stack_size = 128 * 4
	};
	refereeTaskHandle = osThreadNew(Referee_Task, NULL, &refereeTaskAttributes);
	UNUSED(refereeTaskHandle);
	UART_RegisterCallback(&Referee_UARTRxCallback);
}

#endif
