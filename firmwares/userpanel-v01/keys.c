#include <keys.h>
#include <avr/io.h>
#include <canix/syslog.h>
#include <lcd.h>
#include <lcdstatemachine.h>

#define KEY_COUNTER_START 5

#define MAX_KEY_FLOAT_COUNTER  16
uint8_t key_counter[MAX_KEY_FLOAT_COUNTER];
uint8_t key_state[MAX_KEY_FLOAT_COUNTER];

void keys_init(void)
{
	int i;

	// Activate Key-Swtiches: Configure to input, enable internal pull-up
	DDRA &= ~ 0x3f;
	PORTA |= 0x3f;

	DDRD &= ~ 0xcf;
	PORTD |= 0xcf;

	for (i = 0; i < MAX_KEY_FLOAT_COUNTER; i++)
	{
		key_counter[i] = 0;
		key_state[i] = 0;
	}
}

uint16_t keys_key_pressed(void)
{
	uint16_t flags;

	flags = (PINA << 8) | PIND;
	flags = (~flags) & KEY_MASK;
	return flags;
}

void keys_handle_pressed(void)
{
	uint16_t keys;
	uint8_t i;

	keys = keys_key_pressed();

	// Magic Reset Key Combi
	if ((keys & (1<<KEY_ESC)) && (keys & (1<<KEY_UP)) 
			&& (keys & (1<<KEY_DOWN)))
		canix_reset();

	for (i = 0; i < 16; i++)
	{
		if (key_counter[i] == 0)
		{
			// Wenn Taste bisher nicht gedrueckt war und sie es jetzt ist
			// dann Zaehler starten und key_down_event() ausloesen
			if ((key_state[i] == 0) && (keys & (1<<i)))
			{
				key_counter[i] = KEY_COUNTER_START;
				key_state[i] = 1;
				key_down_event(i);
			}
			if ((key_state[i] == 1) && ((keys & (1<<i)) == 0))
			{
				key_counter[i] = KEY_COUNTER_START;
				key_state[i] = 0;
				key_up_event(i);
			}
		}
		else
			key_counter[i] -= 1;
	}
}

