#include <powerport_page.h>

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

/**
 * der Status der konfigurierten Action Gruppe
 */
static int8_t powerport_page_state = -1;

/**
 * der Timer der Lichtzone, sofern eine Lichtzone die konfigurierte
 * Gruppe verwaltet
 */
static int8_t powerport_page_timer = -1;

void powerport_page_handle_key_down_event(eds_powerport_page_block_t *p, 
		uint8_t key)
{
	if (key == KEY_OK)
	{
		if ((powerport_page_state == 1) && (powerport_page_timer > 0))
		{
			// Powerport ist eingeschaltet und der Timer laeuft; in diesem
			// Fall schaltet die OK Taste den Timer aus, laesst aber den
			// Powerport an
			//
			canix_frame message;

			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_CONTROL;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_HES;
			message.data[1] = HCAN_HES_POWER_GROUP_SET_TIMER;
			message.data[2] = p->gruppe;
			message.data[3] = 0;
			message.data[4] = 0;
			message.size = 5;
			canix_frame_send_with_prio(&message, HCAN_PRIO_HI);

			lcdctrl_blink();
			canix_sleep_100th(5);
			powerport_page_print_page(p);

			return;

		}

		// Wenn der Powerport ein oder aus ist:
		if (powerport_page_state != -1)
		{
			canix_frame message;

			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_CONTROL;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_HES;
			message.data[1] = powerport_page_state ? 
				HCAN_HES_POWER_GROUP_OFF : HCAN_HES_POWER_GROUP_ON;
			message.data[2] = p->gruppe;
			message.size = 3;
			canix_frame_send_with_prio(&message, HCAN_PRIO_HI);

			lcdctrl_blink();
			canix_sleep_100th(5);
			powerport_page_print_page(p);

			return;
		}
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

void powerport_page_print_page(eds_powerport_page_block_t *p)
{
	char s[40];
	canix_frame message;

	// LCD Bildschirm loeschen
	lcd_clrscr();

	// 1. Zeile
	snprintf_P(s,sizeof(s), PSTR("%s"), p->line0); 
	lcd_gotoxy(0,0);
	lcd_puts(s);

	snprintf_P(s,sizeof(s), PSTR("%s"), p->line1); 
	lcd_gotoxy(0,1);
	lcd_puts(s);

	// In powerport_page_state wird spaeter der State gespeichert; sollte
	// keine Meldung eintreffen, so nuetzt es nichts, wenn der User OK
	// drueckt - man weiss ja den State nicht und kann ihn nicht aendern!
	// Das erkennt man am Wert -1
	powerport_page_state = -1;
	powerport_page_timer = -1;

	// nun den Status des Powerports erfragen und (asynchron)
	// in powerport_page_can_callback weitermachen

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_POWER_GROUP_STATE_QUERY;
	message.data[2] = p->gruppe;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);

	canix_sleep_100th(1);

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_POWER_GROUP_TIMER_QUERY;
	message.data[2] = p->gruppe;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);

}

void powerport_page_can_callback(eds_powerport_page_block_t *p,
		const canix_frame *frame)
{
	if ((frame->proto == HCAN_PROTO_SFP) && (frame->data[0] == HCAN_SRV_HES))
	{
		switch (frame->data[1])
		{
			case HCAN_HES_POWER_GROUP_STATE_REPLAY :
				{
					// die Antwort zur Powerport State Anfrage ist da

					if (frame->data[2] == p->gruppe)
					{
						char s[16];
						if(frame->data[4] == 0)
						{
							snprintf_P(s,sizeof(s), PSTR("[%s]"), 
								frame->data[3] ? "ein" : "aus");
						}
						else
						{
							snprintf_P(s,sizeof(s), PSTR("[%s %3dmin]"), 
									frame->data[3] ? "aus in" : "bis ein", frame->data[4]);
							
						}
						lcd_gotoxy(0,1); // ex 4,1
						lcd_puts(s);

						// State merken, da der User eventuell OK drueckt
						// und der State dann geaendert werden muss
						//

						powerport_page_state = frame->data[3] ? 1 : 0;
					}
				}
				break;

			case HCAN_HES_POWER_GROUP_TIMER_REPLAY :
				{
					if (frame->data[2] == p->gruppe)
					{
						char s[16];

						if ((frame->data[3] == 0) && (frame->data[4] == 0))
						{
							powerport_page_timer = 0;

							// bei abgelaufenem Timer keine Zeitinfo 
							// aufs Display schreiben
							return;
						}
						else
						{
							powerport_page_timer = (((frame->data[3] << 8) |
								frame->data[4]) + 30) / 60;
						}

						snprintf_P(s,sizeof(s), PSTR(",%3dmin]"), 
								powerport_page_timer);
						lcd_gotoxy(8,1);
						lcd_puts(s);
					}
				}
				break;
		}
	}
}

