#include "MPU6050.h"
#include "main.h"
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
void MPU6050_Init()
{
	uint8_t data;
	
	data=0x01;
	HAL_I2C_Mem_Write(&hi2c1, 0xD0, MPU6050_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT,&data, 1, 100);
	HAL_Delay(50); 
	
	data=0x00;
	HAL_I2C_Mem_Write(&hi2c1, 0xD0, MPU6050_PWR_MGMT_2, I2C_MEMADD_SIZE_8BIT,&data, 1, 100);
	
	data=0x09;
	HAL_I2C_Mem_Write(&hi2c1, 0xD0, MPU6050_SMPLRT_DIV, I2C_MEMADD_SIZE_8BIT,&data, 1, 100);
	
	data=0x06;
	HAL_I2C_Mem_Write(&hi2c1, 0xD0, MPU6050_CONFIG, I2C_MEMADD_SIZE_8BIT,&data, 1, 100);
	
	data=0x00;
	HAL_I2C_Mem_Write(&hi2c1, 0xD0, MPU6050_GYRO_CONFIG, I2C_MEMADD_SIZE_8BIT,&data, 1, 100);
}
