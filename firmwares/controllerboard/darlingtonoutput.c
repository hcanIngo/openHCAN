#include "../controllerboard/darlingtonoutput.h"

#include <avr/io.h>
#include <canix/syslog.h>
#include "../controllerboard/devices/ports.h"

void darlingtonoutput_init(void)
{
#if defined(__AVR_ATmega32__) || defined(__AVR_ATmega644P__)
	// Darlington Ports auf Output setzen
	DDRD |= 0xff;
	DDRB |= 0x0f;

	// Darlington Ports ausschalten
	PORTD = 0;
	PORTB &= ~ (0x0f);
#endif
}

uint8_t darlingtonoutput_getpin(uint8_t n)
{
#if defined(__AVR_ATmega32__) || defined(__AVR_ATmega644P__)
	if (n < 8)
	{
		return PORTD & (1<< n);
	}
	else if (n < 12)
	{
		n = n - 8;
		return PORTB & (1<< n);
	}
#endif
	return ports_getOutput(n);
}

void darlingtonoutput_setpin(uint8_t n, uint8_t state)
{
#if defined(__AVR_ATmega32__) || defined(__AVR_ATmega644P__)
	if (n < 8)
	{
		DDRD |= (1<< n); // Output Modus setzen
		if (state) PORTD |= (1<< n);
		else	   PORTD &= ~ (1<< n);
	}
	else if (n < 12)
	{
		n = n - 8;
		DDRB |= (1<< n); // Output Modus setzen
		if (state)	PORTB |= (1<< n);
		else 		PORTB &= ~ (1<< n);
	}
	else
	{
		//canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("chgShOutPinStat,n=%d,stat=%d"), n, state);
		ports_setOutput(n, state);
	}
#elif defined(__AVR_ATmega328P__)
	//canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("chgShOutPinStat,n=%d,stat=%d"), n, state);
	ports_setOutput(n, state);
#endif
}
