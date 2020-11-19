/*
 * referee.h
 *
 *  Created on: Nov 11, 2020
 *      Author: xqe2011
 */

#ifndef INC_REFEREE_H_
#define INC_REFEREE_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_REFEREE_ENABLE

/* 指令ID */
/* 比赛状态数据 */
#define REFEREE_CMD_GAME_STATUS 	 0x0001
/* 比赛结果数据 */
#define REFEREE_CMD_GAME_RESULT		 0x0002
/* 比赛机器人血量数据 */
#define REFEREE_CMD_HP 				 0x0003
/* 飞镖发射状态 */
#define REFEREE_CMD_DART_STATUS      0x0004
/* 人工智能挑战赛加成与惩罚区状态 */
#define REFEREE_CMD_ICRA_BUFF_STATUS 0x0005
/* 场地事件数据 */
#define REFEREE_CMD_EVENT 			 0x0101
/* 场地补给站动作标识数据 */
#define REFEREE_CMD_SUPPLY_ACTION	 0x0102
/* 裁判警告数据 */
#define REFEREE_CMD_WARNING			 0x0104
/* 飞镖发射口倒计时 */
#define REFEREE_CMD_DART_REMAIN_TIME 0x0105
/* 机器人状态数据 */
#define REFEREE_CMD_ROBOT_STATUS 	 0x0201
/* 实时功率热量数据 */
#define REFEREE_CMD_HEAT 			 0x0202
/* 机器人位置数据 */
#define REFEREE_CMD_POSITION		 0x0203
/* 机器人增益数据 */
#define REFEREE_CMD_BUFF			 0x0204
/* 空中机器人能量状态数据 */
#define REFEREE_CMD_DRONE_ENERGY	 0x0205
/* 伤害状态数据 */
#define REFEREE_CMD_HURT			 0x0206
/* 实时射击数据 */
#define REFEREE_CMD_SHOOT			 0x0207
/* 弹丸剩余发射数 */
#define REFEREE_CMD_REMAIN_BULLET	 0x0208
/* 机器人 RFID 状态 */
#define REFEREE_CMD_RFID			 0x0209
/* 飞镖机器人客户端指令数据 */
#define REFEREE_CMD_DART_CMD 		 0x020A
/* 机器人间交互数据 */
#define REFEREE_CMD_INTERACTIVE		 0x0301



/* 比赛类型 */
/* RoboMaster 机甲大师赛 */
#define REFEREE_GAME_TYPE_ALL     1
/* RoboMaster 机甲大师单项赛 */
#define REFEREE_GAME_TYPE_SINGLE  2
/* ICRA RoboMaster 人工智能挑战赛 */
#define REFEREE_GAME_TYPE_ICRA    3

/* 比赛阶段 */
/* 未开始 */
#define REFEREE_GAME_STAGE_NOT_START     0
/* 准备 */
#define REFEREE_GAME_STAGE_READY 		 1
/* 自检 */
#define REFEREE_GAME_STAGE_CHECK         2
/* 5s倒计时 */
#define REFEREE_GAME_STAGE_5S_COUNTDOWN  3
/* 对战中 */
#define REFEREE_GAME_STAGE_BATTLING 	 4
/* 结算中 */
#define REFEREE_GAME_STOPPING	         5

/* 比赛结果宏 */
/* 平局 */
#define REFEREE_GAME_RESULT_DRAW  0
/* 红方 */
#define REFEREE_GAME_RESULT_RED   1
/* 蓝方 */
#define REFEREE_GAME_RESULT_BLUE  2

/* 所属队伍宏 */
/* 红方 */
#define REFEREE_ROBOT_GROUP_RED   1
/* 蓝方 */
#define REFEREE_ROBOT_GROUP_BLUE  2

/* 停机坪状态 */
/* 无机器人占领 */
#define REFEREE_PARKING_NO                  0
/* 空中机器人已占领但未停桨 */
#define REFEREE_PARKING_OCCUPY_AND_RUNNING  1
/* 空中机器人已占领并停桨 */
#define REFEREE_PARKING_OCCUPY_AND_STOP     2

