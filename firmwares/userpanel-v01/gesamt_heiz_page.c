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
#include <gesamt_heiz_page.h>
#include <lcdstatemachine.h>

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

// Fuer die Heizungs-Modi-Konstanten
#include "../controllerboard-1612-v01/heizung.h"

//Standardwerte festlegen:
const uint16_t VORGABE_TEMPERATUR_FROSTSCHUTZ = 10*16; // 10 °C (fixer Faktor 16, da in 16tel Grad)
const uint16_t VORGABE_DAUER_FROSTSCHUTZ = 64800;      // 18 h (18*3600, fixer Faktor 3600, da in Sekunden)
const uint16_t VORGABE_DAUER_FROSTSCHUTZ_KURZ = 10800; //  3 h ( 3*3600, fixer Faktor 3600, da in Sekunden)

const uint16_t VORGABE_TEMPERATUR_THERMOSTAT = 18*16;  // 18 °C (fixer Faktor 16, da in 16tel Grad)
const uint16_t VORGABE_DAUER_THERMOSTAT = 4*3600;      //  4 h  (fixer Faktor 3600, da in Sekunden)


volatile uint16_t gesamt_heiz_details_dauer = 0;

uint8_t gesamt_heiz_details_mode_edit;
int16_t gesamt_heiz_details_Tsoll_edit;
uint16_t gesamt_heiz_details_dauer_edit;

/*
 * es gibt 3 Subpages: 0,1,2
 */
uint8_t gesamt_heiz_page_subpage = 0;
uint8_t gesamt_heiz_page_state = STATE_VIEW;


void set_gesamt_heiz_edited_details(eds_gesamt_heiz_page_block_t *p)
{
	canix_frame message;

	// 1. wir muessen anhand der subpage herausfinden, welcher Parameter
	// geaendert wurde
	
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;

	switch (gesamt_heiz_page_subpage)
	{
		case 0 :
			// hier wurde der Modus geaendert. Das bedeutet, dass wir
			// die Parameter, die dieser Modus beinhaltet, nicht kennen und
			// erstmal Defaults senden muessen. Der Benutzer kann dann
			// diese Parameter finetunen, wenn der Modus gewechselt ist.

			switch (gesamt_heiz_details_mode_edit)
			{
				case HEIZUNG_MODE_OFF :
					message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_OFF;
					//message.data[2] Heizungs-ID wird unten ausgefuellt
					message.size = 3;
					break;
				case HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ :
					message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;
					//message.data[2] Heizungs-ID wird unten ausgefuellt
					message.data[3] = (uint8_t)(VORGABE_TEMPERATUR_FROSTSCHUTZ >> 8); // Hi-Byte
					message.data[4] = (uint8_t)(VORGABE_TEMPERATUR_FROSTSCHUTZ);      // Lo-Byte
					message.data[5] = (uint8_t)(VORGABE_DAUER_FROSTSCHUTZ >> 8); // Hi-Byte
					message.data[6] = (uint8_t)(VORGABE_DAUER_FROSTSCHUTZ);      // Lo-Byte
					message.size = 7;
					break;
				case HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ :
					message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;
					//message.data[2] Heizungs-ID wird unten ausgefuellt
					message.data[3] = (uint8_t)(VORGABE_TEMPERATUR_FROSTSCHUTZ >> 8); // Hi-Byte
					message.data[4] = (uint8_t)(VORGABE_TEMPERATUR_FROSTSCHUTZ);      // Lo-Byte
					message.data[5] = (uint8_t)(VORGABE_DAUER_FROSTSCHUTZ_KURZ >> 8); // Hi-Byte
					message.data[6] = (uint8_t)(VORGABE_DAUER_FROSTSCHUTZ_KURZ);      // Lo-Byte
					message.size = 7;
					break;
				case HEIZUNG_MODE_THERMOSTAT_X :
					message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;
					//message.data[2] Heizungs-ID wird unten ausgefuellt
					message.data[3] = (uint8_t)(VORGABE_TEMPERATUR_THERMOSTAT >> 8); // Hi-Byte
					message.data[4] = (uint8_t)(VORGABE_TEMPERATUR_THERMOSTAT);      // Lo-Byte
					message.data[5] = (uint8_t)(VORGABE_DAUER_THERMOSTAT >> 8); // Hi-Byte
					message.data[6] = (uint8_t)(VORGABE_DAUER_THERMOSTAT);      // Lo-Byte
					message.size = 7;
					break;
				case HEIZUNG_MODE_AUTOMATIK_X :
					message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK;
					//message.data[2] Heizungs-ID wird unten ausgefuellt
					message.size = 3;
					break;
			}
			break;
		
		case 1 :
		case 2 :
			if ( (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ) ||
				 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ) ||
				 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_X) )
			{
				message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;
				//message.data[2] Heizungs-ID wird unten ausgefuellt
				message.data[3] = gesamt_heiz_details_Tsoll_edit >> 8;
				message.data[4] = gesamt_heiz_details_Tsoll_edit;
				message.data[5] = gesamt_heiz_details_dauer_edit >> 8;
				message.data[6] = gesamt_heiz_details_dauer_edit;
				message.size = 7;
				break;
			}
				
	}
	
	//an alle Heizungen:
	uint8_t i;
	uint8_t *heiz_id;
	heiz_id = (uint8_t *) &(p->heiz_id0);
	for (i = 0; i < MAX_HEIZ_IDS; i++)
	{
		if(heiz_id[i] != 255)
		{
			message.data[2] = heiz_id[i];
			canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
		}
	}
}

