#include <fenster_monitor_page.h>
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

volatile uint8_t fenster_monitor_reed_state[24];

/**
 * Fuegt, falls der Reedkontakt relevant ist, seinen Status in das
 * Array fenster_monitor_reed_state ein
 */
void fenster_monitor_insert_reed_state(eds_fenster_monitor_page_block_t *p,
		uint8_t gruppe, uint8_t state)
{
	uint8_t i;

	uint8_t *reeds = (uint8_t *)(&p->reed0);

	for (i = 0; i < 24; i++)
	{
		if (gruppe == reeds[i])
			fenster_monitor_reed_state[i] = state;
	}
}

/**
 * Fragt alle konfigurierten Reedkontakte nach ihrem Status und sammelt
 * die Ergebnisse in fenster_monitor_reed_state; liefert die Anzahl der
 * offenen Reedkontakte zurueck
 */
uint8_t fenster_monitor_collect_reed_states(eds_fenster_monitor_page_block_t 
		*p)
{
	uint8_t i,n_reed_open;
	uint8_t *reeds = (uint8_t *)(&p->reed0);

	// Ergebnis-Array initialisieren:
	for (i = 0; i < 24; i++)
		fenster_monitor_reed_state[i] = 255;

	// ueber alle Reedkontakte iterieren:
	for (i = 0; i < 24; i++)
	{
		if (reeds[i] != 255) // ist es ein konfigurierter Reedkontakt?
		{
			// Nach Kontakt-Status fragen; die Ergebnisse kommen asynchron
			// ueber den CAN Handler rein
			canix_frame message;
			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_CONTROL;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_HES;
			message.data[1] = HCAN_HES_REEDKONTAKT_STATE_QUERY;
			message.data[2] = reeds[i]; // die Reedkontakt-Gruppen-ID
			message.size = 3;
			canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);

			canix_sleep_100th(10); // 100msec Pause
		}
		wdt_reset();
	}

	// Reedkontakte zaehlen:
	n_reed_open = 0;
	for (i = 0; i < 24; i++)
	{
		if ((fenster_monitor_reed_state[i]) && 
				(fenster_monitor_reed_state[i] != 255))
			n_reed_open++;
	}

	return n_reed_open;
}

void fenster_monitor_page_handle_key_down_event(
		eds_fenster_monitor_page_block_t *p, uint8_t key)
{
	if (key == KEY_ESC)
	{
		goto_page_up();
	}
}

void fenster_monitor_page_print_page(eds_fenster_monitor_page_block_t *p)
{
	char s[32];

	// LCD Bildschirm loeschen
	lcd_clrscr();

	snprintf_P(s,sizeof(s)-1,
					PSTR("Fenster:"));
	lcd_gotoxy(0,0);
	lcd_puts(s);

	snprintf_P(s,sizeof(s)-1,
					PSTR(" %d offen"), fenster_monitor_collect_reed_states(p));

	lcd_gotoxy(0,1);
	lcd_puts(s);

}

void fenster_monitor_page_can_callback(eds_fenster_monitor_page_block_t *p,
		const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_REEDKONTAKT_STATE_REPLAY :
			{
				fenster_monitor_insert_reed_state(p, frame->data[2],
						frame->data[3]);
			}
			break;
	}
}
