#ifndef _SI7021_H_
#define _SI7021_H_

#include "sys.h"

#define SI7021_ADDR 0x40 << 1
#define SI7021_DELAY_INTERVAL 20

// I2C commands
#define RH_READ														0xE5
#define TEMP_READ													0xE3
#define POST_RH_TEMP_READ									0xE0
#define SI7021_MEASURE_TEMP_NOHOLD_CMD		0xF3
#define SI7021_MEASURE_HUM_NOHOLD_CMD			0xF5
#define SI7021_WRITE_USER_REG							0xE6
#define SI7021_READ_USER_REG							0xE7
#define SI7021_RESET_CMD									0xFE
#define SI7021_USER_REG_DEFAULT						0x3A

// compound commands
//const uint8_t code SI7021_SERIAL1_READ[2]      ={ 0xFA, 0x0F };
//const uint8_t code SI7021_SERIAL2_READ[2]      ={ 0xFC, 0xC9 };

void si7021Init(void);
bit si7021SensorExists(void);
void si7021Convert(void);
int16_t si7021GetTemp(void);
uint16_t si7021GetHumidity(void);


#endif /* _SI7021_H_ */