/* 飞镖发射口的状态 */
/* 关闭 */
#define REFEREE_DART_OPENING_STATUS_CLOSE    0
/* 正在开启或者关闭 */
#define REFEREE_DART_OPENING_STATUS_RUNNING  1
/* 已经开启 */
#define REFEREE_DART_OPENING_STATUS_OPEN     2

/* 飞镖打击目标 */
/* 前哨站 */
#define REFEREE_DART_TARGET_OUTPOST  1
/* 基地 */
#define REFEREE_DART_TARGET_BASE     2

/* 伤害类型 */
/* 装甲收到伤害 */
#define REFEREE_HURT_TYPE_ARMOR_ATTACK        0
/* 模块离线 */
#define REFEREE_HURT_TYPE_OFFLINE             1
/* 超枪口射速 */
#define REFEREE_HURT_TYPE_OVER_SHOOTER_SPEED  2
/* 超枪口热量 */
#define REFEREE_HURT_TYPE_OVER_SHOOTER_HEAT   3
/* 超底盘功率 */
#define REFEREE_HURT_TYPE_OVER_CHASSIS_POWER  4
/* 装甲撞击 */
#define REFEREE_HURT_TYPE_ARMOR_CRASH         5

/* 子弹类型 */
/* 17mm弹丸 */
#define REFEREE_BULLET_TYPE_17  1
/* 42mm弹丸 */
#define REFEREE_BULLET_TYPE_42  2

/* 补给站出弹口状态 */
/* 关闭 */
#define REFEREE_SUPPLY_STEP_CLOSE     0
/* 子弹准备中 */
#define REFEREE_SUPPLY_STEP_PREPARING 1
/* 子弹下落 */
#define REFEREE_SUPPLY_STEP_OPEN      2


