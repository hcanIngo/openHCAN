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
#include "powerportAutomat.h"

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "darlingtonoutput.h"
#include <devices.h>

static inline void sendMessage(device_data_powerportAutomat *p, uint8_t active);

void powerportAutomat_init(device_data_powerportAutomat *p, eds_block_p it)
{
	/* 65535 bedeutet das noch kein Helligkeitstelegramm empfangen wurde
	 * Wertebereich des Helligkeitssensor: 0 ... 1023 */
	p->brightness = 65535;
	
	p->msgIstVersendet = false;

	/* Wir gehen davon aus, das die Automatik aktiviert ist. Ist ein Schalter konfiguriert,  
	 * so wird er auch seinen Zustand melden, wenn irgendein C1612-Board einen Reboot durchfuehrt. */
	p->automatikEin = true;
}

void powerportAutomat_can_callback(device_data_powerportAutomat *p, const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_SCHALTER_ON :
			if ( (p->config.automatikEin_schalter_gruppe != 255) &&
			     (frame->data[2] == p->config.automatikEin_schalter_gruppe) )
			{
				p->automatikEin = true;
			}
			else if ( (p->config.steller_gruppe != 255) &&
			          (frame->data[2] == p->config.steller_gruppe) )
			{
				sendMessage(p, HCAN_HES_POWER_GROUP_ON);
			}
			break;

		case HCAN_HES_SCHALTER_OFF :
			if ( (p->config.automatikEin_schalter_gruppe != 255) &&
			     (frame->data[2] == p->config.automatikEin_schalter_gruppe) )
			{
				p->automatikEin = false;
			}
			else if ( (p->config.steller_gruppe != 255) &&
			          (frame->data[2] == p->config.steller_gruppe) )
			{
				sendMessage(p, HCAN_HES_POWER_GROUP_OFF);
			}
			break;

		case HCAN_HES_HELLIGKEITS_INFO :
			if ( (p->config.helligkeits_gruppe != 255) && // Helligkeitssensor vorhanden
			     (frame->data[2] == p->config.helligkeits_gruppe) ) // Helligkeits-Gruppe
			{
				// Helligkeitswert speichern:
				p->brightness = (frame->data[3] << 8) + frame->data[4]; // [3] = brightness_hi
			}
			break;
	}
}

inline void powerportAutomat_timer_handler(device_data_powerportAutomat *p, uint8_t zyklus) {}

static inline void sendMessage(device_data_powerportAutomat *p, uint8_t active)
{
	canix_frame message;

	// Helligkeitssensor:
	if(	p->automatikEin // Automatikbetrieb
		&& (HCAN_HES_POWER_GROUP_ON == active) // HCAN_HES_POWER_GROUP_OFF soll immer moeglich sein
		&& (p->brightness != 65535) // Helligkeitssensor verwenden?
		&& (p->brightness > p->config.helligkeitsschwelle) ) // zu hell?
	{
		return; // zu hell -> kein HCAN_HES_POWER_GROUP_ON senden
	}

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = active; // HCAN_HES_POWER_GROUP_ON oder HCAN_HES_POWER_GROUP_OFF
	message.data[2] = p->config.powerport_gruppe;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}
