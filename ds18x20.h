#ifndef _DS18X20_H_
#define _DS18X20_H_

#include "pinout.h"
#include "sys.h"

// DS18X20 commands
#define DS18X20_CMD_SEARCH_ROM		0xF0
#define DS18X20_CMD_READ_ROM		0x33
#define DS18X20_CMD_MATCH_ROM		0x55
#define DS18X20_CMD_SKIP_ROM		0xCC
#define DS18X20_CMD_ALARM_SEARCH	0xEC
#define DS18X20_CMD_CONVERT			0x44
#define DS18X20_CMD_WRITE_SCRATCH	0x4E
#define DS18X20_CMD_READ_SCRATCH	0xBE
#define DS18X20_CMD_COPY_SCRATCH	0x48
#define DS18X20_CMD_RECALL_EE		0xB8
#define DS18X20_CMD_READ_POWER		0xB4

#define DS18S20_FAMILY_CODE			0x10
#define DS18B20_FAMILY_CODE			0x28

#define DS18X20_MAX_DEV				4
#define DS18X20_SCRATCH_LEN			9
#define DS18X20_SCRATCH_TEMP_LEN	2
#define DS18X20_ID_LEN				8

typedef struct {
	uint8_t sp[DS18X20_SCRATCH_TEMP_LEN];
	uint8_t id[DS18X20_ID_LEN];
} ds18x20Dev;

bit ds18x20IsOnBus(void);
void ds18x20SendBit(bit value);
bit ds18x20GetBit(void);
void ds18x20SendByte(uint8_t value);
uint8_t ds18x20GetByte(void);
void ds18x20Select(ds18x20Dev *dev);
void ds18x20GetAllTemps();
void ds18x20ConvertTemp(void);
uint8_t ds18x20SearchRom(uint8_t *bitPattern, uint8_t lastDeviation);
void ds18x20SearchDevices(void);
uint8_t ds18x20Process(void);
int16_t ds18x20GetTemp(uint8_t num);
uint8_t ds18x20GetDevCount(void);
uint8_t _crc_ibutton_update(uint8_t crc, uint8_t value);


#endif /* _DS18X20_H_ */
