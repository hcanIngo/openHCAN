#include <canix/mem_check.h>
#include <avr/io.h>

#define MASK 0xaa

extern uint8_t __heap_start;

uint16_t get_mem_unused(void)
{
	uint16_t unused = 0;
	uint8_t *p = &__heap_start;

	do
	{
		if (*p++ != MASK)
			break;

		unused++;

	} while (p <= (uint8_t*) RAMEND);

	return unused;
}

// http://www.mikrocontroller.net/topic/83833
// WinAVRs haben ohne diesen Prototypen Probleme
void __init8_mem(void) __attribute__ ((naked, section (".init8")));

void __init8_mem(void) 
{
	__asm volatile (
			"ldi r30, lo8 (__heap_start)" "\n\t"
			"ldi r31, hi8 (__heap_start)" "\n\t"
			"ldi r24, %0" "\n\t"
			"ldi r25, hi8(%1)" "\n"
			"0:" "\n\t"
			"st Z+,r24" "\n\t"
			"cpi r30, lo8(%1)" "\n\t"
			"cpc r31, r25" "\n\t"
			"brlo 0b"
			:
			: "i" (MASK), "i" (RAMEND+1)
			);
}

