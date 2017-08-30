#ifndef _RTC_H_
#define _RTC_H_

#include "sys.h"

#define RTC_I2C_ADDR		0x68 << 1

enum {
	RTC_SEC = 0,
	RTC_MIN,
	RTC_HOUR,
	RTC_WDAY,
	RTC_DATE,
	RTC_MONTH,
	RTC_YEAR,
	RTC_ETM,
	RTC_AGING = 0x10,
	RTC_TEMP = 0x11
};

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t wday;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t etm;
	uint8_t temp;
} RTC_type;

extern RTC_type rtc;

#define RTC_NOEDIT			0xFF

void rtcInit(void);
void rtcReadTime(void);
void rtcSaveTime(void);
void rtcSaveDate(void);
void rtcSavePPM(void);
void rtcNextEditParam(void);
void rtcChangeTime(int8_t diff);
uint16_t rtcYearDay(void);
uint8_t rtcDaysInMonth(void);

uint8_t rtcBinDecToDec(uint8_t num);
uint8_t rtcDecToBinDec(uint8_t num);

#endif /* _RTC_H_ */