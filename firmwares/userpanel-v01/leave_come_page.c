#include <leave_come_page.h>
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
#include "lampen_monitor.h"
#include "sonstige_monitor.h"
#include "heiz_monitor.h"

//Standardwerte festlegen:
const uint16_t LEAVE_COME_VORGABE_TEMPERATUR_FROSTSCHUTZ = 10*16; // 10 °C (fixer Faktor 16, da in 16tel Grad)
const uint16_t LEAVE_COME_VORGABE_DAUER_FROSTSCHUTZ = 64800;      // 18 h (18*3600, fixer Faktor 3600, da in Sekunden)


volatile uint8_t leave_come_state;

uint8_t leave_come_page_get_ignore(uint8_t gruppe)
{
	eds_block_p it = eds_find_next_block((eds_block_p)0, EDS_leave_come_page_BLOCK_ID);
	if (!it)
	{
		load_error_page(ERROR_PAGE_LEAVE_COME_MISSING);
		return 1;
	}
	userpanel_ignore_t c;
	eeprom_read_block(&c, (it+2), sizeof(c)); // Ist ein EEPROM-Zugriff, daher langsamer!


	// in allen ignore-Gruppen schauen:
	uint8_t i;
	for (i = 0; i < 4; i++)
	{
		if (c.ignore[i] == gruppe) // ignorieren (Ausnahme z.B. Garage)?
			return 1;
	}

	return 0; // diese Gruppe muss nicht ignoriert werden
}

static void leave_come_page_set_heizung(uint8_t mode)
{
	eds_block_p it = eds_find_next_block((eds_block_p)0, EDS_userpanel_heizungen_BLOCK_ID);
	if (!it)
	{
		load_error_page(ERROR_CONF_HEIZUNG_MISSING);
		return;
	}
	userpanel_heizungen_t c;
	eeprom_read_block(&c, (it+2), sizeof(c)); // Ist ein EEPROM-Zugriff, daher langsamer!


	canix_frame message;
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	if (HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ == mode)
	{
		message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;
		//message.data[2] Heizungs-ID wird unten ausgefuellt
		message.data[3] = (uint8_t)(LEAVE_COME_VORGABE_TEMPERATUR_FROSTSCHUTZ >> 8); // Hi-Byte
		message.data[4] = (uint8_t)(LEAVE_COME_VORGABE_TEMPERATUR_FROSTSCHUTZ);      // Lo-Byte
		message.data[5] = (uint8_t)(LEAVE_COME_VORGABE_DAUER_FROSTSCHUTZ >> 8); // Hi-Byte
		message.data[6] = (uint8_t)(LEAVE_COME_VORGABE_DAUER_FROSTSCHUTZ);      // Lo-Byte
		message.size = 7;
	}
	else
	{
		message.data[1] = HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK;
		//message.data[2] Heizungs-ID wird unten ausgefuellt
		message.size = 3;
	}
	// ueber alle Heizungen iterieren:
	uint8_t i;
	for (i = 0; i < 24; i++)
	{
		if (c.heizung[i] != 255) // ist es ein konfigurierte Heizung?
		{
			message.data[2] = c.heizung[i];
			canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);

			canix_sleep_100th(10); // 100msec Pause
		}
		wdt_reset();
	}
}

static void leave_come_page_set_lampen_aus(void)
{
	eds_block_p it = eds_find_next_block((eds_block_p)0, EDS_userpanel_lampen_BLOCK_ID);
	if (!it)
	{
		load_error_page(ERROR_CONF_LAMPEN_MISSING);
		return;
	}

	userpanel_lampen_t c;
	eeprom_read_block(&c, (it+2), sizeof(c));
	uint8_t i;

	// ueber alle Lampen iterieren:
	for (i = 0; i < 24; i++)
	{
		if (c.lampe[i] != 255 && !leave_come_page_get_ignore(i)) // konfiguriert und soll nicht ignoriert werden?
		{
			// Nach Kontakt-Status fragen; die Ergebnisse kommen asynchron
			// ueber den CAN Handler rein
			canix_frame message;
			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_CONTROL;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_HES;
			message.data[1] = HCAN_HES_POWER_GROUP_OFF;
			message.data[2] = c.lampe[i]; // die Lampen-Gruppen-ID
			message.size = 3;
			canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);

			canix_sleep_100th(10); // 100msec Pause
		}
		wdt_reset();
	}
}

