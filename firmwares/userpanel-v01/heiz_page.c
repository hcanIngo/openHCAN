#include <heiz_page.h>
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
#include "../controllerboard-1612-v01//heizung.h"

volatile uint8_t heiz_details_expected = 0;
volatile uint8_t heiz_details_heiz_id = 0;
volatile uint8_t heiz_details_mode = 0;
volatile uint8_t heiz_details_rate = 0;
volatile int16_t heiz_details_Tsoll = 0;
volatile int16_t heiz_details_Tist = 0;
volatile uint16_t heiz_details_dauer = 0;

uint8_t heiz_details_mode_edit;
int16_t heiz_details_Tsoll_edit;
uint8_t heiz_details_rate_edit;
uint16_t heiz_details_dauer_edit;

/*
 * es gibt 3 Subpages: 0,1,2
 */
uint8_t heiz_page_subpage = 0;
uint8_t heiz_page_state = STATE_VIEW;

uint8_t get_heiz_details(uint8_t heiz_id, 
		uint8_t *mode,
		uint8_t *rate,
		int16_t *Tsoll,
		uint16_t *dauer,
		uint16_t *Tist)
{
	canix_frame message;

	heiz_details_heiz_id = heiz_id;
	heiz_details_Tsoll = 0;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_HEIZUNG_DETAILS_REQUEST;
	message.data[2] = heiz_id;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
	heiz_details_expected = 1; // Bit 0 = HCAN_HES_HEIZUNG_DETAILS_REQUEST

	// Auf die Anwort warten
	canix_sleep_100th(5);

	// ... und noch nach T(ist) fragen...
	message.data[1] = HCAN_HES_HEIZUNG_TIST_REQUEST;
	message.data[2] = heiz_id;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
	heiz_details_expected = 2; // Bit 1 = HCAN_HES_HEIZUNG_TIST_REQUEST

	// Auf die Anwort warten
	canix_sleep_100th(5);

	if (heiz_details_expected == 0)
	{
		// aha, es ist eine Antwort eingetroffen...

		*mode = heiz_details_mode;
		*rate = heiz_details_rate;
		*Tsoll = heiz_details_Tsoll;
		*Tist = heiz_details_Tist;
		*dauer = heiz_details_dauer;
		heiz_details_heiz_id = 0;

		return 0;
	}

	// Timeout... keine Antwort erhalten :-(
	return 1;
}

void set_heiz_edited_details(eds_heiz_page_block_t *p)
{
	canix_frame message;

	// 1. wir muessen anhand er subpage herausfinden, welcher Parameter
	// geaendert wurde
	
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;

	switch (heiz_page_subpage)
	{
		case 0 :
			// hier wurde der Modus geaendert. Das bedeutet, dass wir
			// die Parameter, die dieser Modus beinhaltet, nicht kennen und
			// erstmal Defaults senden muessen. Der Benutzer kann dann
			// diese Parameter finetunen, wenn der Modus gewechselt ist.

			switch (heiz_details_mode_edit)
			{
				case HEIZUNG_MODE_OFF :
					message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_OFF;
					message.data[2] = p->heiz_id;
					message.size = 3;
					break;
				case HEIZUNG_MODE_MANUAL :
					message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_MANUAL;
					message.data[2] = p->heiz_id;
					message.data[3] = 0;
					message.data[4] = 84;
					message.data[5] = 96; // Dauer: 6h
					message.size = 6;
					break;
				case HEIZUNG_MODE_THERMOSTAT : 
					message.data[1] = 
						HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;
					message.data[2] = p->heiz_id;
					message.data[3] = 1;
					message.data[4] = 64; // 20 Grad ist Standard
					message.data[5] = 84;
					message.data[6] = 96; // Dauer: 6h
					message.size = 7;
					break;
				case HEIZUNG_MODE_AUTOMATIK : 
					message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK;
					message.data[2] = p->heiz_id;
					message.size = 3;
					break;
			}
			break;
		
		case 1 :
		case 2 :
			if (heiz_details_mode_edit == HEIZUNG_MODE_MANUAL)
			{
				message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_MANUAL;
				message.data[2] = p->heiz_id;
				message.data[3] = heiz_details_rate_edit;
				message.data[4] = heiz_details_dauer_edit >> 8;
				message.data[5] = heiz_details_dauer_edit;
				message.size = 6;
				break;
			}
			if (heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT)
			{
				message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;
				message.data[2] = p->heiz_id;
				message.data[3] = heiz_details_Tsoll_edit >> 8;
				message.data[4] = heiz_details_Tsoll_edit;
				message.data[5] = heiz_details_dauer_edit >> 8;
				message.data[6] = heiz_details_dauer_edit;
				message.size = 7;
				break;
			}
				
	}
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
	
}

