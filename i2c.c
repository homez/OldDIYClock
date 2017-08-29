#include "pinout.h"
#include "sys.h"
#include "delay.h"
#include "i2c.h"

static bit I2CswGetBit(void)
{
	bit result;
	
	I2C_Delay;
	SCL = 1;
	I2C_Delay;
	result = SDA;
	SCL = 0;

	return result;
}

static void I2CswSendBit(bit value)
{
	SDA = value;
	I2CswGetBit();
}

bit I2CswStartACKS(uint8_t addr)
{
	SCL = 1;
	SDA = 1;
	I2C_Delay;
	SDA = 0;
	I2C_Delay;
	SCL = 0;

	return I2CswWriteByteACKS(addr);
}

void I2CswStart(uint8_t addr)
{
	SCL = 1;
	SDA = 1;
	I2C_Delay;
	SDA = 0;
	I2C_Delay;
	SCL = 0;

	I2CswWriteByte(addr);

	return;
}

void I2CswStop(void)
{
	SCL = 0;
	SDA = 0;
	I2C_Delay;
	SCL = 1;
	I2C_Delay;
	SDA = 1;
}

void I2CswWriteByte(uint8_t value)
{
	uint8_t i = 0;

	for (i = 0; i < 8; i++) {
		I2CswSendBit(value & 0x80);
		value <<= 1;
	}
	I2CswSendBit(I2C_ACK);

	return;
}

bit I2CswWriteByteACKS(uint8_t value)
{
	uint8_t i = 0;

	for (i = 0; i < 8; i++) {
		I2CswSendBit(value & 0x80);
		value <<= 1;
	}
	SDA = 1;
	return I2CswGetBit();
}

uint8_t I2CswReadByte(bit ack)
{
	uint8_t i, ret;

	SDA = 1;

	ret = 0;
	for (i = 0; i < 8; i++) {
		ret <<= 1;
		if (I2CswGetBit())
			ret |= 0x01;
	}
	I2CswSendBit(!ack);

	return ret;
}
