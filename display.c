#include "sys.h"
#include "display.h"
#include "pinout.h"
#include "rtc.h"
#include "bmxx80.h"
#include "si7021.h"
#include "key.h"
#include "settings.h"
#include "alarm.h"
#include "fonts.h"
#include "weekpicture.h"
#include "menupicture.h"
#include "holidays.h"

uint8_t data disp[DISPLAYSIZE];

bit scroll_start;
uint8_t scroll_disp_index;

uint8_t msg_length;
uint8_t *msg_str;
uint8_t rndr_disp_index;
uint8_t rndr_index;
uint8_t rndr_line;


uint8_t *pdisp;
uint8_t code *fptr;
uint8_t displayBright;
uint8_t dispMode = MODE_MAIN;
uint8_t code hourbright[12] = { 0x00, 0x00, 0x12, 0x34, 0x55, 0x55, 0x55, 0x55, 0x55, 0x54, 0x32, 0x10 };

uint8_t menuNumber = MODE_EDIT_TIME;
uint8_t screenTime = 0;
uint8_t widgetNumber = 0;
bit reversed;
bit refstart;
uint8_t refcount;
uint8_t dotcount;
uint8_t buf[5];

void wiNext(void);
void wiTime(void);
void wiHoly(void);

Widget code widgets[7] = {
	{5, wiTime}, /* WI_TIME */
	{2, wiNext}, /* WI_DATE */ 
	{2, wiNext}, /* WI_WEEK */ 
	{2, wiNext}, /* WI_TEMP */ 
	{2, wiNext}, /* WI_PRES */ 
	{2, wiNext}, /* WI_HUMI */ 
	{0, wiHoly}, /* WI_HOLY */ 
};

void displayInit(void)
{
	P0M1 = 0x00;
	P0M0 = 0x3F;
	P1M1 = 0x00;
	P1M0 = 0xFF;
	P2M1 = 0x00;
	P2M0 = 0xFF;
	P3M1 = 0x00;
	P3M0 = 0x7F;
	displayBright = eep.bright;
	refstart = 0;
	reversed = key_mer;
	updateFont();
	pdisp = &disp[0];

	return;
}

void displayClear(void)
{
	P3 &= 0x80;
	P1 |= 0xff;
	P2 |= 0xff;
	P0 |= 0x3F;

	return;
}

void displayRefresh(void)
{
	static uint8_t columnrefnum = 0;
	static uint8_t columnrefwidth = 0;
	uint8_t k;

	if(columnrefwidth < 5 ) {
		if ( columnrefwidth == displayBright )
			displayClear();
		columnrefwidth++;
		return;
	}
	else {
		columnrefwidth = 0;
		displayClear();
		if(!reversed) {
			k = 21-columnrefnum;
		}
		else {
			k = columnrefnum;
		}
		switch(k) {
			case 0:		col1=0;	 break;
			case 1:		col2=0;	 break;
			case 2:		col3=0;	 break;	
			case 3:		col4=0;	 break;
			case 4:		col5=0;	 break;
			case 5:		col6=0;	 break;	
			case 6:		col7=0;	 break;
			case 7:		col8=0;	 break;
			case 8:		col9=0;	 break;
			case 9:		col10=0; break;
			case 10:	col11=0; break;
			case 11:	col12=0; break;
			case 12:	col13=0; break;
			case 13:	col14=0; break;	
			case 14:	col15=0; break;
			case 15:	col16=0; break;
			case 16:	col17=0; break;	
			case 17:	col18=0; break;
			case 18:	col19=0; break;
			case 19:	col20=0; break;
			case 20:	col21=0; break;
			case 21:	col22=0; break;
			default:break;
		}

		if(!reversed) {	
			P3 = disp[21-k];
		}
		else {
			P3 = ((disp[k]>>6)&0x01)|((disp[k]>>4)&0x02)|((disp[k]>>2)&0x04)| (disp[k]&0x08)|((disp[k]<<2)&0x10)|((disp[k]<<4)&0x20)|((disp[k]<<6)&0x40);
		}
		columnrefnum++;
		if(columnrefnum >= DISPLAYSIZE) {
			columnrefnum = 0;
			reversed = key_mer;
			if(refstart==0) {
				refstart = 1;
			}
		}
	}

	return;
}

