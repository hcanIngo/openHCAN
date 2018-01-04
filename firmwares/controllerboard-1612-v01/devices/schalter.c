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
#include "schalter.h"
#include <canix/syslog.h>
#include <darlingtonoutput.h>
#include <hcan.h>

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "../input.h"

void schalter_init(device_data_schalter *p, eds_block_p it) {}

static inline void sendMessage(device_data_schalter *p, uint8_t active)
{
	canix_frame message;

	if(active)
		p->lastEdge = RISING;
	else
		p->lastEdge = FALLING;


	if(p->config.feature & (1<<FEATURE_SCHALTER_INVERTIEREN))
		active = !active; // invertieren

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	if(active)
	{
		if(p->config.feature & (1<<FEATURE_SCHALTER_MUTE))
			message.data[1] = HCAN_HES_MUTE_ON;
		else message.data[1] = HCAN_HES_SCHALTER_ON;
	}
	else
	{
		if(p->config.feature & (1<<FEATURE_SCHALTER_MUTE))
			message.data[1] = HCAN_HES_MUTE_OFF;
		else message.data[1] = HCAN_HES_SCHALTER_OFF;
	}
	message.data[2] = p->config.gruppe;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}

inline void schalter_timer_handler(device_data_schalter *p, uint8_t zyklus)
{
	if (zyklus != 10) return; // 10tel-Sekunden-Zyklus verwendet

	// µC-interner-Pullup am Eingangport aktiv:
	// Wenn Schalter high liefert, dann ist der Pin 0, ansonsten 1
	uint8_t pullup = p->config.feature & (1<<FEATURE_SCHALTER_PULLUP_AUS);
	uint8_t active = inputport_read(pullup, p->config.port) == 0;

	if (p->newState != 3)
	{
		if ((active && FALLING == p->lastEdge) || 	// Zustandswechsel 0->1?
			(!active && RISING == p->lastEdge))		// Zustandswechsel 1->0?
		{
			// Schalterstellung geaendert:
			if (p->newState < 255)
				p->newState++; // Entprellschutz
		}
	}


	if (p->newState == 3) // mind. 30msec veraenderte Schalterstellung
	{
		sendMessage(p, active);
		p->newState = 0;
	}
}


void schalter_can_callback(device_data_schalter *p, const canix_frame *frame)
{
	if (frame->data[1] == HCAN_HES_BOARD_ACTIVE)
	{
		/* HCAN_HES_BOARD_ACTIVE kommt immer wenn ein C1612 gerade hochgelaufen ist,
		 * damit devices (z. B. powerports), welche von Schaltern beeinflusst werden,
		 * ihren von der Schalterstellung abhaengigen Zustand initial erhalten.
		 * schalter_can_callback wird in main.c fuer jedes Schalter-Device einmal aufgerufen. */
		if(p->config.gruppe != 255)
		{
			canix_sleep_100th(10); // 100msec Pause
			p->newState = 3; // sodass im schalter_timer_handler der aktuelle Zustand gesendet wird
		}
	}
}
