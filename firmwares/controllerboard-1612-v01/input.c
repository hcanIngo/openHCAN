#include "input.h"

#include <avr/io.h>
#include <canix/canix.h>
#include "devices.h"

uint8_t inputport_read(uint8_t pullup, uint8_t n)
{
#if defined(__AVR_ATmega32__) || defined(__AVR_ATmega644P__)
	if (expanderActive && (n < 2))
	{
    	return 0; // expander @ PC0, PC1
  	}
   	if (n < 8)
	{
		DDRC &= ~ (1<< n); // Modus Input setzen
		if(pullup) PORTC |= (1<< n);   // Pullup einschalten
		else       PORTC &= ~ (1<< n); // Pullup ausschalten
		return PINC & (1<< n);
	}
   	else if (n < 16)
	{
		n &= 0x07; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins sind zu spiegel ( 0 -> 7, 1 -> 6 etc.)
		DDRA &= ~ (1<< n); // Modus Input setzen
		if(pullup) PORTA |= (1<< n);   // Pullup einschalten
		else       PORTA &= ~ (1<< n); // Pullup ausschalten
		return PINA & (1<< n);
	}
	else if (n < 244)
	{
		return ports_getInput (n); // n-tes Bit abfragen
	}
#elif defined(__AVR_ATmega328P__)
   	switch (n)
	{
		case 3:
		case 4:
			if (expanderActive) return 0; // expander @ PC3, PC4
		case 0:
		case 1:
		case 2:
		case 5:
		case 6:
		case 7:
			DDRC &= ~ (1<< n); // Modus Input setzen
			if(pullup) PORTC |= (1<< n);   // Pullup einschalten
			else       PORTC &= ~ (1<< n); // Pullup ausschalten
			return PINC & (1<< n);

		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			n &= 0x07; // auf den Bereich 0-7 holen
			if (n == 0) n = 1; // PD1
			else n += 2; // PD3..PD7
			DDRD &= ~ (1<< n); // Modus Input setzen
			if(pullup) PORTD |= (1<< n);   // Pullup einschalten
			else       PORTD &= ~ (1<< n); // Pullup ausschalten
			return PIND & (1<< n);

		case 14:
		case 15:
			n &= 0x07; // auf den Bereich 0-7 holen
			DDRB &= ~ (1<< n); // Modus Input setzen
			if(pullup) PORTB |= (1<< n);   // Pullup einschalten
			else       PORTB &= ~ (1<< n); // Pullup ausschalten
			return PINB & (1<< n);

		default:
			return ports_getInput (n); // n-tes Bit abfragen
	}
#endif

	return 0;
}