void gesamt_heiz_page_handle_key_down_event(eds_gesamt_heiz_page_block_t *p,
		uint8_t key)
{
	if (key == KEY_OK)
	{
		if (gesamt_heiz_page_state == STATE_VIEW)
		{
			if (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ)
			{
					gesamt_heiz_details_Tsoll_edit = VORGABE_TEMPERATUR_FROSTSCHUTZ;
					gesamt_heiz_details_dauer_edit = VORGABE_DAUER_FROSTSCHUTZ;
			}
			else if (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ)
			{
					gesamt_heiz_details_Tsoll_edit = VORGABE_TEMPERATUR_FROSTSCHUTZ;
					gesamt_heiz_details_dauer_edit = VORGABE_DAUER_FROSTSCHUTZ_KURZ;
			}
			else if (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_X)
			{
				if (gesamt_heiz_details_Tsoll_edit == 0
					|| gesamt_heiz_details_Tsoll_edit == VORGABE_TEMPERATUR_FROSTSCHUTZ)
					gesamt_heiz_details_Tsoll_edit = VORGABE_TEMPERATUR_THERMOSTAT;
				if (gesamt_heiz_details_dauer_edit == 0
					|| gesamt_heiz_details_dauer_edit == VORGABE_DAUER_FROSTSCHUTZ)
					gesamt_heiz_details_dauer_edit = VORGABE_DAUER_THERMOSTAT;
			}

			// Ausnahme: Wenn wir im Automatik-Modus sind, kann man natuerlich
			// die Tsoll nicht aendern (Tsoll wird auf subpage 1 editiert)
			if ((gesamt_heiz_page_subpage == 1) &&
					(gesamt_heiz_details_mode_edit == HEIZUNG_MODE_AUTOMATIK_X))
				return;

			gesamt_heiz_page_state = STATE_EDIT;
			lcd_command(LCD_DISP_ON_CURSOR_BLINK);
			gesamt_heiz_page_print_page(p);
			return;
		}
		if (gesamt_heiz_page_state == STATE_EDIT)
		{
			// das Editieren wurde mit RETURN abgeschlossen; d.h. wir muessen
			// den Wert uebernehmen und ihn an die Heizung weiterreichen
			gesamt_heiz_page_state = STATE_VIEW;
			lcd_command(LCD_DISP_ON);
			set_gesamt_heiz_edited_details(p);
			gesamt_heiz_page_print_page(p);
			return;
		}
	}

	if (key == KEY_ESC)
	{
		if (gesamt_heiz_page_state == STATE_VIEW)
		{
			gesamt_heiz_page_subpage = 0;
			goto_page_up();
			return;
		}
		if (gesamt_heiz_page_state == STATE_EDIT)
		{
			gesamt_heiz_page_state = STATE_VIEW;
			lcd_command(LCD_DISP_ON);
			gesamt_heiz_page_print_page(p);
			return;
		}
	}

	if (key == KEY_UP)
	{
		if (gesamt_heiz_page_state == STATE_VIEW)
		{
			if (gesamt_heiz_page_subpage > 0)
				gesamt_heiz_page_subpage--;
			gesamt_heiz_page_print_page(p);
			return;
		}
		if (gesamt_heiz_page_state == STATE_EDIT)
		{
			switch (gesamt_heiz_page_subpage)
			{
				case 0 :
					if (gesamt_heiz_details_mode_edit < HEIZUNG_MODE_AUTOMATIK_X)
						gesamt_heiz_details_mode_edit++;
					break;
				case 1 :
					if ( (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ) ||
						 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ) ||
						 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_X) )
					{
						if (gesamt_heiz_details_Tsoll_edit < 480)
							gesamt_heiz_details_Tsoll_edit += 2;
					}
					break;
				case 2 :
					if ( (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ) ||
						 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ) ||
						 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_X) )
					{
						if (gesamt_heiz_details_dauer_edit < 43200) // 12h
						{
							gesamt_heiz_details_dauer_edit = gesamt_heiz_details_dauer_edit
								/ 900 * 900;
							gesamt_heiz_details_dauer_edit += 900;
						}
					}
					break;

			}
			gesamt_heiz_page_print_page(p);
			return;
		}
	}

	if (key == KEY_DOWN)
	{
		if (gesamt_heiz_page_state == STATE_VIEW)
		{
			uint8_t OBERE_GRENZE = 2; // HEIZUNG_MODE_THERMOSTAT_X...
			if(gesamt_heiz_details_mode_edit == HEIZUNG_MODE_OFF
			   || gesamt_heiz_details_mode_edit == HEIZUNG_MODE_AUTOMATIK_X)
				OBERE_GRENZE = 0; // HEIZUNG_MODE_OFF oder HEIZUNG_MODE_AUTOMATIK_X

			if (gesamt_heiz_page_subpage < OBERE_GRENZE)
				gesamt_heiz_page_subpage++;
			gesamt_heiz_page_print_page(p);
			return;
		}
		if (gesamt_heiz_page_state == STATE_EDIT)
		{
			switch (gesamt_heiz_page_subpage)
			{
				case 0 :
					if (gesamt_heiz_details_mode_edit > HEIZUNG_MODE_OFF)
						gesamt_heiz_details_mode_edit--;
					break;
				case 1 :
					if ( (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ) ||
						 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ) ||
						 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_X) )
					{
						if (gesamt_heiz_details_Tsoll_edit > 0)
							gesamt_heiz_details_Tsoll_edit -= 2;
					}
					break;
				case 2 :
					if ( (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ) ||
						 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ) ||
						 (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_X) )
					{
						if (gesamt_heiz_details_dauer_edit > 900)
						{
							gesamt_heiz_details_dauer_edit = gesamt_heiz_details_dauer_edit
								/ 900 * 900;
							gesamt_heiz_details_dauer_edit -= 900;
						}
					}
					break;

			}
			gesamt_heiz_page_print_page(p);
			return;
		}
	}
}

