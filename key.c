#include "pinout.h"
#include "sys.h"
#include "key.h"

uint8_t sensTimer = 0;
uint8_t scrollTimer = 0;
uint16_t alarmTimer = 0;
static volatile uint8_t beepTimer = 0;
static volatile uint8_t secTimer = TIME_SEC;
static volatile uint8_t cmdBuf = BTN_STATE_0;

void CheckBtn(void)
{
	static int16_t btnCnt = 0;						// Buttons press duration value
	static uint8_t btnPrev = 0;						// Previous buttons state

	uint8_t btnNow = BTN_STATE_0;

	if (!key_set)
		btnNow |= BTN_0;
	if (!key_inc)
		btnNow |= BTN_1;
	if (!key_dec)
		btnNow |= BTN_2;

	// If button event has happened, place it to command buffer
	if (btnNow) {
		if (btnNow == btnPrev) {
			btnCnt++;
			if (btnCnt == LONG_PRESS)
				cmdBuf = (btnPrev << 4);
		}
		else {
			btnPrev = btnNow;
		}
	}
	else {
		if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS))
			cmdBuf = btnPrev;
		btnCnt = 0;
	}

	if (secTimer) {
		secTimer--;
	}
	else {
		secTimer = TIME_SEC;
		if (sensTimer)
			sensTimer--;
		if (scrollTimer)
			scrollTimer--;

		if (alarmTimer)
			alarmTimer--;
	}

	if (beepTimer) {
		beepTimer--;
	}
	else {
		if (alarmTimer)
			beepTimer = TIME_SEC;
	}

	if ((beepTimer & 0x0E) > 8) {
		if (secTimer > TIME_SEC / 2)
			beep=0;
	}
	else {
		beep=1;
	}

	return;
}

void startBeeper(uint8_t time)
{
	beepTimer = time;
	secTimer = TIME_SEC;
	alarmTimer = 0;

	return;
}

uint8_t getBtnCmd(void)
{
	uint8_t ret = cmdBuf;
	cmdBuf = BTN_STATE_0;

	return ret;
}
