#include <avr/io.h>
#include "tasterinput.h"
#include <canix/canix.h>
#include "shiftio.h"
#include <devices.h>

uint8_t tasterport_read(uint8_t n)
{
	if ((n == 0) && (shiftIOExt > 0))
	{
		// PC0 fuer shiftIn und oder shiftOut als Clk verwendet
		// error: PC0 steht nicht als Eingang zur Verfuegung!
	}
	else if ( ((n == 1) || (n == 4)) && (shiftIOExt == 0x01) )
	{
		// PC1, PC4 fuer shift-In verwendet. Siehe shiftio.h. Daher:
		// error: PC1, PC4 stehen nicht als Eingaenge zur Verfuegung!
	}
	else if ( ((n == 2) || (n == 3)) && (shiftIOExt == 0x10) )
	{
		// PC2, PC3 fuer shift-Out verwendet. Siehe shiftio.h. Daher:
		// error: PC2, PC3 stehen nicht als Eingaenge zur Verfuegung!
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
	/* Beispielsweise sind 7 x 8 Bit-Shiftregister
	 * angeschlossen, dann existieren 56 weitere Eingaenge.
	 * Diese Eingaenge sind dann in der EEPROM-Konfiguration
	 * mit den Indizes 16...71 einzubinden. */
	else if (n < 244) //255-16=224  244/8= 28 8-Bit Shiftregister maximal.
	{
		n -= 16; //auf den Bereich 0-223 holen
		return isBitFromShiftInSet(n); //n-tes Bit abfragen
	}

	return 0;
}

