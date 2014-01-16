#include <avr/io.h>
#include "tasterinput.h"
#include <canix/canix.h>
#include "shiftio.h"
#include <devices.h>

uint8_t tasterport_read(uint8_t n)
{
	if ((n < 2) && (hasInExt || hasOutExt))
	{
		// PC0, PC1 shiftIn und oder shiftOut als SCL=PC0 und SDA=PC1 verwendet
		// error: PC0 und 1 steht nicht als Eingang zur Verfuegung!
	}
	else if ( (n == 2) && hasInExt )
	{
		// PC2 fuer shift-In als Interruptleitung verwendet.
		// error: PC2 steht nicht als Eingang zur Verfuegung!
	}
	else if (n < 8)
	{
		// Pins sind 1:1 von PORTC auszulesen

		// Modus Input setzen
		DDRC &= ~ (1<< n);

		// Pullup einschalten
		PORTC |= (1<< n);

		return PINC & (1<< n);
	}
	else if (n < 16)
	{
		// auf den Bereich 0-7 holen:
		n &= 0x07;

		// Pins sind zu spiegel ( 0 -> 7, 1 -> 6 etc.)
		n = 7 - n;

		// Modus Input setzen
		DDRA &= ~ (1<< n);

		// Pullup einschalten
		PORTA |= (1<< n);

		return PINA & (1<< n);
	}
	else if (n < 244)
	{
		return isBitFromShiftInSet(n); // n-tes Bit abfragen
	}

	return 0;
}

