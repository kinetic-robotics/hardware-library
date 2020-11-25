/*
 * library.c
 *
 *  Created on: Oct 19, 2020
 *      Author: xqe2011
 */

#include <Library/Inc/dct/communicate.h>
#include "Library/Inc/library.h"
#include "Library/Inc/motor.h"
#include "Library/Inc/imu.h"
#include "Library/Inc/rc.h"
#include "Library/Inc/storage.h"
#include "Library/Inc/capacity.h"
#include "Library/Inc/referee.h"
#include "Library/Inc/led.h"
#include "Library/Inc/drivers/driver.h"

/**
 * 整个库,如果出现了不可挽回的错误,都会跳转到这
 */
void Library_Error()
{
	while(1);
}

void Library_Init()
{
	Driver_Init();
	#ifdef CONFIG_LED_ENABLE
		LED_Init();
	#endif

	#ifdef CONFIG_STORAGE_ENABLE
		Storage_Init();
	#endif
	#ifdef CONFIG_RC_ENABLE
		RC_Init();
	#endif
	#ifdef CONFIG_IMU_ENABLE
		IMU_Init();
	#endif
	#ifdef CONFIG_MOTOR_ENABLE
		Motor_Init();
	#endif
	#ifdef CONFIG_CAPACITY_ENABLE
		Capacity_Init();
	#endif
	#ifdef CONFIG_REFEREE_ENABLE
		Referee_Init();
	#endif
	#ifdef CONFIG_DCT_ENABLE
		DCT_Communicate_Init();
	#endif
}
