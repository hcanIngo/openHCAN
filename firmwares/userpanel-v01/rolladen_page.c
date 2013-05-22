#include <rolladen_page.h>

#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <keys.h>
#include <string.h>
#include <stdio.h>
#include <lcd.h>
#include <lcdctrl.h>
#include <eds.h>
#include <eds-structs.h>
#include <canix/rtc.h>
#include <canix/tools.h>
#include <hcan_multicast.h>
#include <hcan.h>
#include <hcan-def.h>
#include <lcdstatemachine.h>

/*
 *
 */

void rolladen_page_handle_key_down_event(eds_rolladen_page_block_t *p, 
		uint8_t key)
{
	if (key == KEY_OK)
	{
		canix_frame message;

		message.src = canix_selfaddr();
		message.dst = HCAN_MULTICAST_CONTROL;
		message.proto = HCAN_PROTO_SFP;
		message.data[0] = HCAN_SRV_HES;
		message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
		message.data[2] = p->gruppe;
		message.data[3] = p->pos;
		message.size = 4;
		canix_frame_send_with_prio(&message, HCAN_PRIO_HI);

		lcdctrl_blink();

		return;
	}

	if (key == KEY_ESC)
	{
		goto_page_up();
		return;
	}

	if (key == KEY_UP)
		goto_prev_page();

	if (key == KEY_DOWN)
		goto_next_page();
}

void rolladen_page_print_page(eds_rolladen_page_block_t *p)
{
	char s[32];

	// LCD Bildschirm loeschen
	lcd_clrscr();

	// 1. Zeile
	snprintf_P(s,sizeof(s)-1, PSTR("%s"), p->line0);
	lcd_gotoxy(0,0);
	lcd_puts(s);

	snprintf_P(s,sizeof(s)-1, PSTR("%s"), p->line1);
	lcd_gotoxy(0,1);
	lcd_puts(s);
}

void rolladen_page_can_callback(eds_rolladen_page_block_t *p,
		const canix_frame *frame)
{
}