void backToMainMode(uint8_t mode)
{
	switch(mode) {
		case SAVEANDBACK:
		case CANCELANDBACK: {
				while(refstart == 0) {}
				displayClear();
				EA = 0;
				if (mode == SAVEANDBACK) {
					settingsSave();
				}
				else {
					settingsInit();
				}
				EA = 1;
		}
		default:
		case BACK: {
			dispMode = MODE_MAIN;
			screenTime = 0;
			widgetNumber = 0;
		}
	}

	return;
}

void checkAlarm(void)
{
	static bit firstCheck = 1;

	rtcReadTime();

	// Once check if it's a new second
	if (rtc.sec == 0) {
		if (firstCheck) {
			firstCheck = 0;
			// Check alarm
			if (alarm.on && rtc.hour == alarm.hour && rtc.min == alarm.min) {
				if (*((int8_t*)&alarm.mon + rtc.wday - 1))
					alarmTimer = 60 * (uint16_t)eep.alarmTimeout;
			}
			else {
				// Check new hour
				if (rtc.hour > alarm.hour && rtc.min == 0 && eep.hourSignal)
					startBeeper(BEEP_LONG);
			}
		}
	}
	else {
		firstCheck = 1;
	}

	return;
}

void updateFont(void)
{
	switch(eep.fontMode) {
		default:
		case 0: {fptr = &num_font1[0]; break; }
		case 1: {fptr = &num_font2[0]; break; }
		case 2: {fptr = &num_font3[0]; break; }
		case 3: {fptr = &num_font4[0]; break; }
		case 4: {fptr = &num_font5[0]; break; }
	}

	return;
}

void writeToPtr(uint8_t value)
{
	*pdisp = value;
	pdisp++;
}

void showDS3231(void)
{
	uint8_t i, code *sptr = &pic_DS3231[0];

	for(i=0; i<DISPLAYSIZE; i++, sptr++, pdisp++) {
		*pdisp = *sptr;
	}

	return;
}

void showDot(void)
{
	uint8_t i, dot;

	switch(eep.dotMode) {
		case 0: {
			if (dotcount < 15) { dot = 0; }
			else if (dotcount < 45) { dot = 3; }
			else { dot = 0; }
			break;
		}
		default:
		case 1: {
			if (dotcount < 5) { dot = 0; }
			else if (dotcount < 13) { dot = 1; }
			else if (dotcount < 22) { dot = 2; }
			else if (dotcount < 39) { dot = 3; }
			else if (dotcount < 48) { dot = 2; }
			else if (dotcount < 56) { dot = 1; }
			else { dot = 0; }
			break;
		}
		case 2: {
			if (dotcount < 6) { dot = 0; }
			else if (dotcount < 18) { dot = 3; }
			else if (dotcount < 30) { dot = 4; }
			else if (dotcount < 42) { dot = 5; }
			else if (dotcount < 54) { dot = 6; }
			else { dot = 0; }
			break;
		}
		case 3: {
			if (dotcount < 7) { dot = 0; }
			else if (dotcount < 22) { dot = 7; }
			else if (dotcount < 37) { dot = 3; }
			else if (dotcount < 52) { dot = 8; }
			else { dot = 0; }
			break;
		}
		case 4: {
			if (dotcount < 7) { dot = 0; }
			else if (dotcount < 22) { dot = 1; }
			else if (dotcount < 37) { dot = 2; }
			else if (dotcount < 52) { dot = 1; }
			else { dot = 0; }
			break;
		}
	}
	for(i=0; i<4; i++, pdisp++) {
		*pdisp = dot_font[4*dot+i];
	}

	return;
}

