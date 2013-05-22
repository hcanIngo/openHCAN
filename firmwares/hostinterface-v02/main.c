/*
 *  This file is part of the HCAN tools suite.
 *
 *  HCAN is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  HCAN is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with HCAN; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>


#include <canix/canix.h>
#include <canix/led.h>
#include <hcan.h>
#include <canix/tools.h>
#include <canix/mcp2515.h>
#include <canix/mcp2515.h>

#include <usb.h>

#define HOSTINTERFACE_V02_VERSION "0.2"
char hostinterface_v02_version[] PROGMEM = "HOSTINTERFACE_V02_VERSION_" \
										HOSTINTERFACE_V02_VERSION "_";


char hexchars[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d',
	    'e','f' };

#define LED1_ON  { PORTA &= ~(1<<0); led1_counter = 4; }
#define LED1_OFF PORTA |= (1<<0)
#define LED2_ON  { PORTA &= ~(1<<1); led2_counter = 4; }
#define LED2_OFF PORTA |= (1<<1)
#define LED3_ON  { PORTA &= ~(1<<2); led3_counter = 4; }
#define LED3_OFF PORTA |= (1<<2)

uint8_t led1_counter = 0;
uint8_t led2_counter = 0;
uint8_t led3_counter = 0;


uint8_t serial = 0;

void sendvalue(char c)
{
	usb_putc(hexchars[(c & 0xf0) >> 4]);
	usb_putc(hexchars[(c & 0x0f)]);
}

void handle_frame_input(void)
{
	//char data[8];
	uint32_t extended;
	uint8_t i;
	canix_frame frame;

	serial++;

	//usb_putc('X');
	//sendvalue(serial);

	// Frame Begin
	usb_putc('C');

	// Frame einlesen
	frame.size = can_read_message(&extended, (char *) &(frame.data[0]));
	frame.dst = (extended >> 10) & ADDR_MASK;
	frame.src = extended & ADDR_MASK;
	frame.proto = (extended >> 20) & PROTO_MASK;

	// ext ID
	sendvalue(extended & 0xff);
	sendvalue((extended >> 8) & 0xff);
	sendvalue((extended >> 16) & 0xff);
	sendvalue((extended >> 24) & 0xff);

	sendvalue(frame.size);

	// Daten
	for (i = 0; i < frame.size; i++)
		sendvalue(frame.data[i]);

	// Frame Ende
	usb_putc('F');
}


bool escaped = 0;
uint8_t obuffer[64];
uint8_t oindex = 0;

#define OBUFFER_OVERFLOW  2
#define SIZE_MISMATCH     3
#define MCP_INIT_ERROR    4

int firstBitsChar = -1;

void handle_serial_command(uint8_t command)
{
	switch (command)
	{
		case 'B' : // Resync noetig
			oindex = 0;
			firstBitsChar = -1;
			usb_putc('B');
			break;
		case 'C' : // neues Frame kommt, also vorbereiten
			firstBitsChar = -1;
			oindex = 0;
			break;
		case 'F' : 
			{
				uint8_t size; // Frame ist komplett uebertragen, versenden
				uint32_t extended;
				extended = 
					(obuffer[0]) |
					(((uint32_t)obuffer[1]) << 8) |
					(((uint32_t)obuffer[2]) << 16) |
					(((uint32_t)obuffer[3]) << 24);

				if (obuffer[4] != oindex - 5)
				{
					wdt_enable(WDTO_15MS);
					while (1);
				}
				size = obuffer[4];

				LED2_ON;
				can_send_message(extended, &obuffer[5], size);
				break;
			}
		case 'P' : // ping
			//if (usb_writable())
				usb_putc('P');
			break;
		case 'R' : // reset
			wdt_enable(WDTO_15MS);
			while (1);
		case 'S' : // State: 02 == App/Firmware
			usb_putc('0');
			usb_putc('2');
			break;
	}
}


uint8_t convert_hexstr2int(char msb, char lsb)
{
	uint8_t v = 0;

	if ((msb >= '0') && (msb <= '9'))
		v |= (msb - '0') << 4;
	else
		v |= ((msb - 'a') + 10) << 4;

	if ((lsb >= '0') && (lsb <= '9'))
		v |= (lsb - '0');
	else
		v |= ((lsb - 'a') + 10);

	return v;
}

void handle_serial_input(char c)
{
	// Commmands sind im Bereich: [A-Z]
	if ((c >= 'A') && (c <= 'Z'))
	{
		handle_serial_command(c);
	}
	else
	{
		// es ist ein Wert:
		if (firstBitsChar == -1)
			firstBitsChar = c;
		else
		{
			obuffer[oindex++] = convert_hexstr2int((uint8_t)firstBitsChar,c);
			firstBitsChar = -1;
		}
	}

	// Bounds check
	if (oindex > 63)
	{
		wdt_enable(WDTO_15MS);
		while (1);
	}
}

void led_off_timer(void)
{
	// wird mit 100Hz aufgerufen
	
	if (led1_counter > 0)
	{
		led1_counter -= 1;

		if (led1_counter == 1)
			LED1_OFF;
	}
	if (led2_counter > 0)
	{
		led2_counter -= 1;

		if (led2_counter == 1)
			LED2_OFF;
	}
	if (led3_counter > 0)
	{
		led3_counter -= 1;

		if (led3_counter == 1)
			LED3_OFF;
	}
}

int main(void)
{
	wdt_reset();

	DDRA = 0xff;
	PORTA |= 0x03;

	canix_init();
	canix_reg_rtc_callback(led_off_timer);
	
	usb_putc('Y');
	usb_putc('0' + MCUCSR);
	MCUCSR = 0;

	LED3_OFF;
	LED3_OFF;

	while (1)
	{
		wdt_reset();

		// Pruefen ob Nachricht empfangen wurde:
		if (can_message_available() == true)
		{
			LED1_ON;
			handle_frame_input();
		}

		// Pruefen ob ein char ueber die serielle Schnittstelle kam
		if (usb_readable())
		{
			char c;
			c = usb_getc();
			handle_serial_input(c);
		}
	}

	return 0;
}