void heiz_page_handle_key_down_event(eds_heiz_page_block_t *p, 
		uint8_t key)
{
	uint16_t dummy;

	if (key == KEY_OK)
	{
		if (heiz_page_state == STATE_VIEW)
		{
			// Wir beginnen mit dem Editieren; dazu muessen zuerst die
			// aktuellen Parameter in die *_edit Variablen geladen werden,
			// damit sie spaeter per RETURN uebernommen, oder per ESC
			// verworfen werden koennen

			get_heiz_details(p->heiz_id, 
					&heiz_details_mode_edit, 
					&heiz_details_rate_edit,
					&heiz_details_Tsoll_edit,
					&heiz_details_dauer_edit,
					&dummy);

			// Ausnahme: Wenn wir im Automatik-Modus sind, kann man natuerlich
			// die Tsoll nicht aendern (Tsoll wird auf subpage 1 editiert)
			if ((heiz_page_subpage == 1) &&
					(heiz_details_mode_edit == HEIZUNG_MODE_AUTOMATIK))
				return;

			heiz_page_state = STATE_EDIT;
			lcd_command(LCD_DISP_ON_CURSOR_BLINK);
			heiz_page_print_page(p);
			return;
		}
		if (heiz_page_state == STATE_EDIT)
		{
			// das Editieren wurde mit RETURN abgeschlossen; d.h. wir muessen
			// den Wert uebernehmen und ihn an die Heizung weiterreichen
			heiz_page_state = STATE_VIEW;
			lcd_command(LCD_DISP_ON);
			set_heiz_edited_details(p);
			heiz_page_print_page(p);
			return;
		}
	}

	if (key == KEY_ESC)
	{
		if (heiz_page_state == STATE_VIEW)
		{
			heiz_page_subpage = 0;
			goto_page_up();
			return;
		}
		if (heiz_page_state == STATE_EDIT)
		{
			heiz_page_state = STATE_VIEW;
			lcd_command(LCD_DISP_ON);
			heiz_page_print_page(p);
			return;
		}
	}

	if (key == KEY_UP)
	{
		if (heiz_page_state == STATE_VIEW)
		{
			if (heiz_page_subpage > 0)
				heiz_page_subpage--;
			heiz_page_print_page(p);
			return;
		}
		if (heiz_page_state == STATE_EDIT)
		{
			switch (heiz_page_subpage)
			{
				case 0 :
					if (heiz_details_mode_edit < HEIZUNG_MODE_AUTOMATIK)
						heiz_details_mode_edit++;
					break;
				case 1 :
					if (heiz_details_mode_edit == HEIZUNG_MODE_MANUAL)
					{
						if (heiz_details_rate_edit < 100)
							heiz_details_rate_edit += 10;
					}
					if (heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT)
					{
						if (heiz_details_Tsoll_edit < 480)
							heiz_details_Tsoll_edit += 2;
					}
					break;
				case 2 :
					if ((heiz_details_mode_edit == HEIZUNG_MODE_MANUAL) ||
							(heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT))
					{
						if (heiz_details_dauer_edit < 43200) // 12h
						{
							heiz_details_dauer_edit = heiz_details_dauer_edit
								/ 900 * 900;
							heiz_details_dauer_edit += 900;
						}
					}
					break;

			}
			heiz_page_print_page(p);
			return;
		}
	}

	if (key == KEY_DOWN)
	{
		if (heiz_page_state == STATE_VIEW)
		{
			uint8_t OBERE_GRENZE = 2; // HEIZUNG_MODE_THERMOSTAT...
			if(heiz_details_mode_edit == HEIZUNG_MODE_OFF
			   || heiz_details_mode_edit == HEIZUNG_MODE_AUTOMATIK)
				OBERE_GRENZE = 0; // HEIZUNG_MODE_OFF oder HEIZUNG_MODE_AUTOMATIK

			if (heiz_page_subpage < OBERE_GRENZE)
				heiz_page_subpage++;
			heiz_page_print_page(p);
			return;
		}
		if (heiz_page_state == STATE_EDIT)
		{
			switch (heiz_page_subpage)
			{
				case 0 :
					if (heiz_details_mode_edit > HEIZUNG_MODE_OFF)
						heiz_details_mode_edit--;
					break;
				case 1 :
					if (heiz_details_mode_edit == HEIZUNG_MODE_MANUAL)
					{
						if (heiz_details_rate_edit > 0)
							heiz_details_rate_edit -= 10;
					}
					if (heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT)
					{
						if (heiz_details_Tsoll_edit > 0)
							heiz_details_Tsoll_edit -= 2;
					}
					break;
				case 2 :
					if ((heiz_details_mode_edit == HEIZUNG_MODE_MANUAL) ||
							(heiz_details_mode_edit == HEIZUNG_MODE_THERMOSTAT))
					{
						if (heiz_details_dauer_edit > 900)
						{
							heiz_details_dauer_edit = heiz_details_dauer_edit
								/ 900 * 900;
							heiz_details_dauer_edit -= 900;
						}
					}
					break;

			}
			heiz_page_print_page(p);
			return;
		}
	}
}