void showDigit(uint8_t digit)
{
	uint8_t i, code *sptr;
	for(i=0; i<4; i++, pdisp++) {
		sptr = fptr + (4*digit+i);
		*pdisp = *sptr;
	}
}

void showMiniDigit(uint8_t digit)
{
	uint8_t i;
	for(i=0; i<3; i++, pdisp++) {
		*pdisp = num_mini[3*digit+i];
	}
}

void showNumber(uint8_t num, uint8_t clean, uint8_t dig )
{
	if(!clean&&(!dig ||((num/10) > 0 ))) {
		showDigit(num/10);
	}
	else
	{
		SPACELINE;
		SPACELINE;
		SPACELINE;
		SPACELINE;
	}
	SPACELINE;
	if(!clean) {
		showDigit(num%10);
	}
	else
	{
		SPACELINE;
		SPACELINE;
		SPACELINE;
		SPACELINE;
	}

	return;
}

void showTime(void)
{
	if( rtc.hour > 24 || rtc.min > 60 )
	{
		showDS3231();
		return;
	}
	showNumber(rtc.hour, 0, 0);
	showDot();
	showNumber(rtc.min, 0, 0);

	return;
}

void showDate(void)
{
	uint8_t i;

	if( rtc.month > 12 || rtc.date > 32 )
	{
		showDS3231();
		return;
	}
	showNumber(rtc.date, 0, 1);
	for(i=0; i<4; i++, pdisp++) {
		*pdisp = dot_font[4+i];
	}
	showNumber(rtc.month, 0, 0);

	return;
}

void showDayWeek(void)
{
	uint8_t i, code *sptr;

	switch(rtc.wday) {
		case 1: sptr = &pic_mon[0];break;
		case 2: sptr = &pic_tue[0];break;
		case 3: sptr = &pic_wed[0];break;
		case 4: sptr = &pic_thu[0];break;
		case 5: sptr = &pic_fri[0];break;
		case 6: sptr = &pic_sat[0];break;
		case 7: sptr = &pic_sun[0];break;
		default: sptr = &pic_DS3231[0];break;
	}
	for(i=0; i<DISPLAYSIZE; i++, sptr++, pdisp++) {
		*pdisp = *sptr;
	}

	return;
}

void showTemperature(void)
{
	uint8_t i;
	int8_t temp = 0;

	if( /*eep.tempsource == TS_SI &&*/ si7021SensorExists() ) {
		temp = (si7021GetTemp() / 5);
		if(temp & 1 ) temp += 1;
		temp >>= 1;
	}
	else if ( /*eep.tempsource == TS_BMP &&*/ bmxx80HaveSensor() ) {
		temp = (bmxx80GetTemp() / 5 );
		if(temp & 1 ) temp += 1;
		temp >>= 1;
	}
	else {
		temp = rtc.temp;
	}
	temp += eep.tempcoef;

	if (temp > 99) {
		for(i=0; i<4; i++, pdisp++) {
			*pdisp = 0x00;
		}
		for(i=0; i<5; i++, pdisp++) {
			*pdisp = temperature_font[50+i];
		}
		SPACELINE;
		SPACELINE;
	}
	else {
		if (temp > 9) {
			for(i=0; i<5; i++, pdisp++) {
				*pdisp = temperature_font[5*(temp/10)+i];
			}
		}
		else {
			for(i=0; i<5; i++, pdisp++) {
				*pdisp = 0x00;
			}
		}
		SPACELINE;

		for(i=0; i<5; i++, pdisp++) {
			*pdisp = temperature_font[5*(temp%10)+i];
		}
	}
	SPACELINE;
	SPACELINE;
	for(i=0; i<9; i++, pdisp++) {
		*pdisp = temperature_font[55+i];
	}

	return;
}

