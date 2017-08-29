#include "bmp180.h"
#include "i2c.h"
#include "delay.h"
#include "sys.h"

 bmp180CalData cd;

 int16_t temperature = 0;
 int16_t pressure = 0;

 uint8_t bmp180Sensor = BMP_NONE;

 uint16_t bmp180ReadI2CWord(uint8_t addr)
{
	uint16_t ret;

	I2CswStart(BMP180_ADDR);
	I2CswWriteByte(addr);
	I2CswStart(BMP180_ADDR | I2C_READ);
	ret = ((uint16_t)I2CswReadByte(I2C_ACK)) << 8;
	ret |= I2CswReadByte(I2C_NOACK);
	I2CswStop();

	return ret;
}

void bmp180Init(void)
{
	bit BMxx80Pres = !I2CswStartACKS(BMP180_ADDR);
	if(BMxx80Pres) {
		I2CswWriteByte(BMP180_CHIP_ID);
		I2CswStop();
		I2CswStart(BMP180_ADDR | I2C_READ);
		bmp180Sensor = I2CswReadByte(I2C_NOACK);
	}
	I2CswStop();
	if( bmp180Sensor == 0x00 || bmp180Sensor == 0xFF ) {
		bmp180Sensor = BMP_NONE;
		return;
	}
	//			BMxx80_REGISTER_SOFTRESET, 0xB6
	I2CswStart(BMP180_ADDR);
	I2CswWriteByte(BMP180_REGISTER_SOFTRESET);
	I2CswWriteByte(0xB6);
	I2CswStop();
	delay_ms(150);
	delay_ms(150);
	/*
	for (i = 0; i < BMP180_CAL_LEN; i++) {
		param = bmp180ReadI2CWord(BMP180_CAL_DATA_START + 2 * i);
		if (param == 0x0000 || param == 0xFFFF) {
			bmp180Sensor = 0;										// No valid sensor on bus
			return;
		}
		*((uint8_t*)(&cd.ac1 + (i<<1))) = (param >> 8);
		*((uint8_t*)(&cd.ac1 + (i<<1) +1)) = param;
	}
	*/
	cd.ac1 = bmp180ReadI2CWord(BMP180_CAL_DATA_START);
	cd.ac2 = bmp180ReadI2CWord(BMP180_CAL_DATA_START+2);
	cd.ac3 = bmp180ReadI2CWord(BMP180_CAL_DATA_START+4);
	cd.ac4 = bmp180ReadI2CWord(BMP180_CAL_DATA_START+6);
	cd.ac5 = bmp180ReadI2CWord(BMP180_CAL_DATA_START+8);
	cd.ac6 = bmp180ReadI2CWord(BMP180_CAL_DATA_START+10);
	cd.b1 = bmp180ReadI2CWord(BMP180_CAL_DATA_START+12);
	cd.b2 = bmp180ReadI2CWord(BMP180_CAL_DATA_START+14);
	cd.mb = bmp180ReadI2CWord(BMP180_CAL_DATA_START+16);
	cd.mc = bmp180ReadI2CWord(BMP180_CAL_DATA_START+18);
	cd.md = bmp180ReadI2CWord(BMP180_CAL_DATA_START+20);

	return;
}

uint8_t bmp180HaveSensor(void)
{
	return bmp180Sensor;
}

uint16_t bmp180GetRawData(uint8_t param)
{
	uint16_t ret;

	I2CswStart(BMP180_ADDR);
	I2CswWriteByte(BMP180_CTRL_REG);
	I2CswWriteByte(param);
	I2CswStop();

	delay_ms(BMP085_CONV_TIME);

	ret = bmp180ReadI2CWord(BMP180_CONV_REG);

	return ret;
}


void bmp180Convert (void)
{
	int32_t x1, x2, x3, b3, b5, b6, b8, p;
	uint32_t b4, b7;

	uint16_t ut = bmp180GetRawData(BMP180_TEMP_MEASURE);
	uint16_t up = bmp180GetRawData(BMP180_PRESSURE_MEASURE);

	if ( bmp180Sensor == BMP180 ) {
		x1 = ((int32_t)ut - cd.ac6) * cd.ac5 >> 15;
		x2 = ((int32_t)cd.mc << 11) / (x1 + cd.md);
		b5 = x1 + x2;

		temperature = (b5 + 8) >> 4;							// Value in 0.01 °C

		b6 = b5 - 4000;
		b8 = (b6 * b6) >> 12;
		x3 = (b8 * cd.b2 + b6 * cd.ac2) >> 11;
		b3 = (x3 +  4 * (int32_t)cd.ac1 + 2) >> 2;
		x1 = (b6 * cd.ac3) >> 13;
		x2 = (b8 * cd.b1) >> 16;
		x3 = ((x1 + x2) + 2) >> 2;
		b4 = (cd.ac4 * (uint32_t)(x3 + 32768)) >> 15;
		b7 = (((uint32_t) up - b3) * (50000));

		p = (b7 / b4) << 1;

		x1 = (p >> 8) * (p >> 8);
		x1 = (x1 * 3038) >> 16;
		x2 = (-7357 * p) >> 16;

		pressure = (p + ((x1 + x2 + 3791) >> 4)) * 3 / 40;		// Value in 10*mmHg
		//	pressure = (p + ((x1 + x2 + 3791) >> 4)) * 4 / 400;		// Value in 10*kPa
	}
}

int16_t bmp180GetTemp(void)
{
	return temperature;
}

int16_t bmp180GetPressure(void)
{
	return pressure;
}