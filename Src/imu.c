/************************** (C) COPYRIGHT 2018 DJI *************************
 * @file       bsp_imu.c
 *     mpu6500 module driver, configurate MPU6500 and Read the Accelerator
 *             and Gyrometer data using SPI interface
 * @note
 * @Version    V1.0.0
 * @Date       Jan-30-2018
 ************************** (C) COPYRIGHT 2018 DJI *************************/
#include "Configurations/library_config.h"
#ifdef CONFIG_IMU_ENABLE

#include <stdint.h>
#include "cmsis_os.h"
#include "Library/Inc/algorithm/pid.h"
#include "Library/Inc/imu.h"
#include "Library/Inc/tool.h"
#include "Configurations/library_config.h"
#include "Library/Inc/drivers/spi.h"
#include "Library/Inc/drivers/gpio.h"
#include "Library/Inc/drivers/pwm.h"
#include <string.h>
#include <math.h>

static float        q0 = 1.0f;
static float        q1 = 0.0f;
static float        q2 = 0.0f;
static float        q3 = 0.0f;
static float        exInt, eyInt, ezInt;                 /* error integral */
static float        gx, gy, gz, ax, ay, az, mx, my, mz;
static uint32_t     lastUpdate, nowUpdate;               /* Sampling cycle count, ubit ms */
static IMU_MPUData  mpuData;
static IMU_Info     info;
uint32_t a = 0;
static PID_Info     heatPID;

/**
 * 控制MPU6500的NSS线
 * @param value 0表示低电平,1表示高电平
 */
static void IMU_MPUNssSet(uint8_t value)
{
	GPIO_Set(CONFIG_IMU_NSS_PIN, value);
}

/**
 * Write a byte of data to specified register
 * @param  reg:  the address of register to be written
 *         data: data to be written
 * @retval 0
 * @usage  call in IMU_ISTRegWriteByMPU(),
 *                 IMU_ISTRegReadByMPU(),
 *                 IMU_MPUMasteI2CAutoReadConfig(),
 *                 IMU_ISTInit(),
 *                 IMU_MPUSetGyroFSR(),
 *                 IMU_MPUSetAccelFSR(),
 *                 IMU_MPUDeviceInit() function
 */
static uint8_t IMU_MPUWriteByte(uint8_t const reg, uint8_t const data)
{
    IMU_MPUNssSet(0);
    uint8_t tx = reg & 0x7F;
    uint8_t rx = 0;
    SPI_TransmitReceive(CONFIG_IMU_SPI, &tx, &rx, 1, 55);
    tx = data;
    SPI_TransmitReceive(CONFIG_IMU_SPI, &tx, &rx, 1, 55);
    IMU_MPUNssSet(1);
    return 0;
}

/**
 * Read a byte of data from specified register
 * @param  reg: the address of register to be read
 * @retval value
 * @usage  call in IMU_ISTRegReadByMPU(),
 *                 IMU_MPUDeviceInit() function
 */
static uint8_t IMU_MPUReadByte(uint8_t const reg)
{
    IMU_MPUNssSet(0);
    uint8_t tx = reg | 0x80;
    uint8_t rx = 0;
    SPI_TransmitReceive(CONFIG_IMU_SPI, &tx, &rx, 1, 55);
    SPI_TransmitReceive(CONFIG_IMU_SPI, &tx, &rx, 1, 55);
    IMU_MPUNssSet(1);
    return rx;
}

/**
 * Read bytes of data from specified register
 * @param  reg: address from where data is to be written
 * @retval 0
 * @usage  call in IMU_ISTGetData(),
 *                 IMU_MPUGetData(),
 *                 IMU_MPUOffsetCall() function
 */