void heiz_page_print_page(eds_heiz_page_block_t *p)
{
	char s[32];
	uint8_t mode;
	uint8_t rate;
	uint16_t dauer;
	int16_t Tsoll;
	int16_t Tist;
	uint8_t t1,t2,d1,d2;

	// LCD Bildschirm loeschen
	lcd_clrscr();

	if (heiz_page_state == STATE_VIEW)
	{
		if (get_heiz_details(p->heiz_id, &mode, &rate, &Tsoll, 
					&dauer, (uint16_t *) &Tist) != 0)
		{
			// vermutlich keine Antwort gekommen
			snprintf_P(s,sizeof(s)-1,
					PSTR("Fehler!"));
			lcd_gotoxy(0,1);
			lcd_puts(s);
			return;
		}
	}
	if (heiz_page_state == STATE_EDIT)
	{
		mode = heiz_details_mode_edit;
		rate = heiz_details_rate_edit;
		Tsoll = heiz_details_Tsoll_edit;
		dauer = heiz_details_dauer_edit;
		Tist = heiz_details_Tist;
	}

	// 1. Zeile
	snprintf_P(s,sizeof(s)-1,PSTR("%6s"),p->name);
	lcd_gotoxy(0,0);
	lcd_puts(s);

	t1 = Tist >> 4;
	t2 = ((Tist & 0x0f) * 10) >> 4;
	snprintf_P(s,sizeof(s)-1,PSTR(" Ti=%02d.%d\337C"),t1,t2);
	lcd_gotoxy(6,0);
	lcd_puts(s);

	// 2. Zeile
	//
	// Je nach STATE unterschiedliche Werte verwenden:

	t1 = Tsoll >> 4;
	t2 = ((Tsoll & 0x0f) * 10) >> 4;

	d1 = dauer / 3600;
	d2 = (dauer / 60) % 60;

	switch (heiz_page_subpage)
	{
		case 0 : 
			switch (mode)
			{
				case HEIZUNG_MODE_OFF :
					snprintf_P(s,sizeof(s)-1, PSTR("Mode aus"));
					break;
				case HEIZUNG_MODE_MANUAL :
					snprintf_P(s,sizeof(s)-1, PSTR("Mode manuell"));
					break;
				case HEIZUNG_MODE_THERMOSTAT : 
					snprintf_P(s,sizeof(s)-1, PSTR("Mode Therm."));
					break;
				case HEIZUNG_MODE_AUTOMATIK :
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
					snprintf_P(s,sizeof(s)-1, PSTR(" - "));
					break;
				case HEIZUNG_MODE_MANUAL :
					snprintf_P(s,sizeof(s)-1, PSTR("Rate=%d%%"), rate);
					break;
				case HEIZUNG_MODE_THERMOSTAT : 
					snprintf_P(s,sizeof(s)-1, PSTR("Ts=%02d.%d\337C"),
							t1,t2);
					break;
				case HEIZUNG_MODE_AUTOMATIK :
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
					snprintf_P(s,sizeof(s)-1, PSTR(" - "));
					break;
				case HEIZUNG_MODE_MANUAL :
					snprintf_P(s,sizeof(s)-1, PSTR("Dauer=%d:%02dh"),
							d1,d2);
					break;
				case HEIZUNG_MODE_THERMOSTAT : 
					snprintf_P(s,sizeof(s)-1, PSTR("Dauer=%d:%02dh"),
							d1,d2);
					break;
				case HEIZUNG_MODE_AUTOMATIK :
					snprintf_P(s,sizeof(s)-1, PSTR(" - "));
					break;
			}
			lcd_gotoxy(0,1);
			lcd_puts(s);
			break;
	}
}

void heiz_page_can_callback(eds_heiz_page_block_t *p,
		const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS :
		case HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS :
		case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS :
		case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS :
			{
				if (heiz_details_expected &&
						(frame->data[2] == heiz_details_heiz_id))
				{
					// es weren Heiz Details erwartet und die Heizungs-ID
					// passt auch; das muss also die erwartete Message
					// sein
					heiz_details_expected &= ~0x01;
					switch (frame->data[1])
					{
						case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS :
							heiz_details_mode = HEIZUNG_MODE_OFF;
							heiz_details_Tsoll = 0;
							heiz_details_dauer = 0;
							break;
						case HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS :
							heiz_details_mode = HEIZUNG_MODE_MANUAL;
							heiz_details_rate = frame->data[3];
							heiz_details_dauer = (frame->data[4] << 8) |
								frame->data[5];
							break;
						case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS :
							heiz_details_mode = HEIZUNG_MODE_THERMOSTAT;
							heiz_details_Tsoll = (frame->data[3] << 8) |
								frame->data[4];
							heiz_details_dauer = (frame->data[5] << 8) |
								frame->data[6];
							break;
						case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS :
							heiz_details_mode = HEIZUNG_MODE_AUTOMATIK;
							heiz_details_Tsoll = (frame->data[3] << 8) |
								frame->data[4];
							heiz_details_dauer = 0;
							break;

					}
				}
			}
			break;
		case HCAN_HES_HEIZUNG_TIST_REPLAY :
			{
				if (frame->data[2] == heiz_details_heiz_id)
				{
					heiz_details_expected &= ~0x02;
					heiz_details_Tist = (frame->data[3] << 8) |
						frame->data[4];
				}
			}
			break;
	}
}
