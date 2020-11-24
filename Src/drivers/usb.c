/*
 * usb.c
 *
 *  Created on: Nov 24, 2020
 *      Author: xqe2011
 */
#include "Configurations/library_config.h"
#ifdef CONFIG_DRIVER_USB_ENABLE

#include "Library/Inc/library.h"
#include "Library/Inc/drivers/usb.h"
#include "Library/Inc/tool.h"
#include "USB_DEVICE/App/usbd_cdc_if.h"

/* 回调列表 */
static USB_RxCallback callbacks[8];
static uint8_t callbacksLength = 0;
extern USBD_HandleTypeDef hUsbDeviceFS;

/**
 * USB是否已经连接
 * @return
 */
uint8_t USB_IsConnected()
{
	return hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED;
}

/**
 * 发送USB数据,如果USB未连接,则不会发送
 * @param data
 * @param dataLength
 */
void USB_Send(uint8_t* data, uint16_t dataLength)
{
	if (!USB_IsConnected()) return;
	/* 针对长度为64的整数倍时的处理,分两包发 */
	if (dataLength % 64 == 0) {
		CDC_Transmit_FS(data, dataLength - 1);
		CDC_Transmit_FS(data + dataLength - 1, 1);
	} else {
		CDC_Transmit_FS(data, dataLength);
	}
}

/**
 * 注册USB接收回调
 * @param callback 回调
 */
void USB_RegisterCallback(USB_RxCallback callback)
{
	if (callbacksLength > TOOL_GET_ARRAY_LENGTH(callbacks) - 1) {
		Library_Error();
		return;
	}
	callbacks[callbacksLength] = callback;
	callbacksLength++;
}

/**
 * USB接收中断
 */
void USB_ReceiveIT(uint8_t* data, uint32_t dataLength)
{
	for(size_t i = 0;i < callbacksLength;i++) {
		callbacks[i](data, dataLength);
	}
}


/**
 * USB模块初始化
 */
void USB_Init()
{

}

#endif