static uint8_t IMU_MPUReadBytes(uint8_t const regAddr, uint8_t* pData, uint8_t len)
{
    IMU_MPUNssSet(0);
    uint8_t tx = regAddr | 0x80;
    uint8_t rx = 0;
    uint8_t txBuff[14];
    txBuff[0] = tx;
    SPI_TransmitReceive(CONFIG_IMU_SPI, &tx, &rx, 1, 55);
    SPI_TransmitReceive(CONFIG_IMU_SPI, txBuff, pData, len, 55);
    IMU_MPUNssSet(1);
    return 0;
}

/**
 * Fast inverse square-root, to calculate 1/Sqrt(x)
 * @param  x: the number need to be calculated
 * @retval 1/Sqrt(x)
 * @usage  call in IMU_AHRSUpdate() function
 */
static float IMU_InvSqrt(float x)
{
	float halfx = 0.5f * x;
	float y     = x;
	long  i     = *(long*)&y;

	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));

	return y;
}

/**
 * Write IST8310 register through MPU6500's I2C master
 * @param  addr: the address to be written of IST8310's register
 * @param  data: data to be written
 * @usage  call in IMU_ISTInit() function
 */
static void IMU_ISTRegWriteByMPU(uint8_t addr, uint8_t data)
{
    /* turn off slave 1 at first */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_CTRL, 0x00);
    osDelay(2);
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_REG, addr);
    osDelay(2);
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_DO, data);
    osDelay(2);
    /* turn on slave 1 with one byte transmitting */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_CTRL, 0x80 | 0x01);
    /* wait longer to ensure the data is transmitted from slave 1 */
    osDelay(10);
}

/**
 * Write IST8310 register through MPU6500's I2C Master
 * @param  addr: the address to be read of IST8310's register
 * @retval value
 * @usage  call in IMU_ISTInit() function
 */
static uint8_t IMU_ISTRegReadByMPU(uint8_t addr)
{
    uint8_t retval;
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV4_REG, addr);
    osDelay(10);
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV4_CTRL, 0x80);
    osDelay(10);
    retval = IMU_MPUReadByte(IMU_MPU6500_I2C_SLV4_DI);
    /* turn off slave4 after read */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV4_CTRL, 0x00);
    osDelay(10);
    return retval;
}

/**
 * Initialize the MPU6500 I2C Slave 0 for I2C reading.
 * @param  device_address: slave device address, Address[6:0]
 */
static void IMU_MPUMasteI2CAutoReadConfig(uint8_t device_address, uint8_t reg_base_addr, uint8_t data_num)
{
    /* configure the device address of the IST8310 use slave1, auto transmit single measure mode */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_ADDR, device_address);
    osDelay(2);
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_REG, IMU_IST8310_R_CONFA);
    osDelay(2);
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_DO, IMU_IST8310_ODR_MODE);
    osDelay(2);

    /* use slave0,auto read data */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV0_ADDR, 0x80 | device_address);
    osDelay(2);
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV0_REG, reg_base_addr);
    osDelay(2);

    /* every eight mpu6500 internal samples one i2c master read */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV4_CTRL, 0x03);
    osDelay(2);
    /* enable slave 0 and 1 access delay */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_MST_DELAY_CTRL, 0x01 | 0x02);
    osDelay(2);
    /* enable slave 1 auto transmit */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_CTRL, 0x80 | 0x01);
	/* Wait 6ms (minimum waiting time for 16 times internal average setup) */
    osDelay(6);
    /* enable slave 0 with data_num bytes reading */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV0_CTRL, 0x80 | data_num);
    osDelay(2);
}

/**
 * Initializes the IST8310 device
 * @usage  call in IMU_MPUDeviceInit() function
 */
