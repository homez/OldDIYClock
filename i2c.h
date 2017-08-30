#ifndef _I2C_H_
#define _I2C_H_

#include "pinout.h"
#include "sys.h"

#define I2C_NOACK	0
#define I2C_ACK		1
#define I2C_READ	1

#define SDA sda
#define SCL scl

#define I2C_Delay delay_1us();

void I2CswStart(uint8_t addr);
bit I2CswStartACKS(uint8_t addr);
void I2CswStop(void);
void I2CswWriteByte(uint8_t value);
bit I2CswWriteByteACKS(uint8_t value);
uint8_t I2CswReadByte(bit ack);


#endif /* _I2C_H_ */
