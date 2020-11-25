/*
 * rc.h
 *
 *  Created on: Oct 21, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_RC_H_
#define LIBRARY_RC_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_RC_ENABLE

#include <stdint.h>

/* 遥控器一帧的大小 */
#define RC_DBUS_FRAME_SIZE 18

/* 遥控器状态正常 */
#define RC_OK 1

/* 遥控器超时 */
#define RC_TIMEOUT 150

/* 遥控器最大行程 */
#define RC_MAX_VALUE 660.0f

/* 遥控器波轮最大行程 */
#define RC_WHEEL_MAX_VALUE 1024.0f

typedef struct {
  /* 遥控器的通道数据，数值范围：-660 ~ 660 */
  int16_t ch1;   /* 右侧左右 */
  int16_t ch2;   /* 右侧上下 */
  int16_t ch3;   /* 左侧左右 */
  int16_t ch4;   /* 左侧上下 */

  /* 遥控器的拨杆数据，上中下分别为：1、3、2 */
  uint8_t sw1;   /* 左侧拨杆 */
  uint8_t sw2;   /* 右侧拨杆 */

  /* PC 鼠标数据 */
  struct
  {
    /* 鼠标移动相关 */
    int16_t x;   /* 鼠标平移 */
    int16_t y;   /* 鼠标上下 */
    /* 鼠标按键相关，1为按下，0为松开 */
    uint8_t l;   /* 左侧按键 */
    uint8_t r;   /* 右侧按键 */
  }mouse;

  /* PC 键盘按键数据 */
  union
  {
    uint16_t keyCode;
    struct
    {
      uint16_t W:1;
      uint16_t S:1;
      uint16_t A:1;
      uint16_t D:1;
      uint16_t SHIFT:1;
      uint16_t CTRL:1;
      uint16_t Q:1;
      uint16_t E:1;
      uint16_t R:1;
      uint16_t F:1;
      uint16_t G:1;
      uint16_t Z:1;
      uint16_t X:1;
      uint16_t C:1;
      uint16_t V:1;
      uint16_t B:1;
    }bit;
  }kb;

  /* 遥控器左侧拨轮数据 */
  int16_t wheel;

  /* 上次接收到遥控器数据的时间 */
  uint32_t lastReceiveTime;

  /* 遥控器状态 */
  uint8_t state;
} RC_Info;

const RC_Info* RC_GetData();
void RC_Init();

#endif
#endif /* LIBRARY_RC_H_ */
