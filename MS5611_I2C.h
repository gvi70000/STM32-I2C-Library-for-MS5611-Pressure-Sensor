#ifndef __MS5611_I2C_H
#define __MS5611_I2C_H

#ifdef __cplusplus
extern "C" {
#endif
#include "i2c.h"
#define OneByte			    			  1
#define TwoBytes			    			2
#define ThreeBytes			    		3
#define MS5611_I2C_ADDR			    0xEC
#define CONVERSION_TIME					10 // conversion time in miliseconds
#define MS561101BA_2K						2000
#define MS561101BA_1K5					1500	
#define CMD_MS5611_RESET				0x1E
//#define CMD_MS5611_PROM_Setup		0xA0
#define CMD_MS5611_PROM_C1			0xA2
#define CMD_MS5611_PROM_C2			0xA4
#define CMD_MS5611_PROM_C3			0xA6
#define CMD_MS5611_PROM_C4			0xA8
#define CMD_MS5611_PROM_C5			0xAA
#define CMD_MS5611_PROM_C6			0xAC
//#define CMD_MS5611_PROM_CRC			0xAE
#define CMD_MS5611_PRES					0x48   // Maximum resolution (oversampling)
#define CMD_MS5611_TEMP					0x58   // Maximum resolution (oversampling)
#define FIX_TEMP								25	
#define CONST_PF								0.1902630958f //(1/5.25588f) Pressure factor	
	
	struct _MS5611_msg {
		int32_t Pressure;
		int32_t Temperature;
	};
	
	extern struct _MS5611_msg MS5611_Val;
	
	void MS5611_Init(void);
	void MS5611_read(void);
	//float MS5611_getAltitude(void); // in meter units
	
#ifdef __cplusplus
}
#endif

#endif /* __MS5611_I2C_H */
