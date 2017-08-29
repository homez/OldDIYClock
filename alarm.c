#include "sys.h"
#include "alarm.h"
#include "settings.h"

Alarm_type alarm;
static code Alarm_type alarmMin = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ALARM_NOEDIT};
static code Alarm_type alarmMax = {1, 23, 59, 1, 1, 1, 1, 1, 1, 1, ALARM_NOEDIT};

void alarmInit(void)
{
	int8_t i;
	for(i=0; i<ALARM_ETM; i++) {
		*(((int8_t*)&alarm.on) + i) = *(((int8_t*)&eep.on) + i);
	}

	alarm.etm = ALARM_NOEDIT;

	return;
}

void alarmSave(void)
{
	int8_t i;
	for(i=0; i<ALARM_ETM; i++) {
		*(((int8_t*)&eep.on) + i) = *(((int8_t*)&alarm.on) + i);
	}

	settingsSave();

	alarm.etm = ALARM_NOEDIT;

	return;
}

void alarmNextEditParam(void)
{
	if (alarm.etm >= ALARM_SUN)
		alarm.etm = ALARM_ON;
	else
		alarm.etm++;

	return;
}

void alarmChange(int8_t diff)
{
	int8_t *alrm = (int8_t*)&alarm + alarm.etm;
	int8_t alrmMax = *((int8_t*)&alarmMax + alarm.etm);
	int8_t alrmMin = *((int8_t*)&alarmMin + alarm.etm);

	*alrm += diff;

	if (*alrm > alrmMax)
		*alrm = alrmMin;
	if (*alrm < alrmMin)
		*alrm = alrmMax;

	return;
}