typedef struct {
	uint8_t  gameType;                  /* 比赛类型,参考REFEREE_GAME_TYPE宏 */
	uint8_t  gameStage;                 /* 比赛阶段,参考REFEREE_GAME_STAGE宏 */
	uint16_t gameRemainTime;            /* 比赛剩余时间,单位秒 */
	uint16_t our1HP; 					/* 我方1英雄机器人血量，未上场以及罚下血量为0 */
	uint16_t our2HP; 					/* 我方2工程机器人血量，未上场以及罚下血量为0 */
	uint16_t our3HP; 					/* 我方3步兵机器人血量，未上场以及罚下血量为0 */
	uint16_t our4HP; 					/* 我方4步兵机器人血量，未上场以及罚下血量为0 */
	uint16_t our5HP; 					/* 我方5步兵机器人血量，未上场以及罚下血量为0 */
	uint16_t our7HP; 					/* 我方7哨兵机器人血量，未上场以及罚下血量为0 */
	uint16_t ourOutpostHP; 				/* 我方前哨站机器人血量，未上场以及罚下血量为0 */
	uint16_t ourBaseHP; 				/* 我方基地机器人血量，未上场以及罚下血量为0 */
	uint16_t enemy1HP; 					/* 对方1英雄机器人血量，未上场以及罚下血量为0 */
	uint16_t enemy2HP; 					/* 对方2工程机器人血量，未上场以及罚下血量为0 */
	uint16_t enemy3HP; 					/* 对方3步兵机器人血量，未上场以及罚下血量为0 */
	uint16_t enemy4HP; 					/* 对方4步兵机器人血量，未上场以及罚下血量为0 */
	uint16_t enemy5HP; 					/* 对方5步兵机器人血量，未上场以及罚下血量为0 */
	uint16_t enemy7HP;                  /* 对方7哨兵机器人血量，未上场以及罚下血量为0 */
	uint16_t enemyOutpostHP; 			/* 对方前哨站机器人血量，未上场以及罚下血量为0 */
	uint16_t enemyBaseHP; 				/* 对方基地机器人血量，未上场以及罚下血量为0 */
	uint8_t  f1Status;                  /* ICRA中F1红方回血区状态 */
	uint8_t  f1BuffStatus;              /* ICRA中F1红方回血区Buff状态 */
	uint8_t  f2Status;                  /* ICRA中F2红方弹药补给区状态 */
	uint8_t  f2BuffStatus;              /* ICRA中F2红方弹药补给区Buff状态 */
	uint8_t  f3Status;                  /* ICRA中F3蓝方回血区状态 */
	uint8_t  f3BuffStatus;              /* ICRA中F3蓝方回血区Buff状态 */
	uint8_t  f4Status;                  /* ICRA中F4蓝方弹药补给区状态 */
	uint8_t  f4BuffStatus;              /* ICRA中F4蓝方弹药补给区Buff状态 */
	uint8_t  f5Status;                  /* ICRA中F5禁止射击区状态 */
	uint8_t  f5BuffStatus;              /* ICRA中F5禁止射击区Buff状态 */
	uint8_t  f6Status;                  /* ICRA中F6禁止移动区状态 */
	uint8_t  f6BuffStatus;              /* ICRA中F6禁止移动区Buff状态 */
	uint8_t  parkingState;              /* 停机坪状态,参考REFEREE_PARKING宏 */
	uint8_t  energySmallState;          /* 小能量机关状态,0没有,1有 */
	uint8_t  energyBigState;            /* 大能量机关状态,0没有,1有 */
	uint8_t  shieldState;               /* 基地虚拟护盾状态,0没有,1有 */
	uint8_t  dartRemainTime;            /* 飞镖发射口倒计时,单位秒 */
	uint8_t  robotID;                   /* 机器人ID */
	uint8_t  robotGroup;                /* 机器人所属方,参考REFEREE_ROBOT_GROUP宏 */
	uint8_t  robotLevel;                /* 机器人等级,可为1,2,3级 */
	uint16_t remainHP;                  /* 剩余HP */
	uint16_t maxHP;                     /* 最大HP */
	uint16_t shooter17CoolingHeatRate;  /* 17mm枪口每秒冷却值 */
	uint16_t shooter17MaxHeat;          /* 17mm枪口热量上限 */
	uint16_t shooter17MaxSpeed;         /* 17mm枪口上限速度,单位m/s */
	uint16_t shooter17Heat;             /* 17mm枪口热量 */
	uint16_t shooter42CoolingHeatRate;  /* 42mm枪口每秒冷却值 */
	uint16_t shooter42MaxHeat;          /* 42mm枪口热量上限 */
	uint16_t shooter42MaxSpeed;         /* 42mm枪口上限速度,单位m/s */
	uint16_t shooter42Heat;             /* 42mm枪口热量 */
	uint16_t shooterMobile17Heat;       /* 机动17mm枪口热量 */
	uint8_t  isChassisEnable;           /* 底盘电源输出是否启用 */
	uint8_t  isGimbalEnable;            /* 云台电源输出是否启用 */
	uint8_t  isShooterEnable;           /* 射击机构电源输出是否启用 */
	uint8_t  chassisMaxPower;           /* 最大底盘功率 */
	float    chassisOutputVoltage;      /* 底盘输出电压, 单位为V */
	float    chassisOutputCurrent;      /* 底盘输出电流, 单位为A */
	float    chassisOutputPower;        /* 底盘输出功率,单位为W */
	uint16_t chassisPowerBuffer;        /* 底盘功率缓冲,单位为J */
	float    x;                         /* 机器人坐标,x轴 */
	float    y;                         /* 机器人坐标,y轴 */
	float    z;                         /* 机器人坐标,z轴 */
	float    shooterYaw;                /* 枪口朝向,单位度 */
	uint8_t  isGotComplementHPBuff;     /* 是否正在补血 */
	uint8_t  isGotCoolingHeatFasterBuff;/* 是否枪口热量冷却加速 */
	uint8_t  isGotDefenseBuff;          /* 是否获得防御Buff */
	uint8_t  isGotAttackBuff;           /* 是否获得攻击Buff */
	uint16_t droneEnergyPoint;          /* 空中机器人积累的能零点 */
	uint8_t  droneRemainAttackTime;     /* 空中机器人剩余攻击时间,单位s */
	uint16_t remainingBullet;           /* 子弹剩余发射数量 */
	uint8_t  rfidBase;                  /* 基地增益点RFID状态 */
	uint8_t  rfidHightland;             /* 高地增益点RFID状态 */
	uint8_t  rfidEnergy;                /* 能量机关激活点RFID状态 */
	uint8_t  rfidSlope;                 /* 飞坡增益点RFID状态 */
	uint8_t  rfidOutpost;               /* 前哨站RFID状态 */
	uint8_t  rfidResource;              /* 资源岛增益点RFID状态 */
	uint8_t  rfidHP;                    /* 补血点增益点RFID状态 */
	uint8_t  rfidEngineering;           /* 工程机器人补血卡RFID状态 */
	uint8_t  rfidF1;                    /* ICRA的F1区RFID状态 */
	uint8_t  rfidF2;                    /* ICRA的F2区RFID状态 */
	uint8_t  rfidF3;                    /* ICRA的F3区RFID状态 */
	uint8_t  rfidF4;                    /* ICRA的F4区RFID状态 */
	uint8_t  rfidF5;                    /* ICRA的F5区RFID状态 */
	uint8_t  rfidF6;                    /* ICRA的F6区RFID状态 */
	uint8_t  dartOpeningStatus;         /* 飞镖发射口的状态,参考REFEREE_DART_OPENING_STATUS宏 */
	uint8_t  dartTarget;                /* 飞镖打击目标,参考REFEREE_DART_TARGET宏 */
	uint16_t dartChangeTargetTime;      /* 切换打击目标时的比赛剩余时间，单位秒 */
	uint8_t  dartFirstSpeed;            /* 第一枚飞镖速度,单位0.1/m/s/LSB */
	uint8_t  dartSecondSpeed;           /* 第二枚飞镖速度,单位0.1/m/s/LSB */
	uint8_t  dartThirdSpeed;            /* 第三枚飞镖速度,单位0.1/m/s/LSB */
	uint8_t  dartFourthSpeed;           /* 第四枚飞镖速度,单位0.1/m/s/LSB */
	uint16_t dartLastLaunchRemainTime;  /* 最近一次的发射飞镖的比赛剩余时间,单位秒 */
	uint16_t dartLastOperateLaunchTime; /* 最近一次操作手确定发射指令时的比赛剩余时间,单位秒 */
} Referee_Info;

