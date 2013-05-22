#include <usb.h>
#include <avr/io.h>

uint8_t usb_writable(void)
{
	return (PIND & (1<<PD5)) == 0;
}

void usb_putc(unsigned char t)
{

	t = ((t&1)?128:0)|((t&2)?64:0)|((t&4)?32:0)|((t&8)?16:0)|((t&16)?8:0)
		|((t&32)?4:0)|((t&64)?2:0)|((t&128)?1:0);


	// TXE auf input schalten, Pullup on
	DDRD &= ~(1<< PD5);
	PORTD |= (1<< PD5);

	// Warten, solange TXE auf hi
	while (PIND & (1<<PD5));

	// so, jetzt ist er bereit
	// Datenport auf Ausgang
	DDRC = 0xff;
	PORTC = t;

	// USB_WR_ON;
	PORTD |= (1<< PD6);
	DDRD |= (1<< PD6);
	asm("NOP");
	asm("NOP");
	asm("NOP");
	
	// USB_WR_OFF;
	PORTD &= ~(1<< PD6);

	// Data output hochohmig schalten
	DDRC = 0x00;

}

uint8_t usb_readable(void)
{
	// RXF auf input schalten, Pullup on
	DDRD &= ~(1<<PD4);
	PORTD |= (1<<PD5);
	asm("NOP");
	asm("NOP");
	asm("NOP");

	return (PIND & (1<<PD4)) == 0;
}

uint8_t usb_getc(void)
{
	unsigned char t;

	// RXF auf input schalten, Pullup on
	DDRD &= ~(1<<PD4);
	PORTD |= (1<<PD5);

	while (PIND & (1<<PD4));

	// Datenport auf input, Pullup on
	DDRC = 0x00;
	PORTC = 0xff;

	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");

	// RD auf lo schalten
	PORTD &= ~(1<< PD7);
	DDRD |= (1<< PD7);

	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");

	// Daten lesen
	t = PINC;


	t = ((t&1)?128:0)|((t&2)?64:0)|((t&4)?32:0)|((t&8)?16:0)|((t&16)?8:0)
		|((t&32)?4:0)|((t&64)?2:0)|((t&128)?1:0);

	// RD auf hi schalten
	PORTD |= (1<< PD7);

	return t;
}

