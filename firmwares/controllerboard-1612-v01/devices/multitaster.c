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
#include <canix/canix.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include "multitaster.h"
#include "../input.h"

/* multitaster.c empfaengt Tasterbotschaften und wertet Potitelegramme aus. 
 * Beim Aussenden des Taster-down/up-Ereignisses wird der letzte 
 * aus dem Potitelegramm empfangene der ADC-Wert ausgewertet. In dessen 
 * Abhaengigkeit wird die entsprechend konfigurierte Gruppe ausgesandt. */

static void detectAndSetOneScale(device_data_multitaster *p);
static uint8_t getGroup(device_data_multitaster *p);

void multitaster_init(device_data_multitaster *p, eds_block_p it)
{
	detectAndSetOneScale(p); //multitaster_init
	p->lastPotiPos = 0; //Index auf gruppe0
}

/** Ermittelt die Skalenteilung des zugehoerigen Potis. 
 *  Die Groesse eines Skalenteils ergibt sich aus der Anzahl
 *  konfigurierter Gruppen. */
static void detectAndSetOneScale(device_data_multitaster *p)
{
	uint8_t i;
	uint8_t numOfPotiScaleDivisions = 0;
	uint8_t *gruppen;
	gruppen = (uint8_t *) &(p->config.gruppe0);

	for (i = 0; i < MAX_MULTITASTER_GROUPS; i++)
	{
		if (gruppen[i] == 255) break; //Abbruch bei der ersten Gruppe die 255 ist
		numOfPotiScaleDivisions++;
	}
	
	p->oneScale = 1023/numOfPotiScaleDivisions; //10 Bit (2^10 - 1 = 1023)
}

/** multitaster_timer_handler sendet eine Taster UP-/DOWN-Nachricht
 *  auf den CAN-Bus. Der multitaster_timer_handler wird alle 10msec aufgerufen. */ 
inline void multitaster_timer_handler(device_data_multitaster *p, uint8_t zyklus)
{
	if (zyklus != 100) return; // 100tel-Sekunden-Zyklus verwendet

	canix_frame message;
	uint8_t time;

	// Wenn Taste gedrueckt ist, dann ist der Pin 0, ansonsten 1
	uint8_t status = !inputport_read(1, p->config.taster_port);

	// Message schon mal vorbereiten:
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	// message.data[1] wird unten ausgefuellt
	message.data[2] = getGroup(p);
	// message.data[3] wird unten ausgefuellt
	message.size = 4;

	if (status) // gedrueckt
	{
		// Entprellschutz:
		if (p->pressed < 255)
			p->pressed++;
	}
	else
	{
		// Wenn Schalter-Down schon gesendet wurde,
		// dann ein Schalter-Up Event senden:
		if (p->pressed > 3)
		{
			//...und kein Rollladentaster im 2-Tasterbetrieb konfiguriert ist:
			if ( !(p->config.feature & ((1<<FEATURE_MULTITASTER_ROLLADEN_ZU) | (1<<FEATURE_MULTITASTER_ROLLADEN_AUF))) )
			{
				message.data[1] = HCAN_HES_TASTER_UP;
				message.data[3] = p->config.taster_port;
				canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
			}
		}

		// Taste wurde losgelassen, also Zaehler zuruecksetzen
		p->pressed = 0;
	}

	if (p->config.feature & (1<<FEATURE_MULTITASTER_ENTPRELL_1S))
		time = 100;
	else
		time = 3;

	
	if (p->pressed == time) // mind. 30msec gedrueckt
	{
		if (p->config.feature & (1<<FEATURE_MULTITASTER_ROLLADEN_ZU))
		{
			message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
			message.data[3] = 200; //gewuenschte Rollladenposition: Pos=200=0%=zu
		}
		else if (p->config.feature & (1<<FEATURE_MULTITASTER_ROLLADEN_AUF))
		{
			message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
			message.data[3] = 201; //gewuenschte Rollladenposition: Pos=201=100%=auf
		}
		else //kein Rollladentaster im 2-Tasterbetrieb
		{
			message.data[1] = HCAN_HES_TASTER_DOWN;
			message.data[3] = p->config.taster_port;
		}
		
		canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
	}
}


/** multitaster_can_callback speichert die sich ergebende lastPotiPos. 
 *  multitaster_can_callback wird aufgerufen, wenn auf dem CAN-Bus eine
 *  neue Poti-Nachricht vorliegt (max. alle 100 ms). */
void multitaster_can_callback(device_data_multitaster *p, 
		const canix_frame *frame)
{
	if(frame->data[1] == HCAN_HES_POTI_POS_CHANGED)
	{
		if ( frame->data[2] == p->config.poti_gruppe )
		{
			uint16_t adcValue = (frame->data[3] << 8) + frame->data[4];
			uint8_t i;

			for (i = 0; i < MAX_MULTITASTER_GROUPS; i++)
			{
				if ( adcValue <= (i * p->oneScale) )
				{
					p->lastPotiPos = i-1; //d.h. befindet sich bis zum (i-1). Skalenstrich
					//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("PotiPos=%d"), p->lastPotiPos);
					break; //break die "for-Loop"
				}
			}
		}
	}
}

/** getGroup liefert die Gruppe abh. von lastPotiPos. */
static uint8_t getGroup(device_data_multitaster *p)
{
	uint8_t *gruppen;
	gruppen = (uint8_t *) &(p->config.gruppe0);

	if(p->lastPotiPos < MAX_MULTITASTER_GROUPS)
	{
		return gruppen[p->lastPotiPos]; //gruppe0..X abh vom Potibereich zurueckgeben
	}
	else
	{
		canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("Err:lastPotiPos="), p->lastPotiPos);
		return 255; //Gruppe 255
	}
}