/**
 * 收到数据回调
 * @param cmdID 指令ID
 * @param data 数据指针
 * @param dataLength 数据长度
 * @note 这个回调的调用优先级是最低的
 */
typedef void (*Referee_ReceivedMessageCallback)(uint16_t cmdID, uint8_t* data, uint16_t dataLength);

/**
 * 收到比赛结果数据
 * @param who 谁胜利了,参考REFEREE_GAME_RESULT宏
 * @param data 数据指针
 * @param dataLength 数据长度
 */
typedef void (*Referee_ReceivedGameResultCallback)(uint8_t who);

/**
 * 收到飞镖发射状态数据
 * @param belong 发射飞镖的队伍,参考REFEREE_ROBOT_GROUP宏
 * @param remainTime 发射时的剩余比赛时间
 */
typedef void (*Referee_ReceivedDartStatusCallback)(uint8_t belong, uint16_t remainTime);

/**
 * 收到场地补给站动作标识数据
 * @param robotGroup   补弹机器人队伍,参考REFEREE_ROBOT_GROUP宏
 * @param robotID      补弹机器人ID
 * @param supplyID     补给站口ID
 * @param supplyStatus 补给站出弹口状态,参考REFEREE_SUPPLY_STEP
 * @param bulletNum    补给站补弹数量
 */
typedef void (*Referee_ReceivedSupplyActionCallback)(uint8_t robotGroup, uint8_t robotID, uint8_t supplyID, uint8_t supplyStatus, uint8_t bulletNum);