static void leave_come_page_set_sonstige_aus(void)
{
	eds_block_p it = eds_find_next_block((eds_block_p)0, EDS_userpanel_sonstige_BLOCK_ID);
	if (!it)
	{
		load_error_page(ERROR_CONF_SONSTIGE_MISSING);
		return;
	}

	userpanel_sonstige_t c;
	eeprom_read_block(&c, (it+2), sizeof(c));

	// ueber alle Sonstige iterieren:
	uint8_t i;
	for (i = 0; i < 24; i++)
	{
		if (c.sonstiges[i] != 255 && !leave_come_page_get_ignore(i)) // konfiguriert und soll nicht ignoriert werden?
		{
			// Nach Kontakt-Status fragen; die Ergebnisse kommen asynchron
			// ueber den CAN Handler rein
			canix_frame message;
			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_CONTROL;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_HES;
			message.data[1] = HCAN_HES_POWER_GROUP_OFF;
			message.data[2] = c.sonstiges[i]; // die Sonstige-Gruppen-ID
			message.size = 3;
			canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);

			canix_sleep_100th(10); // 100msec Pause
		}
		wdt_reset();
	}
}

/* leave oder come aus dem Zustand ermitteln:
 *
 * - leave, wenn ein Verbraucher (H>15°C, L oder S) aktiv ist
 * - come, wenn alle Heizungen auf Frostschutz stehen (0°C < h < 16°C)
 * - sonst "keine Aktion notwendig"
 */
static uint8_t leave_come_page_get_state (void)
{
	uint8_t nUeber0Grad, nUeber15Grad;
	get_count_Tsoll(&nUeber0Grad, &nUeber15Grad);

	if (0 < nUeber15Grad) // Heizbetrieb?
		return LEAVE_HEATED;

	if (0 < nUeber0Grad) // Frostschutz aktiv?
		return COME_HEATED;

	// bei abgeschalteter Heizung (im Sommer):
	if ((0 < get_count_lampen_states()) || (0 < get_count_sonstige_states(1))) // Verbraucher aktiv? (Hinweis: sonstige ohne ignore)
		return LEAVE_HEATING_IS_OFF;
	else
		return COME_HEATING_IS_OFF;

	return NO_ACTION;
}

void leave_come_page_handle_key_down_event(
		eds_leave_come_page_block_t *p, uint8_t key)
{
	if (key == KEY_ESC)
	{
		goto_page_up();
	}
	if (key == KEY_OK)
	{
		switch (leave_come_state)
		{
			case LEAVE_HEATED :
				leave_come_page_set_lampen_aus();
				leave_come_page_set_sonstige_aus(); // Garage per ignore herausnehmbar
				leave_come_page_set_heizung(HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ);
				break;

			case LEAVE_HEATING_IS_OFF :
				leave_come_page_set_lampen_aus();
				leave_come_page_set_sonstige_aus(); // Garage per ignore herausnehmbar
				break;

			case COME_HEATED :
				leave_come_page_set_heizung(HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK);
				break;

			case COME_HEATING_IS_OFF :
				goto_page_up(); // keine Aktion
				return;
		}

		lcdctrl_blink();
		canix_sleep_100th(5);
		goto_page_up();
	}
}

void leave_come_page_print_page(eds_leave_come_page_block_t *p)
{
	char s[32];

	lcd_clrscr(); // LCD Bildschirm loeschen

	/*
	   leave -> "Verbraucher
	             abschalten?"

	   come  -> "Automatik-Modus
	             fuer Heizungen?"
	*/
	leave_come_state = leave_come_page_get_state ();
	if (   (LEAVE_HEATED == leave_come_state)
		|| (LEAVE_HEATING_IS_OFF == leave_come_state))
		snprintf_P(s, sizeof(s)-1, PSTR("Verbraucher"));
	else if (COME_HEATED == leave_come_state)
		snprintf_P(s, sizeof(s)-1, PSTR("Automatik-Modus"));
	else
		snprintf_P(s, sizeof(s)-1, PSTR("keine Aktion"));

	lcd_gotoxy(0,0);
	lcd_puts(s);

	// 2. Zeile:
	if (   (LEAVE_HEATED == leave_come_state)
		|| (LEAVE_HEATING_IS_OFF == leave_come_state))
		snprintf_P(s, sizeof(s)-1, PSTR("abschalten?"));
	else if (COME_HEATED == leave_come_state)
		snprintf_P(s, sizeof(s)-1, PSTR("fuer Heizungen?"));
	else
		snprintf_P(s, sizeof(s)-1, PSTR("notwendig"));

	lcd_gotoxy(0,1);
	lcd_puts(s);
}
