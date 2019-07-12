#ifndef _SYS_H_
#define _SYS_H_

#include "STC15.H"

#define SYSCLK_18 18432
#define SYSCLK_11 11059
#define SYSCLK SYSCLK_18

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

typedef signed char int8_t;
typedef short int16_t;
typedef long int32_t;

#define ELEMENTS(a) sizeof(a) / sizeof(a[0])
	
#endif /* _SYS_H_ */
