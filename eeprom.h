#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "sys.h"

//#define ENABLE_IAP 0x80           //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81           //if SYSCLK<24MHz
#if (SYSCLK == SYSCLK_18)
	// @18.432MHz
	#define ENABLE_IAP  0x82            //if SYSCLK<20MHz
#else
	// @11.059MHz
	#define ENABLE_IAP 0x83           //if SYSCLK<12MHz
#endif
//#define ENABLE_IAP 0x84           //if SYSCLK<6MHz
//#define ENABLE_IAP 0x85           //if SYSCLK<3MHz
//#define ENABLE_IAP 0x86           //if SYSCLK<2MHz
//#define ENABLE_IAP 0x87           //if SYSCLK<1MHz

#define CMD_IDLE    0
#define CMD_READ    1
#define CMD_PROGRAM 2
#define CMD_ERASE   3

void IapIdle(void);
uint8_t IapReadByte(uint16_t addr);
void IapProgramByte(uint16_t addr, uint8_t dat);
void IapEraseSector(uint16_t addr);

#endif /* _EEPROM_H_ */