#include "delay.h"
#include "sys.h"
#include "intrins.h"

void delay_1us(void)
{
    uint8_t i = DELAY_1us;
		while (--i);
}

void delay_5us(uint8_t us)
{
    uint8_t i;
    for (; us; us--)
        for (i=DELAY_5us; i; i--) {};
}

void delay_ms(uint8_t ms)
{
    uint8_t i, j;
    for (; ms; ms--) {
			DELAY_1ms;
			do {
				while (--j);
			} while (--i);
		}
 }