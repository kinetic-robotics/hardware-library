/*
 * driver.c
 *
 *  Created on: Oct 20, 2020
 *      Author: xqe2011
 */

#include "Library/Inc/drivers/driver.h"
#include "Library/Inc/drivers/can.h"
#include "Library/Inc/drivers/uart.h"
#include "Library/Inc/drivers/spi.h"
#include "Library/Inc/drivers/gpio.h"
#include "Library/Inc/drivers/pwm.h"
#include "Library/Inc/drivers/flash.h"
#include "Library/Inc/drivers/usb.h"
#include "Configurations/library_config.h"

/**
 * 驱动层初始化
 */
void Driver_Init()
{
	#ifdef CONFIG_DRIVER_CAN_ENABLE
		CAN_Init();
	#endif

	#ifdef CONFIG_DRIVER_UART_ENABLE
		UART_Init();
	#endif

	#ifdef CONFIG_DRIVER_UART_ENABLE
		SPI_Init();
	#endif

	#ifdef CONFIG_DRIVER_GPIO_ENABLE
		GPIO_Init();
	#endif

	#ifdef CONFIG_DRIVER_PWM_ENABLE
		PWM_Init();
	#endif

	#ifdef CONFIG_DRIVER_FLASH_ENABLE
		Flash_Init();
	#endif

	#ifdef CONFIG_DRIVER_FLASH_ENABLE
		USB_Init();
	#endif
}
