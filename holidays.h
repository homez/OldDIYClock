#ifndef _HOLIDAYS_H_
#define _HOLIDAYS_H_

#include "sys.h"
enum {
	W1 = 0,
	W2,
	W3,
	W4,
	W5
};

typedef struct __HOLIDAY__ {
	uint8_t month;
	uint8_t day;
	uint8_t length;
	uint8_t code *ptr;
} HOLIDAY;

enum {
	MON = 1,
	TUE,
	WED,
	THU,
	FRY,
	SAT,
	SUN
};


#define hAN(month,date,s)	{ month, date, ELEMENTS(s), &s }
#define hADN(day,s)	{ 0x60 | ((day >> 8) & 0x0F), (day & 0xFF), ELEMENTS(s), &s }
#define hAWN(month,wnum,wday,s)	{ (( wnum + 1 ) << 4 | month), wday, ELEMENTS(s), &s }
#define hAWL(month,wday,s)	{ (0x70 | month), wday, ELEMENTS(s), &s }


extern bit holiday;
extern uint8_t wdays[5];

void checkHolidays(void);

#endif /* _HOLIDAYS_H_ */