static uint8_t IMU_ISTInit()
{
	/* enable iic master mode */
    IMU_MPUWriteByte(IMU_MPU6500_USER_CTRL, 0x30);
    osDelay(10);
	/* enable iic 400khz */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_MST_CTRL, 0x0d);
    osDelay(10);

    /* turn on slave 1 for ist write and slave 4 to ist read */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_ADDR, IMU_IST8310_ADDRESS);
    osDelay(10);
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV4_ADDR, 0x80 | IMU_IST8310_ADDRESS);
    osDelay(10);

    /* IMU_IST8310_R_CONFB 0x01 = device rst */
    IMU_ISTRegWriteByMPU(IMU_IST8310_R_CONFB, 0x01);
    osDelay(10);
    if (IMU_IST8310_DEVICE_ID_A != IMU_ISTRegReadByMPU(IMU_IST8310_WHO_AM_I))
        return 1;

	/* soft reset */
    IMU_ISTRegWriteByMPU(IMU_IST8310_R_CONFB, 0x01);
    osDelay(10);

	/* config as ready mode to access register */
    IMU_ISTRegWriteByMPU(IMU_IST8310_R_CONFA, 0x00);
    if (IMU_ISTRegReadByMPU(IMU_IST8310_R_CONFA) != 0x00)
        return 2;
    osDelay(10);

	/* normal state, no int */
    IMU_ISTRegWriteByMPU(IMU_IST8310_R_CONFB, 0x00);
    if (IMU_ISTRegReadByMPU(IMU_IST8310_R_CONFB) != 0x00)
        return 3;
    osDelay(10);

    /* config low noise mode, x,y,z axis 16 time 1 avg */
    IMU_ISTRegWriteByMPU(IMU_IST8310_AVGCNTL, 0x24); //100100
    if (IMU_ISTRegReadByMPU(IMU_IST8310_AVGCNTL) != 0x24)
        return 4;
    osDelay(10);

    /* Set/Reset pulse duration setup,normal mode */
    IMU_ISTRegWriteByMPU(IMU_IST8310_PDCNTL, 0xc0);
    if (IMU_ISTRegReadByMPU(IMU_IST8310_PDCNTL) != 0xc0)
        return 5;
    osDelay(10);

    /* turn off slave1 & slave 4 */
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV1_CTRL, 0x00);
    osDelay(10);
    IMU_MPUWriteByte(IMU_MPU6500_I2C_SLV4_CTRL, 0x00);
    osDelay(10);

    /* configure and turn on slave 0 */
    IMU_MPUMasteI2CAutoReadConfig(IMU_IST8310_ADDRESS, IMU_IST8310_R_XL, 0x06);
    osDelay(100);
    return 0;
}

/**
 * Get the data of IST8310
 * @param  buff: the buffer to save the data of IST8310
 * @usage  call in IMU_MPUGetData() function
 */
static void IMU_ISTGetData(uint8_t* buff)
{
    IMU_MPUReadBytes(IMU_MPU6500_EXT_SENS_DATA_00, buff, 6);
}


/**
 * Get the data of IMU
 * @usage  call in main() function
 */
static void IMU_MPUGetData()
{
	uint8_t mpuBuff[14] = {0};
    IMU_MPUReadBytes(IMU_MPU6500_ACCEL_XOUT_H, mpuBuff, 14);

    mpuData.ax   = mpuBuff[0] << 8 | mpuBuff[1];
    mpuData.ay   = mpuBuff[2] << 8 | mpuBuff[3];
    mpuData.az   = mpuBuff[4] << 8 | mpuBuff[5];
    mpuData.temp = mpuBuff[6] << 8 | mpuBuff[7];

    mpuData.gx = ((mpuBuff[8]  << 8 | mpuBuff[9])  - mpuData.gx_offset);
    mpuData.gy = ((mpuBuff[10] << 8 | mpuBuff[11]) - mpuData.gy_offset);
    mpuData.gz = ((mpuBuff[12] << 8 | mpuBuff[13]) - mpuData.gz_offset);

    uint8_t istBuff[6] = {0};
    IMU_ISTGetData(istBuff);
    memcpy(&mpuData.mx, istBuff, 6);

    memcpy(&info.ax, &mpuData.ax, 6 * sizeof(int16_t));

    info.temp = 21 + mpuData.temp / 333.87f;
	/* 2000dps -> rad/s */
	info.wx   = mpuData.gx / 16.384f / 57.3f;
    info.wy   = mpuData.gy / 16.384f / 57.3f;
    info.wz   = mpuData.gz / 16.384f / 57.3f;
}


