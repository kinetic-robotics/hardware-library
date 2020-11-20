/*
 * tools.h
 *
 *  Created on: Oct 20, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_TOOL_H_
#define LIBRARY_TOOL_H_

/* 获取位的值 */
#define TOOL_GET_BIT(data, bit) ( ( (data) >> (bit) ) & 0x01)

/* 获取数组长度 */
#define TOOL_GET_ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

/* 限幅函数 */
#define TOOL_LIMIT(a, min, max) if ((a) > (max))  (a) = (max);\
							    if ((a) < (min))  (a) = (min);

/* 绝对值限制函数 */
#define TOOL_ABS_LIMIT(a, max) TOOL_LIMIT(a, -max, max)

/* 角度转弧度系数 */
#define TOOL_RADIAN_COEF      57.3f

#endif /* LIBRARY_TOOL_H_ */
