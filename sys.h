#ifndef _SYS_H_
#define _SYS_H_

#include "STC15.H"

#define SYSCLK_18 18432
#define SYSCLK_11 11059
#define SYSCLK SYSCLK_18

#define int8_t signed char
#define uint8_t unsigned char
#define int16_t signed int
#define uint16_t unsigned int
#define int32_t signed long
#define uint32_t unsigned long

#define ELEMENTS(a) sizeof(a) / sizeof(a[0])
	
#endif /* _SYS_H_ */
