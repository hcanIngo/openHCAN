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
 *  (c) 2017 by Ingo Lages, i (dot) lages (at) gmx (dot) de
 */
#include "heiz_monitor.h"

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
#include <canix/syslog.h>


volatile uint8_t heiz_configured = 1; // ersteinmal aktivieren
volatile uint8_t monitor_Tsoll[24];

void get_count_Tsoll(uint8_t *nUeber0Grad, uint8_t *nUeber15Grad)
{
	*nUeber0Grad = 0;
	*nUeber15Grad = 0;
	uint8_t i;
	for (i = 0; i < 24; i++)
	{
		if (monitor_Tsoll[i] != 255)
		{
			if ((monitor_Tsoll[i] > 15)) // > 15°C
			{
				(*nUeber15Grad)++;
				canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("H%d"), i);
			}
			else if (monitor_Tsoll[i] > 0) // > 0°C
			{
				(*nUeber0Grad)++;
				canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("h%d"), i);
			}
		}
	}
}


/**
 * Fragt alle konfigurierten Heizungen nach ihrem Status und sammelt
 * die Ergebnisse in monitor_heiz_state
 */
static void init_monitor_heiz_request_states(void)
{
	eds_block_p it = eds_find_next_block((eds_block_p)0, EDS_userpanel_heizungen_BLOCK_ID);
	if (!it)
	{
		load_error_page(ERROR_CONF_HEIZUNG_MISSING);
		heiz_configured = 0;
		return;
	}

	userpanel_heizungen_t c;
	eeprom_read_block(&c, (it+2), sizeof(c));
	uint8_t i;

	// Ergebnis-Array initialisieren:
	for (i = 0; i < 24; i++)
		monitor_Tsoll[i] = 255;

	// ueber alle Heizungen iterieren:
	for (i = 0; i < 24; i++)
	{
		if (c.heizung[i] != 255) // ist es ein konfigurierte Heizung?
		{
			// Nach Status fragen; die Ergebnisse kommen asynchron
			// ueber den CAN Handler rein
			canix_frame message;
			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_CONTROL;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_HES;
			message.data[1] = HCAN_HES_HEIZUNG_DETAILS_REQUEST;
			message.data[2] = c.heizung[i]; // die Heizungs-Gruppen-ID
			message.size = 3;
			canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);

			canix_sleep_100th(10); // 100msec Pause
		}
		wdt_reset();
	}
}

static void monitor_insert_heiz_state(uint8_t gruppe, uint16_t Tsoll)
{
	eds_block_p it = eds_find_next_block((eds_block_p)0, EDS_userpanel_heizungen_BLOCK_ID);
	if (!it)
	{
		load_error_page(ERROR_CONF_HEIZUNG_MISSING);
		return;
	}
	userpanel_heizungen_t c;
	eeprom_read_block(&c, (it+2), sizeof(c)); // Ist ein EEPROM-Zugriff, daher langsamer!

	uint8_t i;
	for (i = 0; i < 24; i++)
	{
		if (gruppe == c.heizung[i])
		{
			monitor_Tsoll[i] = Tsoll >> 4; // ganzzahlig durch 16 geteilt
			//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("%d=%d=%d"), i, gruppe, monitor_Tsoll[i]);
		}
	}
}

void monitor_heiz_can_callback(const canix_frame *frame)
{
	int16_t Tsoll;

	if (!heiz_configured)
		return;

	switch (frame->data[1])
	{
		case HCAN_HES_BOARD_ACTIVE :
			init_monitor_heiz_request_states(); // noch Hochlauf, sonst warten wir auf _STATE_CHANGE
			break;

		case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS :
			monitor_insert_heiz_state(frame->data[2], 0); // Tsoll = 0
			break;

		case HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS :
			monitor_insert_heiz_state(frame->data[2], 1); // keine Solltemp. nur direkt eine "rate"
			break;

		case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS :
		case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS :
			Tsoll = (frame->data[3] << 8) | frame->data[4];
			monitor_insert_heiz_state(frame->data[2], (uint16_t)Tsoll);
			break;
	}
}
