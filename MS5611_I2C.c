#include "MS5611_I2C.h"
#include "i2c.h"
#include "gpio.h"
#include <math.h>

struct _MS5611_msg MS5611_Val;
uint16_t C1, C2, C3, C4, C5, C6;
//uint32_t D1, D2;
uint8_t data[3] = {0, 0, 0};
//const float sea_press = 1013.25;

static uint16_t MS5611_read_16bits(uint8_t reg){
	//if(HAL_I2C_Mem_Read(&hi2c2, MS5611_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, TwoBytes, I2C_TIMEOUT)!= HAL_OK)
	//	printf ("HAL_SPI_TransmitReceive \n");
	HAL_I2C_Mem_Read(&hi2c1, MS5611_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, TwoBytes, PORT_TIMEOUT);
	return ((uint16_t)data[0] << 8) | ((uint16_t)data[1] & 0xFF);
}

static void MS5611_write(uint8_t reg){
	//if(HAL_I2C_Master_Transmit(&hi2c2, MS5611_I2C_ADDR, &reg, OneByte, I2C_TIMEOUT) != HAL_OK)
	//	printf ("HAL_SPI_Transmit \n");
	if(HAL_I2C_Master_Transmit(&hi2c1, MS5611_I2C_ADDR, &reg, OneByte, PORT_TIMEOUT) != HAL_OK){
		
	}
}

static uint32_t MS5611_Convert(uint8_t cmd){
	MS5611_write(cmd);
	HAL_Delay(CONVERSION_TIME);
	//if(HAL_I2C_Mem_Read(&hi2c2, MS5611_I2C_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, data, ThreeBytes, I2C_TIMEOUT)!= HAL_OK)
	//	printf ("HAL_SPI_TransmitReceive \n");
	HAL_I2C_Mem_Read(&hi2c1, MS5611_I2C_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, data, ThreeBytes, PORT_TIMEOUT);
	return ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | ((uint32_t)data[2]);
}

void MS5611_Init(void){
	MS5611_write(CMD_MS5611_RESET);
	HAL_Delay(4);
	// We read the factory calibration
	// The on-chip CRC is not used
	C1 = MS5611_read_16bits(CMD_MS5611_PROM_C1);
	C2 = MS5611_read_16bits(CMD_MS5611_PROM_C2);
	C3 = MS5611_read_16bits(CMD_MS5611_PROM_C3);
	C4 = MS5611_read_16bits(CMD_MS5611_PROM_C4);
	C5 = MS5611_read_16bits(CMD_MS5611_PROM_C5);
	C6 = MS5611_read_16bits(CMD_MS5611_PROM_C6);
	//printf("C1 = %d C2 = %d C3 = %d C4 = %d C5 = %d C6 = %d\n", C1, C2, C3, C4, C5, C6);
}

void MS5611_read(void){
	int64_t dT, OFF, SENS, TEMP, T2, OFF2, SENS2, x1, x2;
	int32_t D1, D2;
	D1 = MS5611_Convert(CMD_MS5611_PRES);//raw press
	D2 = MS5611_Convert(CMD_MS5611_TEMP);// raw temp
	//printf("D1 = %d D2 = %d\n", D1, D2);
	// Difference between actual and reference temperature = D2 - Tref
	dT = D2 - ((uint64_t)C5 << 8);
	//printf("dT = %llu\n", dT);
	// Actual temperature = 2000 + dT * TEMPSENS
	TEMP = (int64_t)MS561101BA_2K + ((dT * (int64_t)C6) >> 23);
	//Offset at actual temperature
	OFF = ((uint32_t)C2 << 16) + ((dT * (int64_t)C4) >> 7);
	SENS = ((int64_t)C1 << 15) + (((int64_t)C3 * dT) >> 8);
	// Second order temperature compensation
	if (TEMP < MS561101BA_2K)	{
		x1 = (TEMP - MS561101BA_2K);
		x2 = x1 * x1;
		T2 = ((dT * dT) >> 31);
		OFF2 = ((5 * x2) >> 1);
		SENS2 = ((5 * x2) >> 2);
		if(TEMP < -1500)		{
			x1 = (TEMP + MS561101BA_1K5);
			x2 = x1 * x1;
			OFF2 += (7 * x2);
			SENS2 += ((11 * x2) >> 1);
		}
	}	else {
		T2 = 0;
		OFF2 = 0 ;
		SENS2 = 0 ;
	}
	OFF = OFF - OFF2;
	SENS = SENS - SENS2;
	// Temperature compensated pressure = D1 * SENS - OFF
	MS5611_Val.Temperature = TEMP - T2;
	MS5611_Val.Pressure = ((((D1 * SENS) >> 21) - OFF) >> 15);
}

//float MS5611_getAltitude(void) {
//	int32_t _p, _t;
//	MS5611_read(&_p, &_t);
//	printf("P = %f mbar t = %f degC\n", _p/100.0, _t/100.0);
//	return ((pow((1015.7f / _p), CONST_PF) - 1.0f) * (FIX_TEMP + 273.15f)) / 0.0065f;
//}