void showPressure(void)
{
	int8_t i;
	int16_t pres = bmxx80GetPressure();

	for (i = 0; i < 4; i++)
		buf[i] = 0;

	i=3;
	
	while (pres > 0 || i > 0) {
		buf[i] = pres % 10;
		i--;
		pres /= 10;
	}
	showDigit(buf[0]);
	SPACELINE;
	showDigit(buf[1]);
	SPACELINE;
	showDigit(buf[2]);
	SPACELINE;
#ifndef _DEBUG_
	*pdisp = 0x03;
	pdisp++;
	*pdisp = 0x03;
	pdisp++;
	SPACELINE;
	showDigit(buf[3]);
#else
	showMiniDigit(buf[3]);
	SPACELINE;
	SPACELINE;
	SPACELINE;
	SPACELINE;
/*
	switch()
	{
		case -1:
			writeToPtr(0x02);
			writeToPtr(0x1F);
			writeToPtr(0x02);
			break;
		case 0:
			SPACELINE;
			SPACELINE;
			SPACELINE;
			break;
		case 1:
			writeToPtr(0x20);
			writeToPtr(0x7C);
			writeToPtr(0x20);
			break;
	}
*/
#endif

	return;
}

void showHumidity(void)
{
	int8_t i;
	uint16_t humi = 0;
	if(si7021SensorExists()) {
		humi = si7021GetHumidity();
	}
	else if(bmxx80HaveSensor()==BME280_CHIP_ID){
		humi = bme280GetHumidity();
	}

	for (i = 0; i < 5; i++)
		buf[i] = 0;

	i=4;
	
	while (humi > 0 || i > 0) {
		buf[i] = humi % 10;
		i--;
		humi /= 10;
	}
	
	if( buf[0] == 0 ) {
		showDigit(buf[1]);
		SPACELINE;
		showDigit(buf[2]);
		SPACELINE;
		*pdisp = 0x03;
		pdisp++;
		SPACELINE;
		showDigit(buf[3]);
	}
	else {
		SPACELINE;
		showDigit(buf[0]);
		SPACELINE;
		showDigit(buf[1]);
		SPACELINE;
		showDigit(buf[2]);
		SPACELINE;
	}
	SPACELINE;
	*pdisp = 0x62;
	pdisp++;
//	*(pdisp++) = 0x62;
	*pdisp = 0x64;
	pdisp++;
	*pdisp = 0x08;
	pdisp++;
	*pdisp = 0x13;
	pdisp++;
	*pdisp = 0x23;
	pdisp++;

	return;
}

void autoBright(void)
{
	if( rtc.hour < 24 && eep.bright == 6 ) {
		displayBright = (rtc.hour&0x01)?(hourbright[rtc.hour>>1] & 0x0F):((hourbright[rtc.hour>>1]>>4 )& 0x0F);
	}

	return;
}

void showMainScreen(void)
{
	pdisp = &disp[0];
	updateFont();
	autoBright();

	switch(widgetNumber) {
		case WI_TIME: { showTime(); break;}
		case WI_DATE: { showDate(); break;}
		case WI_WEEK: { showDayWeek(); break;}
		case WI_TEMP: { showTemperature(); break;}
		case WI_PRES: { showPressure(); break;}
		case WI_HUMI: { showHumidity(); break;}
		case WI_HOLY: { showRenderBuffer(); break;}
		default: { widgetNumber = WI_TIME; showTime(); break;}
	}

	return;
}

void checkParam(int8_t *param, int8_t diff, int8_t paramMin, int8_t paramMax)
{
	*param += diff;

	if (*param > paramMax)
		*param = paramMin;
	if (*param < paramMin)
		*param = paramMax;

	return;
}

void changeMenu(int8_t diff)
{
	checkParam(&menuNumber, diff, MODE_EDIT_TIME, MODE_EDIT_TEMP_COEF);

	return;
}

