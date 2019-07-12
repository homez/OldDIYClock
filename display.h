#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include<intrins.h>
#include "sys.h"

#define PARAM_UP				1
#define PARAM_DOWN				-1
#define SPACELINE 		writeToPtr(0x00);
#define SCROLLDIV 		2


enum {
	MODE_MAIN = 0,
	MODE_MENU,
	MODE_EDIT_TIME,
	MODE_EDIT_DATE,
	MODE_EDIT_ALARM,
	MODE_EDIT_HOURSIGNAL,
	MODE_EDIT_FONT,
	MODE_EDIT_DISP,
	MODE_EDIT_DOT,
	MODE_EDIT_BRIGHT,
	MODE_EDIT_TIME_COEF,
	MODE_EDIT_TEMP_COEF,
	MODE_TEST,

	MODE_END
};

enum {
	BACK = 0,
	SAVEANDBACK,
	CANCELANDBACK,
};

enum {
	WI_TIME = 0,
	WI_DATE,
	WI_WEEK,
	WI_TEMP,
	WI_PRES,
	WI_HUMI,
	WI_HOLY
};

enum {
	TS_DS = 0,
	TS_SI,
	TS_BMP
};

typedef void __wi_func(void); 
typedef __wi_func *__ptr_wi_func;

typedef struct {
	uint8_t sec;
	__wi_func code *func;
} Widget;

#define DISPLAYSIZE 22
#define hbd(a,b) a | (b << 4)

extern uint8_t dispMode;
extern uint8_t data disp[DISPLAYSIZE];
extern uint8_t displayBright;
extern uint8_t menuNumber;
extern uint8_t screenTime;
extern uint8_t widgetNumber;
extern bit refstart;
extern uint8_t refcount;
extern uint8_t dotcount;
extern bit reversed;
extern Widget code widgets[7];

void displayInit(void);
void displayClear(void);
void displayRefresh(void);
void backToMainMode(uint8_t mode);
void checkAlarm(void);
void updateFont(void);
void showMainScreen(void);
void showTime(void);
void changeBright(int8_t diff);
void changeMenu(uint8_t diff);
void showMenu(void);
void showTimeEdit(void);
void showDateEdit(void);
void showAlarmEdit(void);
void changeFont(int8_t diff);
void showFontEdit(void);
void changeDisp(int8_t diff);
void showDispEdit(void);
void changeDot(int8_t diff);
void showDotEdit(void);
void showBrightEdit(void);
void changeHourSignal(int8_t diff);
void showHourSignalEdit(void);
void changeTimeCoef(int8_t diff);
void showTimeCoefEdit(void);
void changeTempCoef(int8_t diff);
void showTempCoefEdit(void);
void showRenderBuffer(void);
void incRenderIndex(void);
void setRenderString(uint8_t length, char *str);

#endif /* _DISPLAY_H_ */