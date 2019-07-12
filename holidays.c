#include "holidays.h"
#include "sys.h"
#include "rtc.h"
#include "display.h"

	/*
	 � = \xFD
	*/
	
bit holiday = 0; 

code char hd_01_01[] = "� ����� �����!!!";
code char hd_01_07[] = "��������� ��������.";
code char hd_01_14[] = "������ ����� ���.";
code char hd_01_19[] = "�������� ��������.";
code char hd_01_25[] = "�������� ����.";
code char hd_02_14[] = "���� ���� ���������.";
code char hd_02_23[] = "���� ��������� ���������.";
code char hd_03_08[] = "������������� ������� ����.";
code char hd_03_19[] = "���� ������-����������.";
code char hd_04_01[] = "���� �����.";
code char hd_04_12[] = "���� ������������.";
code char hd_05_01[] = "�������� ����� � �����.";
code char hd_05_09[] = "���� ������!!!";
code char hd_05_19[] = "���� ��������.";
code char hd_05_28[] = "���� ������������.";
code char hd_06_01[] = "���� ������ �����.";
code char hd_06_12[] = "���� ������.";
code char hd_06_27[] = "���� ��������.";
code char hd_07_08[] = "���� �����, ����� � ��������.";
code char hd_08_02[] = "���� ��������-��������� �����.";
code char hd_08_12[] = "���� ������-��������� ���.";
code char hd_09_01[] = "���� ������.";
code char hd_09_09[] = "���� ������������.";
code char hd_10_05[] = "���� �������.";
code char hd_11_18[] = "���� �������� ���� ������.";
code char hd_12_22[] = "���� \xFD���������.";

code char hd_256_day[] = "���� ������������.";
code char hd_06_medicalday[] = "���� ������������ ���������.";
code char hd_07_lastfriday[] = "���� ���������� ��������������.";
code char hd_07_lastsunday[] = "���� ������-�������� �����.";
code char hd_08_beerday[] = "������������� ���� ����.";
code char hd_08_builderday[] = "���� ���������.";
code char hd_09_tankmanday[] = "���� ��������.";
code char hd_10_lastsunday[] = "���� �������������.";
code char hd_11_lastsunday[] = "���� ������.";

HOLIDAY code holidays[] = {
		hAN( 1, 1, hd_01_01 ),
		hAN( 1, 7, hd_01_07 ),
		hAN( 1, 14, hd_01_14 ),
		hAN( 1, 19, hd_01_19 ),
		hAN( 1, 25, hd_01_25 ),
		hAN( 2, 14, hd_02_14 ),
		hAN( 2, 23, hd_02_23 ),
		hAN( 3, 8, hd_03_08 ),
		hAN( 3, 19, hd_03_19 ),
		hAN( 4, 1, hd_04_01 ),
		hAN( 4, 12, hd_04_12 ),
		hAN( 5, 1, hd_05_01 ),
		hAN( 5, 9, hd_05_09 ),
		hAN( 5, 19, hd_05_19 ),
		hAN( 5, 28, hd_05_28 ),
		hAN( 6, 1, hd_06_01 ),
		hAN( 6, 12, hd_06_12 ),
		hAN( 6, 27, hd_06_27 ),
		hAN( 7, 8, hd_07_08 ),
		hAN( 8, 2, hd_08_02 ),
		hAN( 8, 12, hd_08_12 ),
		hAN( 9, 1, hd_09_01 ),
		hAN( 9, 9, hd_09_09 ),
		hAN( 10, 5, hd_10_05 ),
		hAN( 11, 18, hd_11_18 ),
		hAN( 12, 22, hd_12_22 ),
		hADN( 256, hd_256_day ),
		hAWN( 6, W3, SUN, hd_06_medicalday ),
		hAWL( 7, FRY, hd_07_lastfriday ),
		hAWL( 7, SUN, hd_07_lastsunday ),
		hAWN( 8, W1, FRY, hd_08_beerday ),
		hAWN( 8, W2, SUN, hd_08_builderday ),
		hAWN( 9, W2, SUN, hd_09_tankmanday ),
		hAWL( 10, SUN, hd_10_lastsunday ),
		hAWL( 11, SUN, hd_11_lastsunday ),
};

uint8_t wdays[5];

void checkWeekDays(void)
{
	uint8_t i, t;
	int8_t w = rtc.wday;

	for( i = 0; i < 5; i++ ) {
		wdays[i] = 0;
	}
	w = rtc.date;
	while( w > 7 ) {
		w -=7;
	}
	t = rtcDaysInMonth();
	for( i = 0; (( i < 5 ) && ( w <= t )); i++, w+=7 ) {
		wdays[i] = w;
	}

	return;
}

bit checkLastWeek(void)
{
	if( wdays[W5] == 0 ) {
		if( wdays[W4] == rtc.date )
			return 1;
	}
	else {
		if( wdays[W5] == rtc.date )
			return 1;
	}

	return 0;
}

bit checkWeekDay(uint8_t weekly)
{
		return wdays[weekly] == rtc.date;
}

void checkHolidays(void)
{
	uint8_t i;

	holiday = 0;

	checkWeekDays();

	for(i=0; (i < ELEMENTS(holidays)) && (holiday == 0); i++) {
		switch(( holidays[i].month & 0xF0) >> 4) {
			case 0: {
				if((holidays[i].day == rtc.date) && ((holidays[i].month & 0x0F) == rtc.month)) {
					holiday = 1;
					setRenderString(holidays[i].length, holidays[i].ptr);
					break;
				}
				break;
			}
			case 1:
			case 2:
			case 3:
			case 4:
			case 5: {
				if(((holidays[i].month & 0x0F) == rtc.month)&&(rtc.wday == holidays[i].day)&&checkWeekDay((( holidays[i].month & 0x70) >> 4)-1)) {
					holiday = 1;
					setRenderString(holidays[i].length, holidays[i].ptr);
				}
				break;
			}
			case 6: {
				if(rtcYearDay()==((((uint16_t)holidays[i].month & 0x01) << 8) | holidays[i].day)) {
					holiday = 1;
					setRenderString(holidays[i].length, holidays[i].ptr);
				}
				break;
			}
			case 7: {
				if(((holidays[i].month & 0x0F) == rtc.month)&&(rtc.wday == holidays[i].day)&&checkLastWeek()) {
					holiday = 1;
					setRenderString(holidays[i].length, holidays[i].ptr);
				}
				break;
			}
			default: {
				holiday = 0;
			}
		}
	}

	return;
}