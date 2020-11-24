/*
 * usb.h
 *
 *  Created on: Nov 24, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_DRIVERS_USB_H_
#define LIBRARY_DRIVERS_USB_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_USB_ENABLE

/**
 * 用户回调函数
 * @param id
 * @param data 数据指针
 * @param dataLength 数据长度
 */
typedef void (*USB_RxCallback)(uint8_t* data, uint32_t dataLength);

void USB_Send(uint8_t* data, uint16_t dataLength);
void USB_RegisterCallback(USB_RxCallback callback);
void USB_ReceiveIT(uint8_t* data, uint32_t dataLength);
void USB_Init();

#endif
#endif /* INC_DRIVERS_USB_H_ */
