#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "sys.h"
#include "alarm.h"

enum {
	EEP_PAGEBLOCK = 0,
	EEP_HOURSIGNAL,
	EEP_DISPMODE,
	EEP_DOTMODE,
	EEP_FONTMODE,
	EEP_ALARMTIMEOUT,
	EEP_BRIGHT,
	EEP_ALARM_ON,
	EEP_ALARM_HOUR,
	EEP_ALARM_MIN,
	EEP_ALARM_MON,
	EEP_ALARM_TUE,
	EEP_ALARM_WED,
	EEP_ALARM_THU,
	EEP_ALARM_FRI,
	EEP_ALARM_SAT,
	EEP_ALARM_SUN,
	EEP_TEMP_COEF,
	EEP_TIME_COEF,
};

typedef struct {
	uint8_t pageBlock;
	uint8_t hourSignal;
	uint8_t dispMode;
	uint8_t dotMode;
	uint8_t fontMode;
	uint8_t alarmTimeout;
	uint8_t bright;

	//Alarm_type alarm;
	int8_t on;
	int8_t hour;
	int8_t min;
	int8_t mon;
	int8_t tue;
	int8_t wed;
	int8_t thu;
	int8_t fri;
	int8_t sat;
	int8_t sun;
	
	int8_t tempcoef;
	int8_t timecoef;
} EEP_Param;

extern EEP_Param eep;
extern code EEP_Param eepMin;
extern code EEP_Param eepMax;

void settingsInit(void);
void settingsSave(void);

#endif /* _SETTINGS_H_ */