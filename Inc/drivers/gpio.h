/*
 * gpio.h
 *
 *  Created on: Oct 21, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_DRIVERS_GPIO_H_
#define LIBRARY_DRIVERS_GPIO_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_GPIO_ENABLE

#include "cmsis_os.h"
#include "semphr.h"

typedef struct {
	uint16_t pin;         						   /* GPIO Pin */
	GPIO_TypeDef* port;                            /* GPIO Port */
} GPIO_Info;

void GPIO_Set(uint8_t id, uint8_t value);
uint8_t GPIO_Get(uint8_t id);
void GPIO_Init();

#endif
#endif /* LIBRARY_DRIVERS_GPIO_H_ */