void showMenu(void)
{
	uint8_t i, code *sptr;

	switch(menuNumber) {
		case MODE_EDIT_TIME: sptr = &pic_Time[0];break;
		case MODE_EDIT_DATE: sptr = &pic_Date[0];break;
		case MODE_EDIT_ALARM: sptr = &pic_Alarm[0];break;
		case MODE_EDIT_HOURSIGNAL: sptr = &pic_HourSignal[0];break;
		case MODE_EDIT_FONT: sptr = &pic_Font[0];break;
		case MODE_EDIT_DISP: sptr = &pic_Disp[0];break;
		case MODE_EDIT_DOT: sptr = &pic_Dot[0];break;
		case MODE_EDIT_BRIGHT: sptr = &pic_Bright[0];break;
		case MODE_EDIT_TIME_COEF: sptr = &pic_TimeCoef[0];break;
		case MODE_EDIT_TEMP_COEF: sptr = &pic_TempCoef[0];break;
		default:break;
	}
	for(i=0; i<DISPLAYSIZE; i++, sptr++) {
		disp[i] = *sptr;
	}

	return;
}

void showTimeEdit(void)
{
	uint8_t i;
	bit flash;

	pdisp = &disp[0];
	updateFont();

	if (refcount < 15) { flash = 0; }
	else if (refcount < 45) { flash = 1; }
	else { flash = 0; }

	if(rtc.etm == RTC_SEC) {
		for(i=0; i<9; i++, pdisp++) {
			*pdisp = 0x00;
		}
		for(i=0; i<4; i++, pdisp++) {
			*pdisp = dot_font[4*3+i];
		}
		showNumber(rtc.sec, !((rtc.etm != RTC_SEC)||(flash && (rtc.etm == RTC_SEC))), 0);
	}
	else {
		showNumber(rtc.hour, !((rtc.etm != RTC_HOUR)||(flash && (rtc.etm == RTC_HOUR))), 0);
		for(i=0; i<4; i++, pdisp++) {
			*pdisp = dot_font[4*3+i];
		}
		showNumber(rtc.min, !((rtc.etm != RTC_MIN)||(flash && (rtc.etm == RTC_MIN))), 0);
	}

	return;
}

void showDateEdit(void)
{
	uint8_t i;
	bit flash;

	pdisp = &disp[0];
	updateFont();

	if (refcount < 15) { flash = 0; }
	else if (refcount < 45) { flash = 1; }
	else { flash = 0; }

	if(rtc.etm == RTC_YEAR) {
		SPACELINE;
		SPACELINE;
		showNumber(20, 0, 0);
		SPACELINE;
		showNumber(rtc.year, !((rtc.etm != RTC_YEAR)||(flash && (rtc.etm == RTC_YEAR))), 0);
		SPACELINE;
	}
	else {
		showNumber(rtc.date, !((rtc.etm != RTC_DATE)||(flash && (rtc.etm == RTC_DATE))), 1);
		for(i=0; i<4; i++, pdisp++) {
			*pdisp = dot_font[4+i];
		}
		showNumber(rtc.month, !((rtc.etm != RTC_MONTH)||(flash && (rtc.etm == RTC_MONTH))), 0);
	}

	return;
}

