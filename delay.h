#ifndef _DELAY_H_
#define _DELAY_H_

#include "sys.h"

#if (SYSCLK == SYSCLK_11)
	// @11.059MHz
	#define DELAY_1us 2
	#define DELAY_5us 10
	#define DELAY_1ms _nop_(); _nop_(); _nop_(); i = 11; j = 190;
#else
	// @18.432MHz
	#define DELAY_1us 2
	#define DELAY_5us 19
	#define DELAY_1ms i = 18; j = 235;
#endif

void delay_1us(void);
void delay_5us(uint8_t us);
void delay_ms(uint8_t ms);

#endif /* _DELAY_H_ */