/**
 * 收到裁判警告数据
 * @param level 警告等级,可为1-5级
 * @param id 犯规机器人id,1级和5级警告该参数无效
 */
typedef void (*Referee_ReceivedWarningCallback)(uint8_t level, uint8_t id);

/**
 * 收到伤害状态数据
 * @param type 伤害类型,参考REFEREE_HURT_TYPE宏
 * @param armorID 如果时装甲伤害扣血,则为装甲板id,其他扣血时该参数无效
 */
typedef void (*Referee_ReceivedHurtCallback)(uint8_t type, uint8_t armorID);

/**
 * 收到实时射击数据
 * @param type 子弹类型,参考REFEREE_BULLET_TYPE宏
 * @param frequent 子弹射频,单位Hz
 * @param speed 子弹射速,单位m/s
 */
typedef void (*Referee_ReceivedShootCallback)(uint8_t type, uint8_t frequent, float speed);

/**
 * 收到机器人间交互数据
 * @param cmdID 指令ID
 * @param senderID 发送者ID
 * @param data 数据指针
 * @param dataLength 数据长度
 */
typedef void (*Referee_ReceivedInteractiveCallback)(uint16_t cmdID, uint16_t senderID, uint8_t* data, uint16_t dataLength);

/**
 * 回调结构体
 */
typedef struct {
	uint8_t type;     /* 回调类型,参考REFEREE_CALLBACK_TYPE宏 */
	void*   callback; /* 回调函数指针 */
} Referee_Callback;

/* 回调类型 */
/* 收到数据回调 */
#define REFEREE_CALLBACK_TYPE_ALL           0
/* 比赛结果数据回调 */
#define REFEREE_CALLBACK_TYPE_GAME_RESULT   1
/* 飞镖发射状态回调 */
#define REFEREE_CALLBACK_TYPE_DART_STATUS   2
/* 场地补给站动作标识回调 */
#define REFEREE_CALLBACK_TYPE_SUPPLY_ACTION 3
/* 裁判警告回调 */
#define REFEREE_CALLBACK_TYPE_WARNING       4
/* 伤害状态回调 */
#define REFEREE_CALLBACK_TYPE_HURT          5
/* 实时射击回调 */
#define REFEREE_CALLBACK_TYPE_SHOOT         6
/* 机器人间交互回调 */
#define REFEREE_CALLBACK_TYPE_INTERACTIVE   7

/* 裁判系统SOF */
#define REFEREE_HEADER_SOF 0xA5

/* 裁判系统数据解析状态机参数 */
#define REFEREE_STATE_HEADER_SOF         0
#define REFEREE_STATE_HEADER_LENGTH_HIGH 1
#define REFEREE_STATE_HEADER_LENGTH_LOW  2
#define REFEREE_STATE_HEADER_CRC         3
#define REFEREE_STATE_CMD_ID_HIGH        4
#define REFEREE_STATE_CMD_ID_LOW         5
#define REFEREE_STATE_DATA               6
#define REFEREE_STATE_TAIL_CRC_HIGH      7
#define REFEREE_STATE_TAIL_CRC_LOW       8

void Referee_Init();
void Referee_RegisterReceivedMessagesCallback(Referee_ReceivedMessageCallback callback);
void Referee_RegisterReceivedGameResultCallback(Referee_ReceivedGameResultCallback callback);
void Referee_RegisterReceivedDartStatusCallback(Referee_ReceivedDartStatusCallback callback);
void Referee_RegisterReceivedSupplyActionCallback(Referee_ReceivedSupplyActionCallback callback);
void Referee_RegisterReceivedWarningCallback(Referee_ReceivedWarningCallback callback);
void Referee_RegisterReceivedHurtCallback(Referee_ReceivedHurtCallback callback);
void Referee_RegisterReceivedShootCallback(Referee_ReceivedShootCallback callback);
void Referee_RegisterReceivedInteractiveCallback(Referee_ReceivedInteractiveCallback);

#endif
#endif /* INC_REFEREE_H_ */
