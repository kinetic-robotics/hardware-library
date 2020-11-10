/*
 * imu.h
 *
 *  Created on: Oct 21, 2020
 *      Author: xqe2011
 */

#ifndef LIBRARY_IMU_H_
#define LIBRARY_IMU_H_

#include "Configurations/library_config.h"
#ifdef CONFIG_IMU_ENABLE

/**
 * MPU的数据
 */
typedef struct {
	int16_t ax;
	int16_t ay;
	int16_t az;

	int16_t mx;
	int16_t my;
	int16_t mz;

	int16_t temp;

	int16_t gx;
	int16_t gy;
	int16_t gz;

	int16_t ax_offset;
	int16_t ay_offset;
	int16_t az_offset;

	int16_t gx_offset;
	int16_t gy_offset;
	int16_t gz_offset;
} IMU_MPUData;

/**
 * 解算的IMU信息
 */
typedef struct {
	int16_t ax;
	int16_t ay;
	int16_t az;

	int16_t mx;
	int16_t my;
	int16_t mz;

	float temp;

	float wx; /*!< omiga, +- 2000dps => +-32768  so gx/16.384/57.3 =	rad/s */
	float wy;
	float wz;

	float vx;
	float vy;
	float vz;

	float rol;
	float pit;
	float yaw;
} IMU_Info;

/* MPU6500的WHO_AM_I寄存器应该的值 */
#define IMU_MPU6500_WHO_AM_I_VALUE 0x70

