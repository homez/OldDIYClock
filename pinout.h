#ifndef _PINOUT_H_
#define _PINOUT_H_

#include "STC15.H"

sbit row1 = P3^0;	//
sbit row2 = P3^1;	//
sbit row3 = P3^2;	//
sbit row4 = P3^3;	//
sbit row5 = P3^4;	//
sbit row6 = P3^5;	//
sbit row7 = P3^6;	//

sbit col1 = P1^0;	//
sbit col2 = P1^1;	//
sbit col3 = P1^2;	//
sbit col4 = P1^3;	//
sbit col5 = P1^4;	//
sbit col6 = P1^5;	//
sbit col7 = P1^6;	//
sbit col8 = P1^7;	//
sbit col9 = P2^0;	//
sbit col10 = P2^1;	//
sbit col11 = P2^2;	//
sbit col12 = P2^3;	//
sbit col13 = P2^4;	//
sbit col14 = P2^5;	//
sbit col15 = P2^6;	//
sbit col16 = P2^7;	//
sbit col17 = P0^0;	//
sbit col18 = P0^1;	//
sbit col19 = P0^2;	//
sbit col20 = P0^3;	//
sbit col21 = P0^4;	//
sbit col22 = P0^5;	//

sbit sda = P4^2;	// I2C SDA
sbit scl = P4^1;	// I2C SCL
sbit swq = P4^0;	// DS3231 SWQ

sbit key_mer = P4^7;	// 

sbit key_set = P4^6;	// set key
sbit key_inc = P4^5;	//  +  key
sbit key_dec = P4^4;	//  -  key

sbit onewire = P5^4; // ds18b20

sbit beep = P4^3;	// beeper

#endif /* _PINOUT_H_ */