void showAlarmEdit(void)
{
	uint8_t i, j, code *sptr;
	bit flash;

	pdisp = &disp[0];
	updateFont();

	if( alarm.etm == ALARM_ON) {
		if(alarm.on) {
			sptr = &pic_On[0];
		}
		else {
			sptr = &pic_Off[0];
		}
		for(i=0; i<DISPLAYSIZE; i++, sptr++) {
			disp[i] = *sptr;
		}
	}
	else if((alarm.etm == ALARM_HOUR)||(alarm.etm == ALARM_MIN)) {
		if (refcount < 15) { flash = 0; }
		else if (refcount < 45) { flash = 1; }
		else { flash = 0; }

		showNumber(alarm.hour, !((alarm.etm != ALARM_HOUR)||(flash && (alarm.etm == ALARM_HOUR))), 0);
		for(i=0; i<4; i++, pdisp++) {
			*pdisp = dot_font[4*3+i];
		}
		showNumber(alarm.min, !((alarm.etm != ALARM_MIN)||(flash && (alarm.etm == ALARM_MIN))), 0);
	}
	else {
		if (refcount < 27) { flash = 0; }
		else if (refcount < 33) { flash = 1; }
		else { flash = 0; }

		switch(alarm.etm) {
			case ALARM_MON: {sptr = &pic_alarm_mon[0]; break; }
			case ALARM_TUE: {sptr = &pic_alarm_tue[0]; break; }
			case ALARM_WED: {sptr = &pic_alarm_wed[0]; break; }
			case ALARM_THU: {sptr = &pic_alarm_thu[0]; break; }
			case ALARM_FRI: {sptr = &pic_alarm_fri[0]; break; }
			case ALARM_SAT: {sptr = &pic_alarm_sat[0]; break; }
			case ALARM_SUN: {sptr = &pic_alarm_sun[0]; break; }
			default: break;
		}

		for(i=0; i<DISPLAYSIZE; i++, sptr++) {
			j = 0;
			switch(i) {
				case 1:
				case 2: {
					if(((alarm.etm != ALARM_MON)&&alarm.mon)||((alarm.etm == ALARM_MON)&&((alarm.mon && !flash)||(!alarm.mon && flash)))) j = 1;
					break;
				}
				case 4:
				case 5: {
					if(((alarm.etm != ALARM_TUE)&&alarm.tue)||((alarm.etm == ALARM_TUE)&&((alarm.tue && !flash)||(!alarm.tue && flash)))) j = 1;
					break;
				}
				case 7:
				case 8: {
					if(((alarm.etm != ALARM_WED)&&alarm.wed)||((alarm.etm == ALARM_WED)&&((alarm.wed && !flash)||(!alarm.wed && flash)))) j = 1;
					break;
				}
				case 10:
				case 11: {
					if(((alarm.etm != ALARM_THU)&&alarm.thu)||((alarm.etm == ALARM_THU)&&((alarm.thu && !flash)||(!alarm.thu && flash)))) j = 1;
					break;
				}
				case 13:
				case 14: {
					if(((alarm.etm != ALARM_FRI)&&alarm.fri)||((alarm.etm == ALARM_FRI)&&((alarm.fri && !flash)||(!alarm.fri && flash)))) j = 1;
					break;
				}
				case 16:
				case 17: {
					if(((alarm.etm != ALARM_SAT)&&alarm.sat)||((alarm.etm == ALARM_SAT)&&((alarm.sat && !flash)||(!alarm.sat && flash)))) j = 1;
					break;
				}
				case 19:
				case 20: {
					if(((alarm.etm != ALARM_SUN)&&alarm.sun)||((alarm.etm == ALARM_SUN)&&((alarm.sun && !flash)||(!alarm.sun && flash)))) j = 1;
					break;
				}
			}
			disp[i] = *sptr | j;
		}
	}

	return;
}

void changeFont(int8_t diff)
{
	checkParam(&eep.fontMode, diff, eepMin.fontMode/*0*/, eepMax.fontMode/*4*/);

	return;
}

void showFontEdit(void)
{
	pdisp = &disp[0];
	updateFont();
	showTime();

	return;
}

void changeDisp(int8_t diff)
{
	checkParam(&eep.dispMode, diff, eepMin.dispMode/*1*/, eepMax.dispMode/*5*/);

	return;
}

void showDispEdit(void)
{
	uint8_t i;

	pdisp = &disp[0];

	for(i=0; i<16; i++, pdisp++) {
		*pdisp = pic_Type[i];
	}
	SPACELINE;

	for(i=0; i<5; i++, pdisp++) {
		*pdisp = temperature_font[5*eep.dispMode+i];
	}

	return;
}

void changeDot(int8_t diff)
{
	checkParam(&eep.dotMode, diff, eepMin.dotMode/*0*/, eepMax.dotMode/*4*/);

	return;
}