/**
 * Set MPU6500 gyroscope measure range
 * @param  fsr: range(0,±250dps;1,±500dps;2,±1000dps;3,±2000dps)
 * @retval 0
 * @usage  call in IMU_MPUDeviceInit() function
 */
static uint8_t IMU_MPUSetGyroFSR(uint8_t fsr)
{
  return IMU_MPUWriteByte(IMU_MPU6500_GYRO_CONFIG, fsr << 3);
}


/**
 * Set MPU6500 accelerate measure range
 * @param  fsr: range(0,±2g;1,±4g;2,±8g;3,±16g)
 * @retval 0
 * @usage  call in IMU_MPUDeviceInit() function
 */
static uint8_t IMU_MPUSetAccelFSR(uint8_t fsr)
{
  return IMU_MPUWriteByte(IMU_MPU6500_ACCEL_CONFIG, fsr << 3);
}

/**
 * Get the offset data of MPU6500
 * @usage  call in main() function
 */
static void IMU_MPUOffsetCall()
{
	int i;
	for (i=0; i<300;i++) {
		uint8_t mpuBuff[14] = {0};
		IMU_MPUReadBytes(IMU_MPU6500_ACCEL_XOUT_H, mpuBuff, 14);

		mpuData.ax_offset += mpuBuff[0] << 8 | mpuBuff[1];
		mpuData.ay_offset += mpuBuff[2] << 8 | mpuBuff[3];
		mpuData.az_offset += mpuBuff[4] << 8 | mpuBuff[5];

		mpuData.gx_offset += mpuBuff[8]  << 8 | mpuBuff[9];
		mpuData.gy_offset += mpuBuff[10] << 8 | mpuBuff[11];
		mpuData.gz_offset += mpuBuff[12] << 8 | mpuBuff[13];

		osDelay(5);
	}
	mpuData.ax_offset=mpuData.ax_offset / 300;
	mpuData.ay_offset=mpuData.ay_offset / 300;
	mpuData.az_offset=mpuData.az_offset / 300;
	mpuData.gx_offset=mpuData.gx_offset / 300;
	mpuData.gy_offset=mpuData.gx_offset / 300;
	mpuData.gz_offset=mpuData.gz_offset / 300;
}

/**
 * Initialize MPU6500 and magnet meter ist3810
 * @return 0 if failed return 1
 * @usage  call in main() function
 */
static uint8_t IMU_MPUDeviceInit(void)
{
	osDelay(100);
	if (IMU_MPUReadByte(IMU_MPU6500_WHO_AM_I) != IMU_MPU6500_WHO_AM_I_VALUE) {
		return 1;
	}

	uint8_t i                        = 0;
	uint8_t IMU_MPU6500_Init_Data[10][2] = {
		{ IMU_MPU6500_PWR_MGMT_1, 0x80 },     /* Reset Device */
		{ IMU_MPU6500_PWR_MGMT_1, 0x03 },     /* Clock Source - Gyro-Z */
		{ IMU_MPU6500_PWR_MGMT_2, 0x00 },     /* Enable Acc & Gyro */
		{ IMU_MPU6500_CONFIG, 0x04 },         /* LPF 41Hz */
		{ IMU_MPU6500_GYRO_CONFIG, 0x18 },    /* +-2000dps */
		{ IMU_MPU6500_ACCEL_CONFIG, 0x10 },   /* +-8G */
		{ IMU_MPU6500_ACCEL_CONFIG_2, 0x02 }, /* enable LowPassFilter  Set Acc LPF */
		{ IMU_MPU6500_USER_CTRL, 0x20 }       /* Enable AUX */
	};
	for (i = 0; i < 10; i++) {
		IMU_MPUWriteByte(IMU_MPU6500_Init_Data[i][0], IMU_MPU6500_Init_Data[i][1]);
		osDelay(1);
	}

	IMU_MPUSetGyroFSR(3);
	IMU_MPUSetAccelFSR(2);

	IMU_ISTInit();
	IMU_MPUOffsetCall();
	return 0;
}



