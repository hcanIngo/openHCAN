#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "onewire.h"

#define W1_PIN  ow_pin
#define W1_IN   PINA
#define W1_OUT  PORTA
#define W1_DDR  DDRA

uint8_t ow_pin = 0;

void ow_setpin(uint8_t pin)
{
	// Umordnen der Pins:
	// AD0 = 15, AD7 = 8
	ow_pin = (15 - pin);
}

uint8_t ow_getpin(void)
{
	return (15 - ow_pin);
}

uint8_t ow_reset(void)
{
	uint8_t err;

	W1_OUT &= ~(1<<W1_PIN);
	W1_DDR |= 1<<W1_PIN;
	udelay(480);
	cli();
	W1_DDR &= ~(1<<W1_PIN);
	udelay(66);
	err = W1_IN & (1<<W1_PIN);          // no presence detect
	sei();
	udelay(480 - 66);
	if( (W1_IN & (1<<W1_PIN)) == 0 )        // short circuit
		err = 1;
	return err;
}

uint8_t ow_bit_io(uint8_t bit)
{
	cli();
	W1_DDR |= 1<<W1_PIN;
	udelay(1);
	if( bit )
		W1_DDR &= ~(1<<W1_PIN);
	udelay(14);
	if( (W1_IN & (1<<W1_PIN)) == 0 )
		bit = 0;
	udelay( 60 - 15 );
	W1_DDR &= ~(1<<W1_PIN);
	sei();
	return bit;
}

uint8_t ow_write_byte(uint8_t b)
{
	uint8_t i = 8, j;
	do
	{
		j = ow_bit_io( b & 1 );
		b >>= 1;
		if( j )
			b |= 0x80;
	}
	while ( --i );
	return b;
}

uint8_t ow_read_byte(void)
{
	return ow_write_byte(0xff);
}


uint8_t ow_rom_search(uint8_t diff, uint8_t *id)
{
	uint8_t i, j, next_diff;
	uint8_t b;

	if (ow_reset())
		return PRESENCE_ERR;

	ow_write_byte(SEARCH_ROM);
	next_diff = LAST_DEVICE;
	i = 8 * 8; // 8 bytes

	do
	{
		j = 8;
		do
		{
			b = ow_bit_io(1); // read bit
			if (ow_bit_io(1)) // read complement bit
			{
				if (b)        // 11
					return DATA_ERR;
			}
			else
			{
				if (!b)
				{
					if (diff > i || ((*id & 1) && (diff != i)))
					{
						b = 1;          // now 1
						next_diff = i;  // next pass 0
					}
				}
			}
			ow_bit_io(b);  // write bit
			*id >>= 1;
			if (b)
				*id |= 0x80;
			i--;
		}
		while (--j);
		id++; // next byte
	}
	while (i);

	return next_diff; // to continue search
}

void ow_command(uint8_t cmd, uint8_t *id)
{
	uint8_t i;
	ow_reset();
	if (id)
	{
		ow_write_byte(MATCH_ROM); // to a single device
		i = 8;
		do
		{
			ow_write_byte(*id);
			id++;
		}
		while (--i);
	}
	else
	{
		ow_write_byte(SKIP_ROM); // to all devices
	}
	ow_write_byte(cmd);
}


void ow_discover(const canix_frame *frame)
{
	uint8_t id[8], diff;

	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;

	for (diff = SEARCH_FIRST; diff != LAST_DEVICE; )
	{
		wdt_reset();

		diff = ow_rom_search(diff, id );
		if( diff == PRESENCE_ERR )
		{
			answer.data[1] = HCAN_HES_1WIRE_ERROR;
			answer.data[2] = 3; // No sensor found
			answer.size    = 3;
			canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
			break;
		}

		if( diff == DATA_ERR )
		{
			answer.data[1] = HCAN_HES_1WIRE_ERROR;
			answer.data[2] = 4; // Bus error
			answer.size    = 3;
			canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
			break;
		}

		answer.data[1] = HCAN_HES_1WIRE_DISCOVERED_PART_1;
		answer.data[2] = id[0];
		answer.data[3] = id[1];
		answer.data[4] = id[2];
		answer.data[5] = id[3];
		answer.size = 6;
		canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
		//delay(100);
		canix_sleep_100th(2);

		wdt_reset();

		answer.data[1] = HCAN_HES_1WIRE_DISCOVERED_PART_2;
		answer.data[2] = id[4];
		answer.data[3] = id[5];
		answer.data[4] = id[6];
		answer.data[5] = id[7];
		answer.size = 6;
		canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
		//delay(100);
		canix_sleep_100th(2);
	}
}

