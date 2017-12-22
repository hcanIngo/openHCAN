#include <avr/io.h>
#include "tasterinput.h"
#include <canix/canix.h>
#include "devices.h"

uint8_t tasterport_read(uint8_t n)
{
	if (expanderActive && (n < 2))
	{
    	return 0;
  	}
	
	if (n < 8)
	{
		// Pins sind 1:1 von PORTC auszulesen

		DDRC &= ~ (1<< n); // Modus Input setzen
		PORTC |= (1<< n); // Pullup einschalten
		return PINC & (1<< n);
	}
#if defined(__AVR_ATmega32__) || defined(__AVR_ATmega644P__)
	else if (n < 16)
	{
		n &= 0x07; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins sind zu spiegel ( 0 -> 7, 1 -> 6 etc.)
		DDRA &= ~ (1<< n); // Modus Input setzen
		PORTA |= (1<< n); // Pullup einschalten
		return PINA & (1<< n);
	}
#endif
	else if (n < 244)
	{
		return ports_getInput (n); // n-tes Bit abfragen
	}

	return 0;
}