void showDotEdit(void)
{
	pdisp = &disp[0];
	updateFont();

	showTime();

	return;
}

void changeBright(int8_t diff)
{
	checkParam(&eep.bright, diff, eepMin.bright/*0*/, eepMax.bright/*6*/);
	displayBright = eep.bright;
	autoBright();

	return;
}

void showBrightEdit(void)
{
	uint8_t i;

	pdisp = &disp[0];

	for(i=0; i<16; i++, pdisp++) {
		*pdisp = pic_BrEdit[i];
	}
	SPACELINE;

	for(i=0; i<5; i++, pdisp++) {
		*pdisp = temperature_font[5*eep.bright+i];
	}

	return;
}

void changeHourSignal(int8_t diff)
{
	checkParam(&eep.hourSignal, diff, eepMin.hourSignal/*0*/, eepMax.hourSignal/*1*/);

	return;
}

void showHourSignalEdit(void)
{
	uint8_t i, code *sptr;

	if(eep.hourSignal) {
		sptr = &pic_On[0];
	}
	else {
		sptr = &pic_Off[0];
	}
	for(i=0; i<DISPLAYSIZE; i++, sptr++) {
		disp[i] = *sptr;
	}

	return;
}

void changeTimeCoef(int8_t diff)
{
	checkParam(&eep.timecoef, diff, eepMin.timecoef/*-128*/, eepMax.timecoef/*127*/);

	return;
}

void showTimeCoefEdit(void)
{
	int8_t i;
	uint8_t coef;
	bit sign;

	pdisp = &disp[0];
	sign = (eep.timecoef >= 0)? 0: 1;
	if(sign) {
		coef = -eep.timecoef;
	}
	else {
		coef = eep.timecoef;
	}
	
	for (i = 0; i < 3; i++)
		buf[i] = 0;

	i=2;
	
	while (coef > 0 || i > 0) {
		buf[i] = coef % 10;
		i--;
		coef /= 10;
	}

	coef = 3;
	if( !sign )
		coef +=5;
	if( buf[0] == 0 ) {
		coef += 5;
		if( buf[1] == 0 ) {
			coef += 5;
		}
	}

	for(i=0; i<coef; i++, pdisp++) {
		*pdisp = 0x00;
	}
	
	if( sign ) {
		for(i=0; i<4; i++, pdisp++) {
			*pdisp = 0x08;
		}

		SPACELINE;
	}

	if(buf[0] > 0 ) {
		showDigit(buf[0]);

		SPACELINE;
	}

	if((buf[0] > 0 )||(buf[1] > 0 )) {
		showDigit(buf[1]);

		SPACELINE;
	}

	showDigit(buf[2]);

	return;
}

void changeTempCoef(int8_t diff)
{
	checkParam(&eep.tempcoef, diff, eepMin.tempcoef/*-9*/, eepMax.tempcoef/*9*/);

	return;
}

void showTempCoefEdit(void)
{
	pdisp = &disp[0];

	showTemperature();

	return;
}

void wiNext(void)
{
	if( screenTime > widgets[widgetNumber].sec ) {
		widgetNumber++;
		screenTime = 0;
		if(widgetNumber > ELEMENTS(widgets)) {
			widgetNumber = WI_TIME;
		}
		if(widgetNumber == WI_PRES && !bmxx80HaveSensor()) {
			widgetNumber = WI_HUMI;
		}
		if(widgetNumber == WI_HUMI && ( !(bmxx80HaveSensor()==BME280_CHIP_ID||si7021SensorExists()) )) {
			widgetNumber = WI_HOLY;
		}
		if(widgetNumber == WI_HOLY) {
			if(holiday) {
				scroll_start = 1;
				scroll_disp_index = DISPLAYSIZE;
			}
			else {
				widgetNumber = WI_TIME;
				scroll_start = 0;
			}
		}
	}

	return;
}

