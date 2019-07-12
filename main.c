#include "sys.h"
#include "pinout.h"
#include "timer.h"
#include "display.h"
#include "rtc.h"
#include "key.h"
#include "alarm.h"
#include "settings.h"
#include "holidays.h"
#include "delay.h"
#include "i2c.h"

#include "bmxx80.h"
#include "si7021.h"

void hwInit(void)
{
	settingsInit();

	displayInit();

	timerInit();

	rtcInit();

	alarmInit();

	bmxx80Init();

	si7021Init();

	rtc.etm = RTC_NOEDIT;

	return;
}

void main(void)
{
	uint8_t cmd;
	uint8_t direction = PARAM_UP;

	hwInit();
	startBeeper(BEEP_SHORT);

	sensTimer = TEMP_MEASURE_TIME;

	while(1) {
		while(refstart == 0) {}
		refstart = 0;

		if(((refcount % 10) == 0) &&(dispMode == MODE_MAIN||!(dispMode == MODE_EDIT_TIME||dispMode == MODE_EDIT_DATE))) {
			if (sensTimer == 0) {
				sensTimer = SENSOR_POLL_INTERVAL;

				if (bmxx80HaveSensor()) bmxx80Convert();
				if (si7021SensorExists()) si7021Convert();
			}
			checkAlarm();
			if(widgetNumber != WI_HOLY)
			{
				checkHolidays();
			}
		}

		cmd = getBtnCmd();
		if (cmd != BTN_STATE_0) {
			if (cmd < BTN_0_LONG)
				startBeeper(BEEP_SHORT);
			else
				startBeeper(BEEP_LONG);
		}

		/*
		BTN_0 - 'SET'
		BTN_1 -  '+'
		BTN_2 -  '-'
		*/
		switch (cmd) {
			case BTN_0: {
				switch (dispMode) {
					case MODE_MAIN: { break; }
					case MODE_MENU: {
						switch(menuNumber) {
							case MODE_EDIT_TIME: {rtc.etm = RTC_HOUR; dispMode = menuNumber; break;}
							case MODE_EDIT_DATE: {rtc.etm = RTC_YEAR; dispMode = menuNumber; break;}
							case MODE_EDIT_ALARM: {alarm.etm = ALARM_ON; dispMode = menuNumber; break;}
							case MODE_EDIT_HOURSIGNAL:
							case MODE_EDIT_FONT:
							case MODE_EDIT_DISP:
							case MODE_EDIT_DOT:
							case MODE_EDIT_BRIGHT:
							case MODE_EDIT_TEMP_COEF:
							case MODE_EDIT_TIME_COEF: {
								dispMode = menuNumber;
								break;
							}
							default:
								break;
						}
						break;
					}
					case MODE_EDIT_TIME: {
						if(rtc.etm == RTC_SEC) {
							rtcSaveTime();
							backToMainMode(BACK);
						}
						else {
							rtcNextEditParam();
						}
						break;
					}
					case MODE_EDIT_DATE: {
						if(rtc.etm == RTC_DATE) {
							rtcSaveDate();
							backToMainMode(BACK);
						}
						else {
							rtcNextEditParam();
						}
						break;
					}
					case MODE_EDIT_ALARM: {
						if((alarm.etm == ALARM_ON && !alarm.on)||(alarm.etm == ALARM_SUN)) {
							alarmSave();
							backToMainMode(SAVEANDBACK);
						}
						else {
							alarmNextEditParam();
						}
						break;
					}
					case MODE_EDIT_TIME_COEF: {
						rtcSavePPM();
					}
					case MODE_EDIT_HOURSIGNAL:
					case MODE_EDIT_FONT:
					case MODE_EDIT_DISP:
					case MODE_EDIT_DOT:
					case MODE_EDIT_BRIGHT:
					case MODE_EDIT_TEMP_COEF: {
						backToMainMode(SAVEANDBACK);
						break;
					}
				}
				break;
			}
			case BTN_1: {
				direction = PARAM_UP;
			}
			case BTN_2: {
				if (cmd == BTN_2)
					direction = PARAM_DOWN;
				switch (dispMode) {
					case MODE_MAIN: { changeBright(direction); break; }
					case MODE_MENU: { changeMenu(direction); break; }
					case MODE_EDIT_TIME:
					case MODE_EDIT_DATE: { rtcChangeTime(direction); break; }
					case MODE_EDIT_ALARM: { alarmChange(direction); break; }
					case MODE_EDIT_HOURSIGNAL: { changeHourSignal(direction); break; }
					case MODE_EDIT_FONT: { changeFont(direction); break; }
					case MODE_EDIT_DISP: { changeDisp(direction); break; }
					case MODE_EDIT_DOT: { changeDot(direction); break; }
					case MODE_EDIT_BRIGHT: { changeBright(direction); break; }
					case MODE_EDIT_TIME_COEF: { changeTimeCoef(direction); break; }
					case MODE_EDIT_TEMP_COEF: { changeTempCoef(direction); break; }
				}
				break;
			}
			case BTN_0_LONG: {
				switch (dispMode) {
					case MODE_MAIN: { dispMode = MODE_MENU; /*menuNumber = MODE_EDIT_TIME;*/ break; }
					case MODE_MENU: { dispMode = MODE_MAIN; break; }
					case MODE_EDIT_ALARM: {
						alarmInit();
					}
					case MODE_EDIT_HOURSIGNAL:
					case MODE_EDIT_FONT:
					case MODE_EDIT_DISP:
					case MODE_EDIT_DOT:
					case MODE_EDIT_BRIGHT:
					case MODE_EDIT_TEMP_COEF:
					case MODE_EDIT_TIME_COEF:	{
						backToMainMode(CANCELANDBACK);
						break;
					}
					case MODE_EDIT_TIME:
					case MODE_EDIT_DATE: {
						backToMainMode(BACK);
						break;
					}
				}
				break;
			}
			case BTN_1_LONG: {
				break;
			}
			case BTN_2_LONG: {
				break;
			}
			case BTN_0_LONG | BTN_1_LONG: {
				break;
			}
			case BTN_0_LONG | BTN_2_LONG: {
				break;
			}
			case BTN_1_LONG | BTN_2_LONG: {
				switch (dispMode) {
					case MODE_EDIT_TEMP_COEF: {
						eep.tempcoef = 0;
					}
				}
				break;
			}
			case BTN_0_LONG | BTN_1_LONG | BTN_2_LONG: {
				break;
			}
		}

		switch(dispMode) {
			case MODE_MAIN: { showMainScreen(); break; }
			case MODE_MENU: { showMenu(); break; }
			case MODE_EDIT_TIME: { showTimeEdit(); break; } 
			case MODE_EDIT_DATE: { showDateEdit(); break; }
			case MODE_EDIT_ALARM: { showAlarmEdit(); break; }
			case MODE_EDIT_HOURSIGNAL: { showHourSignalEdit(); break; }
			case MODE_EDIT_FONT: { showFontEdit(); break; }
			case MODE_EDIT_DISP: { showDispEdit(); break; }
			case MODE_EDIT_DOT: { showDotEdit(); break; }
			case MODE_EDIT_BRIGHT: { showBrightEdit(); break; }
			case MODE_EDIT_TIME_COEF: { showTimeCoefEdit(); break; }
			case MODE_EDIT_TEMP_COEF: { showTempCoefEdit(); break; }
		}





		dotcount++;
		refcount++;
		if(holiday&&(widgetNumber == WI_HOLY) && (refcount % SCROLLDIV) == 0) {
			incRenderIndex();
		}
		if(dotcount > 59) dotcount = 0;
		if(refcount > 59) {
			refcount = 0;
			screenTime++;
			((__ptr_wi_func)widgets[widgetNumber].func)();
		}
	}
	
	return;
}