void gesamt_heiz_page_print_page(eds_gesamt_heiz_page_block_t *p)
{
	char s[32];
	uint8_t mode = gesamt_heiz_details_mode_edit;
	uint16_t dauer = gesamt_heiz_details_dauer_edit;
	int16_t Tsoll = gesamt_heiz_details_Tsoll_edit;
	uint8_t t1,t2,d1,d2;

	// LCD Bildschirm loeschen
	lcd_clrscr();

	if (gesamt_heiz_page_state == STATE_VIEW)
	{
		if (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ)
		{
				gesamt_heiz_details_Tsoll_edit = VORGABE_TEMPERATUR_FROSTSCHUTZ;
				gesamt_heiz_details_dauer_edit = VORGABE_DAUER_FROSTSCHUTZ;
		}
		else if (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ)
		{
				gesamt_heiz_details_Tsoll_edit = VORGABE_TEMPERATUR_FROSTSCHUTZ;
				gesamt_heiz_details_dauer_edit = VORGABE_DAUER_FROSTSCHUTZ_KURZ;
		}
		else if (gesamt_heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT_X)
		{
			if (gesamt_heiz_details_Tsoll_edit == 0
				|| gesamt_heiz_details_Tsoll_edit == VORGABE_TEMPERATUR_FROSTSCHUTZ)
				gesamt_heiz_details_Tsoll_edit = VORGABE_TEMPERATUR_THERMOSTAT;
			if (gesamt_heiz_details_dauer_edit == 0
				|| gesamt_heiz_details_dauer_edit == VORGABE_DAUER_FROSTSCHUTZ)
				gesamt_heiz_details_dauer_edit = VORGABE_DAUER_THERMOSTAT;
		}
	}
	if (gesamt_heiz_page_state == STATE_EDIT)
	{
		mode = gesamt_heiz_details_mode_edit;
		Tsoll = gesamt_heiz_details_Tsoll_edit;
		dauer = gesamt_heiz_details_dauer_edit;
	}

	// 1. Zeile
	snprintf_P(s,sizeof(s)-1,PSTR("%6s"),p->name);
	lcd_gotoxy(0,0);
	lcd_puts(s);

	// 2. Zeile
	//
	// Je nach STATE unterschiedliche Werte verwenden:

	t1 = Tsoll >> 4;
	t2 = ((Tsoll & 0x0f) * 10) >> 4;

	d1 = dauer / 3600;
	d2 = (dauer / 60) % 60;

	switch (gesamt_heiz_page_subpage)
	{
		case 0 : 
			switch (mode)
			{
				case HEIZUNG_MODE_OFF :
					snprintf_P(s,sizeof(s)-1, PSTR("Mode aus"));
					break;
				case HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ :
					snprintf_P(s,sizeof(s)-1, PSTR("Mode Frost."));
					break;
				case HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ :
					snprintf_P(s,sizeof(s)-1, PSTR("Mode Frost.kurz"));
					break;
				case HEIZUNG_MODE_THERMOSTAT_X :
					snprintf_P(s,sizeof(s)-1, PSTR("Mode Therm."));
					break;
				case HEIZUNG_MODE_AUTOMATIK_X :
					snprintf_P(s,sizeof(s)-1, PSTR("Mode auto"));
					break;
			}
			lcd_gotoxy(0,1);
			lcd_puts(s);
			break;

		case 1 :
			switch (mode)
			{
				case HEIZUNG_MODE_OFF :
				case HEIZUNG_MODE_AUTOMATIK_X :
					snprintf_P(s,sizeof(s)-1, PSTR(" - "));
					break;
				case HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ :
				case HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ :
				case HEIZUNG_MODE_THERMOSTAT_X :
					snprintf_P(s,sizeof(s)-1, PSTR("Ts=%02d.%d\337C"),
							t1,t2);
					break;
			}
			lcd_gotoxy(0,1);
			lcd_puts(s);
			break;

		case 2 :
			switch (mode)
			{
				case HEIZUNG_MODE_OFF :
				case HEIZUNG_MODE_AUTOMATIK_X :
					snprintf_P(s,sizeof(s)-1, PSTR(" - "));
					break;
				case HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ :
				case HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ :
				case HEIZUNG_MODE_THERMOSTAT_X :
					snprintf_P(s,sizeof(s)-1, PSTR("Dauer=%d:%02dh"),
							d1,d2);
					break;
			}
			lcd_gotoxy(0,1);
			lcd_puts(s);
			break;
	}
}

