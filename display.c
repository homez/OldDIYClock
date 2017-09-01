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

uint8_t data disp[DISPLAYSIZE];
uint8_t render_buffer_size = 0;
int16_t scroll_index = -1;
uint8_t xdata render_buffer[RENDSERBUFFERSIZE];
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
	pdisp = &render_buffer[0];

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
		if( columnrefnum > 21 ) {
			columnrefnum = 0;
			reversed = key_mer;
			if(refstart==0) {
				refstart = 1;
			}
		}
	}

	return;
}

void resetDispLoop(void)
{
	dispMode = MODE_MAIN;
	screenTime = 0;
	widgetNumber = 0;

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

void showNumber(uint8_t num, uint8_t clean, uint8_t dig )
{
	uint8_t i, code *sptr;

	for(i=0; i<4; i++, pdisp++) {
			if(!clean&&(!dig ||((num/10) > 0 ))) {
			sptr = fptr + (4*(num/10)+i);
			*pdisp = *sptr;
		}
		else
			*pdisp = 0x00;
	}
	*pdisp = 0x00;
	pdisp++;
	for(i=0; i<4; i++, pdisp++) {
		if(!clean) {
			sptr = fptr + (4*(num%10)+i);
			*pdisp = *sptr;
		}
		else
			*pdisp = 0x00;
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
	int8_t temp = eep.tempcoef;
	temp += rtc.temp;

	if(si7021SensorExists() ) {
		temp = (si7021GetTemp() / 5);
		if(temp & 1 ) temp += 1;
		temp >>= 1;
	}
	else if(bmxx80HaveSensor() ) {
		temp = (bmxx80GetTemp() / 5 );
		if(temp & 1 ) temp += 1;
		temp >>= 1;
	}

	if (temp > 99) {
		for(i=0; i<4; i++, pdisp++) {
			*pdisp = 0x00;
		}
		for(i=0; i<5; i++, pdisp++) {
			*pdisp = temperature_font[50+i];
		}
		*pdisp = 0x00;
		pdisp++;
		*pdisp = 0x00;
		pdisp++;
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
		*pdisp = 0x00;
		pdisp++;

		for(i=0; i<5; i++, pdisp++) {
			*pdisp = temperature_font[5*(temp%10)+i];
		}
	}
	*pdisp = 0x00;
	pdisp++;
	*pdisp = 0x00;
	pdisp++;
	for(i=0; i<9; i++, pdisp++) {
		*pdisp = temperature_font[55+i];
	}

	return;
}

void showPressure(void)
{
	static uint8_t buf[4];
	uint8_t code *sptr;
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

	for(i=0; i<4; i++, pdisp++) {
		sptr = fptr + (4*buf[0]+i);
		*pdisp = *sptr;
	}
	*pdisp = 0x00;
	pdisp++;
	for(i=0; i<4; i++, pdisp++) {
		sptr = fptr + (4*buf[1]+i);
		*pdisp = *sptr;
	}
	*pdisp = 0x00;
	pdisp++;
	for(i=0; i<4; i++, pdisp++) {
		sptr = fptr + (4*buf[2]+i);
		*pdisp = *sptr;
	}
	*pdisp = 0x00;
	pdisp++;
	*pdisp = 0x03;
	pdisp++;
	*pdisp = 0x03;
	pdisp++;
	*pdisp = 0x00;
	pdisp++;
	for(i=0; i<4; i++, pdisp++) {
		sptr = fptr + (4*buf[3]+i);
		*pdisp = *sptr;
	}

	return;
}

void showHumidity(void)
{
	static uint8_t buf[5];
	uint8_t code *sptr;
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
		for(i=0; i<4; i++, pdisp++) {
			sptr = fptr + (4*buf[1]+i);
			*pdisp = *sptr;
		}
		*pdisp = 0x00;
		pdisp++;
		for(i=0; i<4; i++, pdisp++) {
			sptr = fptr + (4*buf[2]+i);
			*pdisp = *sptr;
		}
		*pdisp = 0x00;
		pdisp++;
		*pdisp = 0x03;
		pdisp++;
		*pdisp = 0x00;
		pdisp++;
		for(i=0; i<4; i++, pdisp++) {
			sptr = fptr + (4*buf[3]+i);
			*pdisp = *sptr;
		}
	}
	else {
		*pdisp = 0x00;
		pdisp++;
		for(i=0; i<4; i++, pdisp++) {
			sptr = fptr + (4*buf[0]+i);
			*pdisp = *sptr;
		}
		*pdisp = 0x00;
		pdisp++;
		for(i=0; i<4; i++, pdisp++) {
			sptr = fptr + (4*buf[1]+i);
			*pdisp = *sptr;
		}
		*pdisp = 0x00;
		pdisp++;
		for(i=0; i<4; i++, pdisp++) {
			sptr = fptr + (4*buf[2]+i);
			*pdisp = *sptr;
		}
		*pdisp = 0x00;
		pdisp++;
	}
	*pdisp = 0x00;
	pdisp++;
	*pdisp = 0x62;
	pdisp++;
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
		default: { showTime(); break;}
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
		*pdisp = 0x00;
		pdisp++;
		*pdisp = 0x00;
		pdisp++;
		showNumber(20, 0, 0);
		*pdisp = 0x00;
		pdisp++;
		showNumber(rtc.year, !((rtc.etm != RTC_YEAR)||(flash && (rtc.etm == RTC_YEAR))), 0);
		*pdisp = 0x00;
		pdisp++;
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
	*pdisp = 0x00;
	pdisp++;

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
	*pdisp = 0x00;
	pdisp++;

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
	static uint8_t buf[3];
	uint8_t code *sptr;
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

		*pdisp = 0x00;
		pdisp++;
	}

	if(buf[0] > 0 ) {
		for(i=0; i<4; i++, pdisp++) {
			sptr = fptr + (4*buf[0]+i);
			*pdisp = *sptr;
		}

		*pdisp = 0x00;
		pdisp++;
	}

	if((buf[0] > 0 )||(buf[1] > 0 )) {
		for(i=0; i<4; i++, pdisp++) {
			sptr = fptr + (4*buf[1]+i);
			*pdisp = *sptr;
		}

		*pdisp = 0x00;
		pdisp++;
	}

	for(i=0; i<4; i++, pdisp++) {
		sptr = fptr + (4*buf[2]+i);
		*pdisp = *sptr;
	}

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

void showRenderBuffer(void)
{
	uint8_t i;

	int16_t ind = scroll_index - DISPLAYSIZE;
	if( scroll_index > (render_buffer_size + DISPLAYSIZE )) {
		scroll_index = -1;
		widgetNumber = 0; screenTime = 0;
	}

	for(i=0; i<DISPLAYSIZE; i++) {
		if(( ind + i >= 0 )&&(ind + i < render_buffer_size )) {
			disp[i] = render_buffer[(uint8_t)(ind + i)];
		}
		else {
			disp[i] = 0x00;
		}
	}

	return;
}

void writeRenderBuffer(uint8_t value)
{
	if ( render_buffer_size < RENDSERBUFFERSIZE) {
		render_buffer[render_buffer_size++] = value;
	}

	return;
}

void renderHoliday(uint8_t length, char *str)
{
	uint8_t i, j, t, c;

	render_buffer_size = 0;

	for(i=0; i < (length - 1); i++, str++) {
		c = *str;
		if( c >= 0xA0 ) {
			c -= 0x40;
		}
		else if( c >= 0x20 ) {
			c -= 0x20;
		}
		else {
			c = 0x1F;
		}
		for(j=0; j<5; j++) {
			t = font_cp1251_07[5*c+j];
			if( t != VOID ) {
				writeRenderBuffer(t);
			}
		}
		writeRenderBuffer(0x00);
	}

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

#ifdef _DEBUG_

void showTestRender(void)
{
	/*
	 э = \xFD
	*/
	//char test[] = "TEST RENDER!!! Proverka???\x63";
	//char test[] = "С Новым Годом!!!";
	//char test[] = "ШЩЪЫЬЭЮЯшщъыь\xFDюя";
	char code test[] = "Проверка рендеринга, на длину строки.";
	uint8_t i,j,t;
	uint8_t c;

	scroll_index = 0;
	for(i=0; i < strlen(test); i++) {
		c = test[i];
		if( c >= 0xA0 ) {
			c -= 0x40;
		}
		else if( c >= 0x20 ) {
			c -= 0x20;
		}
		else {
			c = 0x1F;
		}
		for(j=0; j<5; j++) {
			t = font_cp1251_07[5*c+j];
			if( t != VOID ) {
				writeRenderBuffer(t);
			}
		}
		writeRenderBuffer(0x00);
	}

	return;
}

#endif