/**
 * Initialize quaternion
 * @usage  call in main() function
 */
static void IMU_InitQuaternion()
{
	int16_t hx, hy;//hz;

	hx = info.mx;
	hy = info.my;
	//hz = imu.mz;

	if (hx < 0 && hy < 0) {
		if (fabs(hx / hy) >= 1) {
			q0 = -0.005;
			q1 = -0.199;
			q2 = 0.979;
			q3 = -0.0089;
		} else {
			q0 = -0.008;
			q1 = -0.555;
			q2 = 0.83;
			q3 = -0.002;
		}
	} else if (hx < 0 && hy > 0) {
		if (fabs(hx / hy)>=1) {
			q0 = 0.005;
			q1 = -0.199;
			q2 = -0.978;
			q3 = 0.012;
		} else {
			q0 = 0.005;
			q1 = -0.553;
			q2 = -0.83;
			q3 = -0.0023;
		}

	} else if (hx > 0 && hy > 0) {
		if (fabs(hx / hy) >= 1) {
			q0 = 0.0012;
			q1 = -0.978;
			q2 = -0.199;
			q3 = -0.005;
		} else {
			q0 = 0.0023;
			q1 = -0.83;
			q2 = -0.553;
			q3 = 0.0023;
		}

	} else if (hx > 0 && hy < 0) {
		if (fabs(hx / hy) >= 1) {
			q0 = 0.0025;
			q1 = 0.978;
			q2 = -0.199;
			q3 = 0.008;
		} else {
			q0 = 0.0025;
			q1 = 0.83;
			q2 = -0.56;
			q3 = 0.0045;
		}
	}
}

/**
 * Update imu AHRS
 * @usage  call in main() function
 */
static void IMU_AHRSUpdate()
{
	float norm;
	float hx, hy, hz, bx, bz;
	float vx, vy, vz, wx, wy, wz;
	float ex, ey, ez, halfT;
	float tempq0,tempq1,tempq2,tempq3;

	float q0q0 = q0*q0;
	float q0q1 = q0*q1;
	float q0q2 = q0*q2;
	float q0q3 = q0*q3;
	float q1q1 = q1*q1;
	float q1q2 = q1*q2;
	float q1q3 = q1*q3;
	float q2q2 = q2*q2;
	float q2q3 = q2*q3;
	float q3q3 = q3*q3;

	gx = info.wx;
	gy = info.wy;
	gz = info.wz;
	ax = info.ax;
	ay = info.ay;
	az = info.az;
	mx = info.mx;
	my = info.my;
	mz = info.mz;

	nowUpdate  = HAL_GetTick(); /* ms */
	halfT       = ((float)(nowUpdate - lastUpdate) / 2000.0f);
	lastUpdate = nowUpdate;

	/* Fast inverse square-root */
	norm = IMU_InvSqrt(ax*ax + ay*ay + az*az);
	ax = ax * norm;
	ay = ay * norm;
	az = az * norm;

	norm = IMU_InvSqrt(mx*mx + my*my + mz*mz);
	mx = mx * norm;
	my = my * norm;
	mz = mz * norm;
	/* compute reference direction of flux */
	hx = 2.0f*mx*(0.5f - q2q2 - q3q3) + 2.0f*my*(q1q2 - q0q3) + 2.0f*mz*(q1q3 + q0q2);
	hy = 2.0f*mx*(q1q2 + q0q3) + 2.0f*my*(0.5f - q1q1 - q3q3) + 2.0f*mz*(q2q3 - q0q1);
	hz = 2.0f*mx*(q1q3 - q0q2) + 2.0f*my*(q2q3 + q0q1) + 2.0f*mz*(0.5f - q1q1 - q2q2);
	bx = sqrt((hx*hx) + (hy*hy));
	bz = hz;

	/* estimated direction of gravity and flux (v and w) */
	vx = 2.0f*(q1q3 - q0q2);
	vy = 2.0f*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;
	wx = 2.0f*bx*(0.5f - q2q2 - q3q3) + 2.0f*bz*(q1q3 - q0q2);
	wy = 2.0f*bx*(q1q2 - q0q3) + 2.0f*bz*(q0q1 + q2q3);
	wz = 2.0f*bx*(q0q2 + q1q3) + 2.0f*bz*(0.5f - q1q1 - q2q2);

	/*
	 * error is sum of cross product between reference direction
	 * of fields and direction measured by sensors
	 */
	ex = (ay*vz - az*vy) + (my*wz - mz*wy);
	ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
	ez = (ax*vy - ay*vx) + (mx*wy - my*wx);

	/* PI */
	if(ex != 0.0f && ey != 0.0f && ez != 0.0f) {
		exInt = exInt + ex * CONFIG_IMU_KI * halfT;
		eyInt = eyInt + ey * CONFIG_IMU_KI * halfT;
		ezInt = ezInt + ez * CONFIG_IMU_KI * halfT;

		gx = gx + CONFIG_IMU_KP*ex + exInt;
		gy = gy + CONFIG_IMU_KP*ey + eyInt;
		gz = gz + CONFIG_IMU_KP*ez + ezInt;
	}

	tempq0 = q0 + (-q1*gx - q2*gy - q3*gz) * halfT;
	tempq1 = q1 + (q0*gx + q2*gz - q3*gy) * halfT;
	tempq2 = q2 + (q0*gy - q1*gz + q3*gx) * halfT;
	tempq3 = q3 + (q0*gz + q1*gy - q2*gx) * halfT;

	/* normalise quaternion */
	norm = IMU_InvSqrt(tempq0*tempq0 + tempq1*tempq1 + tempq2*tempq2 + tempq3*tempq3);
	q0 = tempq0 * norm;
	q1 = tempq1 * norm;
	q2 = tempq2 * norm;
	q3 = tempq3 * norm;
}

