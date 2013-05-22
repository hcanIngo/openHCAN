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
 *  (c) 2010 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 */
#include <weck_page.h>
#include <lcdstatemachine.h>
#include <canix/syslog.h>

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

// Fuer die Powerport-Feature-Konstanten
#include "../controllerboard-1612-v01/powerport.h"

volatile uint8_t weck_details_expected = 0;
volatile uint8_t weck_details_weck_id = 0;
volatile uint16_t weck_details_dauer = 0;

uint16_t weck_details_dauer_edit;
uint8_t weck_page_state = STATE_VIEW;


uint8_t get_weck_details(uint8_t weck_id, uint16_t *dauer)
{
	canix_frame message;

	weck_details_weck_id = weck_id;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_WECKER_DETAILS_REQUEST;
	message.data[2] = weck_id;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
	weck_details_expected = 1; // Bit 0 = HCAN_HES_WECKER_DETAILS_REQUEST

	// Auf die Anwort warten
	canix_sleep_100th(5);

	if (weck_details_expected == 0)
	{
		// aha, es ist eine Antwort eingetroffen... (und zwar HCAN_HES_WECKER_DETAILS)

		*dauer = weck_details_dauer;
		weck_details_weck_id = 0;

		return 0;
	}

	// Timeout... keine Antwort erhalten :-(
	return 1;
}

void set_weck_edited_details(eds_weck_page_block_t *p)
{
	canix_frame message;

	// 1. herausfinden, welcher Parameter geaendert wurde
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_WECKER_SET;
	message.data[2] = p->weck_id; //=Powerport-Gruppe

	message.data[3] = POWERPORT_FEATURE_WECK_VERZOEGERT_EIN; //Vorbelegung
	if ( p->feature & (1<<POWERPORT_FEATURE_WECK_VERZOEGERT_EIN) )
	{
		message.data[3] = POWERPORT_FEATURE_WECK_VERZOEGERT_EIN;
	}
	else if ( p->feature & (1<<POWERPORT_FEATURE_WECK_AUTO_AUS) )
	{
		message.data[3] = POWERPORT_FEATURE_WECK_AUTO_AUS;
	}

	message.data[4] = weck_details_dauer_edit >> 8; //Dauer bis Wecker einschaltet
	message.data[5] = weck_details_dauer_edit; //LSB
	message.size = 6;

	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);

}

void weck_page_handle_key_down_event(eds_weck_page_block_t *p, uint8_t key)
{
	if (key == KEY_OK)
	{
		if (weck_page_state == STATE_VIEW)
		{
			// Wir beginnen mit dem Editieren; dazu muessen zuerst die
			// aktuellen Parameter in die *_edit Variablen geladen werden,
			// damit sie spaeter per RETURN uebernommen, oder per ESC
			// verworfen werden koennen

			//get_weck_details(p->weck_id, &weck_details_dauer_edit);
			weck_details_dauer_edit = 1200; //Startwert: 1200 s = 20 min

			weck_page_state = STATE_EDIT;
			lcd_command(LCD_DISP_ON_CURSOR_BLINK);
			weck_page_print_page(p);
			return;
		}
		if (weck_page_state == STATE_EDIT)
		{
			// das Editieren wurde mit RETURN abgeschlossen; d.h. wir muessen
			// den Wert uebernehmen und ihn an den Wecker weiterreichen
			weck_page_state = STATE_VIEW;
			lcd_command(LCD_DISP_ON);
			set_weck_edited_details(p);
			weck_page_print_page(p);
			return;
		}
	}

	if (key == KEY_ESC)
	{
		if (weck_page_state == STATE_VIEW)
		{
			goto_page_up();
			return;
		}

		if (weck_page_state == STATE_EDIT)
		{
			weck_page_state = STATE_VIEW;
			lcd_command(LCD_DISP_ON);
			weck_page_print_page(p);
			return;
		}
	}

	if (key == KEY_UP)
	{
		if (weck_page_state == STATE_EDIT)
		{
			if (weck_details_dauer_edit < 43200) // 12h
			{
				weck_details_dauer_edit =
					((weck_details_dauer_edit/300)+1) * 300; //Achtung: 299/300=0, 599/300=1
				weck_details_dauer_edit += 300; //5 min Schritte
			}

			weck_page_print_page(p);
			return;
		}
	}

	if (key == KEY_DOWN)
	{
		if (weck_page_state == STATE_EDIT)
		{
			if (weck_details_dauer_edit > 300)
			{
				weck_details_dauer_edit =
					(weck_details_dauer_edit/300) * 300; //Achtung: 299/300=0, 599/300=1
					//((weck_details_dauer_edit/300)+1) * 300; //Achtung: 299/300=0, 599/300=1
				weck_details_dauer_edit -= 300; //5 min Schritte
			}

			weck_page_print_page(p);
			return;
		}
	}
}

void weck_page_print_page(eds_weck_page_block_t *p)
{
	char s[32];
	uint16_t dauer;
	uint8_t d1,d2;

	// LCD Bildschirm loeschen
	lcd_clrscr();

	if (weck_page_state == STATE_VIEW)
	{
		if (get_weck_details(p->weck_id, &dauer) != 0)
		{
			// vermutlich keine Antwort gekommen
			snprintf_P(s,sizeof(s)-1,
					PSTR("Fehler!"));
			lcd_gotoxy(0,1);
			lcd_puts(s);
			return;
		}
	}
	if (weck_page_state == STATE_EDIT)
	{
		dauer = weck_details_dauer_edit;
	}

	// 1. Zeile
	snprintf_P(s,sizeof(s)-1,PSTR("%8s"),p->name);
	lcd_gotoxy(0,0);
	lcd_puts(s);

	// 2. Zeile
	d1 = dauer / 3600;
	d2 = (dauer / 60) % 60;

	snprintf_P(s,sizeof(s)-1, PSTR("wecken in %d:%02dh"),d1,d2);
	lcd_gotoxy(0,1);
	lcd_puts(s);
}

void weck_page_can_callback(eds_weck_page_block_t *p, const canix_frame *frame)
{
		if (weck_details_expected && (frame->data[2] == weck_details_weck_id))
		{
			// es waren Weck Details erwartet und die Wecker-ID
			// passt auch; das muss also die erwartete Message
			// sein
			weck_details_expected &= ~0x01;
			if (frame->data[1] == HCAN_HES_WECKER_DETAILS)
			{
				weck_details_dauer = (frame->data[3] << 8) | frame->data[4];
			}
		}
}
