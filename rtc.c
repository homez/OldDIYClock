#include "pinout.h"
#include "sys.h"
#include "rtc.h"
#include "delay.h"
#include "settings.h"
#include "i2c.h"

RTC_type rtc;

static code RTC_type rtcMin = {0, 0, 0, 1, 1, 1, 0, RTC_NOEDIT,0};
static code RTC_type rtcMax = {59, 59, 23, 7, 31, 12, 99, RTC_NOEDIT,99};

static code uint16_t rtcMonthNumberDay[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

static void rtcWeekDay(void)
{
	uint8_t a, y, m;

	a = (rtc.month > 2 ? 0 : 1);
	y = 12 + rtc.year - a;
	m = rtc.month + 12 * a - 2;

	rtc.wday = (rtc.date + y + (y / 4) + ((31 * m) / 12) - 1) % 7;

	if (rtc.wday == 0)
		rtc.wday = 7;

	return;
}

uint8_t rtcDaysInMonth(void)
{
	uint8_t ret = rtc.month;

	if (ret == 2) {
		ret = rtc.year & 0x03;
		ret = (ret ? 29 : 28);
	}
	else {
		if (ret > 7)
			ret++;
		ret |= 30;
	}

	return ret;
}

void rtcInit(void)
{
	uint8_t wday;

	I2CswStart(RTC_I2C_ADDR);
	I2CswWriteByte(0x0A);
	I2CswWriteByte(0x81);
	I2CswStop();
	I2CswStart(RTC_I2C_ADDR);
	I2CswWriteByte(0x0D);
	I2CswWriteByte(0x81);
	I2CswWriteByte(0x04);
	I2CswWriteByte(0x00);
	I2CswWriteByte(eep.timecoef);
	I2CswStop();
	
	rtcReadTime();
	wday = rtc.wday;
	rtcWeekDay();
	if(wday != rtc.wday) {
		rtcSaveDate();
	}

	return;
}

void rtcSaveTime(void)
{
	uint8_t i;

	I2CswStart(RTC_I2C_ADDR);
	I2CswWriteByte(RTC_SEC);
	for (i = RTC_SEC; i <= RTC_HOUR; i++)
		I2CswWriteByte(rtcDecToBinDec(*((uint8_t*)&rtc + i)));
	I2CswStop();

	return;
}

void rtcSaveDate(void)
{
	uint8_t i;

	rtcWeekDay();

	I2CswStart(RTC_I2C_ADDR);
	I2CswWriteByte(RTC_WDAY);
	for (i = RTC_WDAY; i <= RTC_YEAR; i++)
		I2CswWriteByte(rtcDecToBinDec(*((uint8_t*)&rtc + i)));
	I2CswStop();

	return;
}

void rtcSavePPM(void)
{
	I2CswStart(RTC_I2C_ADDR);
	I2CswWriteByte(RTC_AGING);
	I2CswWriteByte(eep.timecoef);
	I2CswStop();

	return;
}

void rtcReadTime(void)
{
	uint8_t i, temp;

	I2CswStart(RTC_I2C_ADDR);
	I2CswWriteByte(RTC_SEC);
	I2CswStop();
	I2CswStart(RTC_I2C_ADDR | I2C_READ);
	for (i = RTC_SEC; i <= RTC_YEAR; i++) {
		temp = I2CswReadByte((i == RTC_YEAR) ? I2C_NOACK : I2C_ACK);
		*((uint8_t*)&rtc + i) = rtcBinDecToDec(temp);
	}
	I2CswStop();
	I2CswStart(RTC_I2C_ADDR);
	I2CswWriteByte(RTC_TEMP);
	I2CswStop();
	I2CswStart(RTC_I2C_ADDR | I2C_READ);
	rtc.temp = I2CswReadByte(I2C_NOACK);
	I2CswStop();

	return;
}

void rtcChangeTime(int8_t diff)
{
	int8_t *time = (int8_t*)&rtc + rtc.etm;
	int8_t timeMax = *((int8_t*)&rtcMax + rtc.etm);
	int8_t timeMin = *((int8_t*)&rtcMin + rtc.etm);

	if (rtc.etm == RTC_DATE)
		timeMax = rtcDaysInMonth();

	*time += diff;

	if (*time > timeMax)
		*time = timeMin;
	if (*time < timeMin)
		*time = timeMax;

	return;
}

void rtcNextEditParam(void)
{
	switch (rtc.etm) {
	case RTC_HOUR:
	case RTC_MIN:
		rtc.etm--;
		break;
	case RTC_SEC:
		rtc.etm = RTC_DATE;
		break;
	case RTC_DATE:
	case RTC_MONTH:
	case RTC_YEAR:
		rtc.etm--;
		break;
	default:
		rtc.etm = RTC_HOUR;
		break;
	}

	return;
}

uint16_t rtcYearDay(void)
{
	return rtcMonthNumberDay[rtc.month-1] + rtc.date + (((rtc.year % 4 == 0) && (rtc.month>2))? 1 : 0);
}

uint8_t rtcBinDecToDec(uint8_t num)
{
	return (num >> 4) * 10 + (num & 0x0F);
}

uint8_t rtcDecToBinDec(uint8_t num)
{
	return ((num / 10) << 4) + (num % 10);
}