/**
 * Update imu attitude
 * @usage  call in main() function
 */
static void IMU_AttitudeUpdate()
{
	/* yaw    -pi----pi */
	info.yaw = atan2(2*q1*q2 + 2*q0*q3, -2*q2*q2 - 2*q3*q3 + 1)* 57.3;
	if (info.yaw < 0) {
		info.yaw = 360 + info.yaw;
	}
	/* pitch  -pi/2----pi/2 */
	info.rol = -asin(-2*q1*q3 + 2*q0*q2)* 57.3;
	/* roll   -pi----pi  */
	info.pit = -atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1)* 57.3;
}

/**
 * 获取IMU结果, 只允许读取, 请勿修改!
 * @return 信息指针
 */
const IMU_Info* IMU_GetData()
{
	return &info;
}

/**
 * IMU数据读取任务
 */
static void IMU_Task()
{
	while(1) {
		IMU_MPUGetData();
		IMU_AHRSUpdate();
		IMU_AttitudeUpdate();
		/* IMU恒温 */
		PWM_Set(CONFIG_IMU_HEAT_POWER_PIN, PID_Calc(&heatPID, info.temp, CONFIG_IMU_HEAT_TARGET_TEMP));
		osDelayUntil(1);
	}
}

/**
 * IMU初始化
 */
void IMU_Init()
{
	IMU_MPUDeviceInit();
	IMU_InitQuaternion();
	PID_CREATE_FROM_CONFIG(IMU_HEAT, &heatPID);
	/* 创建任务 */
	static osThreadId_t imuTaskHandle;
	const osThreadAttr_t imuTaskAttributes = {
			.name = "imuTask",
			.priority = (osPriority_t) osPriorityHigh,
			.stack_size = 128 * 4
	};
	imuTaskHandle = osThreadNew(IMU_Task, NULL, &imuTaskAttributes);
	UNUSED(imuTaskHandle);
}

#endif
