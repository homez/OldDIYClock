#ifndef _BMXX80_H_
#define _BMXX80_H_

#include "sys.h"

#define BMXX80_ADDR0					0x76 << 1
#define BMXX80_ADDR1					0x77 << 1

// Conversion parameters
#define BMP180_CONV_REG				0xF6
#define BMP085_CONV_TIME			5

// EEPROM calibration addresses
#define BMP180_CAL_DATA_START		0xAA

// Oversampling settings
#define BMP180_OSS					0

// BMP180 registers
#define BMP180_CTRL_REG				0xF4
#define BMP180_TEMP_MEASURE			0x2E
#define BMP180_PRESSURE_MEASURE		(0x34 | (BMP180_OSS << 6))


#define BMP_NONE 0x00
#define BMP180_CHIP_ID			0x55
#define BMP280_CHIP_ID1			0x56
#define BMP280_CHIP_ID2			0x57
#define BMP280_CHIP_ID3			0x58
#define BME280_CHIP_ID			0x60

enum
{
	BMP180_REGISTER_DIG_AC1             = 0xAA,
	BMP180_REGISTER_DIG_AC2             = 0xAC,
	BMP180_REGISTER_DIG_AC3             = 0xAE,
	BMP180_REGISTER_DIG_AC4             = 0xB0,
	BMP180_REGISTER_DIG_AC5             = 0xB2,
	BMP180_REGISTER_DIG_AC6             = 0xB4,
	BMP180_REGISTER_DIG_B1              = 0xB6,
	BMP180_REGISTER_DIG_B2              = 0xB8,
	BMP180_REGISTER_DIG_MB              = 0xBA,
	BMP180_REGISTER_DIG_MC              = 0xBC,
	BMP180_REGISTER_DIG_MD              = 0xBE,

	BMX280_REGISTER_DIG_T1              = 0x88,
	BMX280_REGISTER_DIG_T2              = 0x8A,
	BMX280_REGISTER_DIG_T3              = 0x8C,

	BMX280_REGISTER_DIG_P1              = 0x8E,
	BMX280_REGISTER_DIG_P2              = 0x90,
	BMX280_REGISTER_DIG_P3              = 0x92,
	BMX280_REGISTER_DIG_P4              = 0x94,
	BMX280_REGISTER_DIG_P5              = 0x96,
	BMX280_REGISTER_DIG_P6              = 0x98,
	BMX280_REGISTER_DIG_P7              = 0x9A,
	BMX280_REGISTER_DIG_P8              = 0x9C,
	BMX280_REGISTER_DIG_P9              = 0x9E,

	BME280_REGISTER_DIG_H1              = 0xA1,
	BME280_REGISTER_DIG_H2              = 0xE1,
	BME280_REGISTER_DIG_H3              = 0xE3,
	BME280_REGISTER_DIG_H4              = 0xE4,
	BME280_REGISTER_DIG_H5              = 0xE5,
	BME280_REGISTER_DIG_H6              = 0xE7,

	BMP180_REGISTER_DIG_T1              = 0x88,

	BMXX80_REGISTER_CHIPID             = 0xD0,
	BME280_REGISTER_VERSION            = 0xD1,
	BMXX80_REGISTER_SOFTRESET          = 0xE0,

	BME280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

	BME280_REGISTER_CONTROLHUMID       = 0xF2,
	BME280_REGISTER_STATUS             = 0xF3,
	BMXX80_REGISTER_CONTROL            = 0xF4,
	BME280_REGISTER_CONFIG             = 0xF5,
	BMX280_REGISTER_PRESSUREDATA       = 0xF7,
	BMX280_REGISTER_TEMPDATA           = 0xFA,
	BME280_REGISTER_HUMIDDATA          = 0xFD
};


typedef struct {
	int16_t ac1;
	int16_t ac2;
	int16_t ac3;
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t b1;
	int16_t b2;
	int16_t mb;
	int16_t mc;
	int16_t md;
} bmp180CalData;

typedef struct {
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;

	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;

	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
} bmx280CalData;

void bmxx80Init(void);
void bmxx80Convert(void);
int16_t bmxx80GetTemp(void);
int16_t bmxx80GetPressure(void);
int16_t bme280GetHumidity(void);
uint8_t bmxx80HaveSensor(void);

#endif /* _BMXX80_H_ */