/* 定义MPU6500寄存器和地址 */
#define IMU_MPU6500_SELF_TEST_XG        0x00
#define IMU_MPU6500_SELF_TEST_YG        0x01
#define IMU_MPU6500_SELF_TEST_ZG        0x02
#define IMU_MPU6500_SELF_TEST_XA        0x0D
#define IMU_MPU6500_SELF_TEST_YA        0x0E
#define IMU_MPU6500_SELF_TEST_ZA        0x0F
#define IMU_MPU6500_XG_OFFSET_H         0x13
#define IMU_MPU6500_XG_OFFSET_L         0x14
#define IMU_MPU6500_YG_OFFSET_H         0x15
#define IMU_MPU6500_YG_OFFSET_L         0x16
#define IMU_MPU6500_ZG_OFFSET_H         0x17
#define IMU_MPU6500_ZG_OFFSET_L         0x18
#define IMU_MPU6500_SMPLRT_DIV          0x19
#define IMU_MPU6500_CONFIG              0x1A
#define IMU_MPU6500_GYRO_CONFIG         0x1B
#define IMU_MPU6500_ACCEL_CONFIG        0x1C
#define IMU_MPU6500_ACCEL_CONFIG_2      0x1D
#define IMU_MPU6500_LP_ACCEL_ODR        0x1E
#define IMU_MPU6500_MOT_THR             0x1F
#define IMU_MPU6500_FIFO_EN             0x23
#define IMU_MPU6500_I2C_MST_CTRL        0x24
#define IMU_MPU6500_I2C_SLV0_ADDR       0x25
#define IMU_MPU6500_I2C_SLV0_REG        0x26
#define IMU_MPU6500_I2C_SLV0_CTRL       0x27
#define IMU_MPU6500_I2C_SLV1_ADDR       0x28
#define IMU_MPU6500_I2C_SLV1_REG        0x29
#define IMU_MPU6500_I2C_SLV1_CTRL       0x2A
#define IMU_MPU6500_I2C_SLV2_ADDR       0x2B
#define IMU_MPU6500_I2C_SLV2_REG        0x2C
#define IMU_MPU6500_I2C_SLV2_CTRL       0x2D
#define IMU_MPU6500_I2C_SLV3_ADDR       0x2E
#define IMU_MPU6500_I2C_SLV3_REG        0x2F
#define IMU_MPU6500_I2C_SLV3_CTRL       0x30
#define IMU_MPU6500_I2C_SLV4_ADDR       0x31
#define IMU_MPU6500_I2C_SLV4_REG        0x32
#define IMU_MPU6500_I2C_SLV4_DO         0x33
#define IMU_MPU6500_I2C_SLV4_CTRL       0x34
#define IMU_MPU6500_I2C_SLV4_DI         0x35
#define IMU_MPU6500_I2C_MST_STATUS      0x36
#define IMU_MPU6500_INT_PIN_CFG         0x37
#define IMU_MPU6500_INT_ENABLE          0x38
#define IMU_MPU6500_INT_STATUS          0x3A
#define IMU_MPU6500_ACCEL_XOUT_H        0x3B
#define IMU_MPU6500_ACCEL_XOUT_L        0x3C
#define IMU_MPU6500_ACCEL_YOUT_H        0x3D
#define IMU_MPU6500_ACCEL_YOUT_L        0x3E
#define IMU_MPU6500_ACCEL_ZOUT_H        0x3F
#define IMU_MPU6500_ACCEL_ZOUT_L        0x40
#define IMU_MPU6500_TEMP_OUT_H          0x41
#define IMU_MPU6500_TEMP_OUT_L          0x42
#define IMU_MPU6500_GYRO_XOUT_H         0x43
#define IMU_MPU6500_GYRO_XOUT_L         0x44
#define IMU_MPU6500_GYRO_YOUT_H         0x45
#define IMU_MPU6500_GYRO_YOUT_L         0x46
#define IMU_MPU6500_GYRO_ZOUT_H         0x47
#define IMU_MPU6500_GYRO_ZOUT_L         0x48
#define IMU_MPU6500_EXT_SENS_DATA_00    0x49
#define IMU_MPU6500_EXT_SENS_DATA_01    0x4A
#define IMU_MPU6500_EXT_SENS_DATA_02    0x4B
#define IMU_MPU6500_EXT_SENS_DATA_03    0x4C
#define IMU_MPU6500_EXT_SENS_DATA_04    0x4D
#define IMU_MPU6500_EXT_SENS_DATA_05    0x4E
#define IMU_MPU6500_EXT_SENS_DATA_06    0x4F
#define IMU_MPU6500_EXT_SENS_DATA_07    0x50
#define IMU_MPU6500_EXT_SENS_DATA_08    0x51
#define IMU_MPU6500_EXT_SENS_DATA_09    0x52
#define IMU_MPU6500_EXT_SENS_DATA_10    0x53
#define IMU_MPU6500_EXT_SENS_DATA_11    0x54
#define IMU_MPU6500_EXT_SENS_DATA_12    0x55
#define IMU_MPU6500_EXT_SENS_DATA_13    0x56
#define IMU_MPU6500_EXT_SENS_DATA_14    0x57
#define IMU_MPU6500_EXT_SENS_DATA_15    0x58
#define IMU_MPU6500_EXT_SENS_DATA_16    0x59
#define IMU_MPU6500_EXT_SENS_DATA_17    0x5A
#define IMU_MPU6500_EXT_SENS_DATA_18    0x5B
#define IMU_MPU6500_EXT_SENS_DATA_19    0x5C
#define IMU_MPU6500_EXT_SENS_DATA_20    0x5D
#define IMU_MPU6500_EXT_SENS_DATA_21    0x5E
#define IMU_MPU6500_EXT_SENS_DATA_22    0x5F
#define IMU_MPU6500_EXT_SENS_DATA_23    0x60
#define IMU_MPU6500_I2C_SLV0_DO         0x63
#define IMU_MPU6500_I2C_SLV1_DO         0x64
#define IMU_MPU6500_I2C_SLV2_DO         0x65
#define IMU_MPU6500_I2C_SLV3_DO         0x66
#define IMU_MPU6500_I2C_MST_DELAY_CTRL  0x67
#define IMU_MPU6500_SIGNAL_PATH_RESET   0x68
#define IMU_MPU6500_MOT_DETECT_CTRL     0x69
#define IMU_MPU6500_USER_CTRL           0x6A
#define IMU_MPU6500_PWR_MGMT_1          0x6B
#define IMU_MPU6500_PWR_MGMT_2          0x6C
#define IMU_MPU6500_FIFO_COUNTH         0x72
#define IMU_MPU6500_FIFO_COUNTL         0x73
#define IMU_MPU6500_FIFO_R_W            0x74
#define IMU_MPU6500_WHO_AM_I            0x75
#define IMU_MPU6500_XA_OFFSET_H         0x77
#define IMU_MPU6500_XA_OFFSET_L         0x78
#define IMU_MPU6500_YA_OFFSET_H         0x7A
#define IMU_MPU6500_YA_OFFSET_L         0x7B
#define IMU_MPU6500_ZA_OFFSET_H         0x7D
#define IMU_MPU6500_ZA_OFFSET_L         0x7E
#define IMU_MPU6500_ID					0x70
#define IMU_MPU6500_IIC_ADDR			0x68

/* 定义IST8310寄存器和地址 */
#define IMU_IST8310_ADDRESS             0x0E
#define IMU_IST8310_DEVICE_ID_A         0x10
#define IMU_IST8310_WHO_AM_I            0x00
#define IMU_IST8310_R_CONFA             0x0A
#define IMU_IST8310_R_CONFB             0x0B
#define IMU_IST8310_R_MODE              0x02
#define IMU_IST8310_R_XL                0x03
#define IMU_IST8310_R_XM                0x04
#define IMU_IST8310_R_YL                0x05
#define IMU_IST8310_R_YM                0x06
#define IMU_IST8310_R_ZL                0x07
#define IMU_IST8310_R_ZM                0x08
#define IMU_IST8310_AVGCNTL             0x41
#define IMU_IST8310_PDCNTL              0x42
#define IMU_IST8310_ODR_MODE            0x01

const IMU_Info* IMU_GetData();
void IMU_Init();

#endif
#endif /* LIBRARY_IMU_H_ */
