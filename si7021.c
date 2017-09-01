#include "si7021.h"
#include "i2c.h"
#include "pinout.h"
#include "delay.h"
#include "sys.h"

bit _si_exists = 0;

int16_t celsiusHundredths = 0;
uint16_t humidityPercent = 0;

void _writeReg(uint8_t * reg, uint8_t reglen)
{
	uint8_t i;
	I2CswStart(SI7021_ADDR);
	for( i = 0; i < reglen; i++ ) {
		reg += i;
		I2CswWriteByte(*reg);
	}
	I2CswStop();

	return;
}

void _readReg(uint8_t * reg, uint8_t reglen)
{
	uint8_t i;
	I2CswStart(SI7021_ADDR|I2C_READ);
	for( i = 0; i < reglen; i++ ) {
		reg[i] = I2CswReadByte(I2C_ACK);
	}
	I2CswReadByte(I2C_NOACK);
	I2CswStop();

	return;
}

void _command(uint8_t _cmd, uint8_t * buf)
{
	_writeReg(&_cmd, sizeof _cmd);
	delay_ms(SI7021_DELAY_INTERVAL);
	_readReg(buf, 2);

	return;
}

int16_t _getCelsiusPostHumidity()
{
	int32_t tempraw;
	uint8_t tempbytes[2];
	_command(SI7021_MEASURE_TEMP_NOHOLD_CMD, tempbytes);
	tempraw = (int32_t)tempbytes[0] << 8 | tempbytes[1];
	tempraw &= 0xFFFC;
	return ((17572 * tempraw) >> 16) - 4685;
}

uint16_t getHumidityBasisPoints() {
	int32_t humraw;
	uint8_t humbytes[2];
	_command(SI7021_MEASURE_HUM_NOHOLD_CMD, humbytes);
	humraw = (int32_t)humbytes[0] << 8 | humbytes[1];
	humraw &= 0xFFFC;
	return ((12500 * humraw) >> 16) - 600;
}

void si7021Init(void)
{
	_si_exists = !I2CswStartACKS(SI7021_ADDR);
	I2CswStop();
	if( !_si_exists ) {
		return;
	}
	//si7021Reset();

	return;
}

void si7021Convert(void)
{
	celsiusHundredths = _getCelsiusPostHumidity()/10; // 0.1
	humidityPercent = getHumidityBasisPoints();
}

int16_t si7021GetTemp(void)
{
	return celsiusHundredths;
}

uint16_t si7021GetHumidity(void)
{
	return humidityPercent;
}

bit si7021SensorExists(void)
{
	return _si_exists;
}
