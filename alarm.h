#ifndef _ALARM_H_
#define _ALARM_H_

#include "sys.h"

enum {
	ALARM_ON = 0,
	ALARM_HOUR,
	ALARM_MIN,
	ALARM_MON,
	ALARM_TUE,
	ALARM_WED,
	ALARM_THU,
	ALARM_FRI,
	ALARM_SAT,
	ALARM_SUN,
	ALARM_ETM
};

typedef struct {
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
	int8_t etm;
} Alarm_type;

extern Alarm_type alarm;

#define ALARM_NOEDIT			0xFF

void alarmInit(void);
void alarmSave(void);
void alarmNextEditParam(void);
void alarmChange(int8_t diff);
uint8_t alarmRawWeekday(void);

#endif /* _ALARM_H_ */