void wiTime(void)
{
	if(eep.dispMode == 1) {
		screenTime = 0;
	}
	wiNext();

	return;
}

void wiHoly(void)
{
	if(scroll_start == 0){
		wiNext();
	}

	return;
}

uint8_t font_char_to_index(uint8_t chr_code)
{
	if(chr_code >= 0xA0) {
		chr_code -= 0x40;
	}
	else if(chr_code >= 0x20) {
		chr_code -= 0x20;
	}
	else {
		chr_code = 0x1F;
	}

	return chr_code;
}

void render_buff_txt(void)
{
	uint8_t r_index, r_line, chr_code, fnt_data;
	
	r_index = rndr_index;
	r_line = rndr_line;

	if(r_line >= 5)
	{
		SPACELINE;
		rndr_disp_index++;
		r_index++;
		r_line = 0;
	}
	while(rndr_disp_index < DISPLAYSIZE)
	{
		if(r_index < msg_length)
		{
			chr_code = *(msg_str + r_index);
			chr_code = font_char_to_index(chr_code);
			for(; r_line < 5; r_line++) {
				fnt_data = font_cp1251_07[5 * chr_code + r_line];
				if( fnt_data != VOID ) {
					if(rndr_disp_index < DISPLAYSIZE)
					{
						writeToPtr(fnt_data);
						rndr_disp_index++;
					}
				}
				else {
					break;
				}
			}
			r_index++;
			r_line = 0;
		}
		if(rndr_disp_index < DISPLAYSIZE)
		{
			SPACELINE;
			rndr_disp_index++;
		}
	}

	return;
}

void stopRender(void)
{
	scroll_start = 0;
	rndr_index = 0;
	rndr_line = 0;
	widgetNumber = WI_TIME;
	screenTime = 0;

	return;
}

void incRenderIndex(void)
{
	uint8_t chr_code, fnt_data;

	if(scroll_start)
	{
		if(scroll_disp_index > 0)
		{
			scroll_disp_index -= 1;
		}
		else
		{
			rndr_line++;
		}

		if(rndr_index == msg_length)
		{
			stopRender();
			return;
		}
		if(rndr_line < 5)
		{
			chr_code = *(msg_str + rndr_index);
			chr_code = font_char_to_index(chr_code);
			fnt_data = font_cp1251_07[5 * chr_code + rndr_line];
			if(fnt_data == VOID)
			{
				rndr_line = 5;
			}
		}
		if(rndr_line > 5)
		{
			rndr_index++;
			rndr_line = 0;
			if(rndr_index == msg_length)
			{
				stopRender();
				return;
			}
		}
	}

	return;
}

void showRenderBuffer(void)
{
	rndr_disp_index = 0;
	
	if(scroll_start == 0)
	{
		stopRender();
		return;
	}

	if(scroll_disp_index > 0)
	{
		while(rndr_disp_index < scroll_disp_index)
		{
			SPACELINE;
			rndr_disp_index++;
		}
		if(rndr_disp_index < DISPLAYSIZE)
		{
				render_buff_txt();
		}
	}
	else
	{
		render_buff_txt();
	}

	return;
}

void setRenderString(uint8_t length, char *str)
{
	msg_length = length - 1;
	msg_str = str;

	return;
}
/*
static char *mkNumberString(int16_t value, uint8_t width, uint8_t lead)
{
	static char strbuf[8];

	uint8_t sign = lead;
	int8_t pos;

	if (value < 0) {
		sign = '-';
		value = -value;
	}

	// Clear buffer and go to it's tail
	for (pos = 0; pos < width; pos++)
		strbuf[pos] = lead;
	strbuf[pos--] = '\0';

	// Fill buffer from right to left
	while (value > 0 || pos > width - 2) {
		strbuf[pos] = value % 10 + 0x30;
		pos--;
		value /= 10;
	}

	if (pos >= 0)
		strbuf[pos] = sign;

	return strbuf;
}
*/
