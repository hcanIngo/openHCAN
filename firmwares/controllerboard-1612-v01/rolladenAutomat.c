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
#include <rolladenAutomat.h>
#include <canix/syslog.h>
#include <darlingtonoutput.h>
#include <hcan.h>

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

#include <darlingtonoutput.h>
#include <devices.h>
#include <zeitzone.h>

static inline void sendMessage(device_data_rolladenAutomat *p);

void rolladenAutomat_init(device_data_rolladenAutomat *p, eds_block_p it)
{
	/* 65535 bedeutet das noch kein Helligkeitstelegramm empfangen wurde
	 * Wertebereich des Helligkeitssensor: 0 ... 1023 */
	p->brightness = 65535;
	
	p->msgIstVersendet = false;

	/* Wir gehen davon aus, das die Automatik aktiviert ist. Ist ein Schalter konfiguriert,  
	 * so wird er auch seinen Zustand melden, wenn irgendein C1612-Board einen Reboot durchfuehrt. */
	p->automatikEin = true;

	p->SchwellwertErkanntSecsCnt = 0; // Neustart
	p->SchwellwertErkannt = HELLIGKEIT_IM_TOLERANZBEREICH;
}

void rolladenAutomat_timer_handler(device_data_rolladenAutomat *p)
{
	// wird im Sekunden-Abstand aufgerufen

	uint8_t i;
	uint8_t* zeitzonen_ids;
	uint8_t zeitzone_match = false;

	if (!p->automatikEin) return; // kein Automatikbetrieb (keine Rollladenautomatik)

	zeitzonen_ids = &(p->config.zeitzone0_id);
	
	for (i = 0; i < N_ZEITZONEN; i++)
	{
		if (zeitzone_matches(zeitzonen_ids[i]))
		{
			if (p->config.helligkeits_gruppe == 255)
			{
				sendMessage(p); // ohne Helligkeitssensor
			}
			else if (p->brightness != 65535) // Helligkeitssensor verwendet und Nachricht empfangen:
			{
				if (p->config.helligkeitswertueberschreitung)
				{
					// Rollladen soll geöffnet werden, wenn es hell wird
					// Reaktion auf Ueberschreitung der helligkeitsschwelle
					if (p->brightness > p->config.helligkeitsschwelle)
					{
						p->SchwellwertErkanntSecsCnt++;
						p->SchwellwertErkannt = HELLIGKEITS_UEBERSCHREITUNG_ERKANNT; // nach Zeitablauf sendMessage
					}
					else
					{
						p->SchwellwertErkanntSecsCnt = 0; // Neustart
						p->SchwellwertErkannt = HELLIGKEIT_IM_TOLERANZBEREICH;
					}
				}
				else // Unterschreitung konfiguriert:
				{
					// Rollladen soll geschlossen werden, wenn es dunkel wird
					// Reaktion auf Unterschreitung der helligkeitsschwelle
					if (p->brightness < p->config.helligkeitsschwelle)
					{
						p->SchwellwertErkanntSecsCnt++;
						p->SchwellwertErkannt = HELLIGKEITS_UNTERSCHREITUNG_ERKANNT; // nach Zeitablauf sendMessage
					}
					else
					{
						p->SchwellwertErkanntSecsCnt = 0; // Neustart
						p->SchwellwertErkannt = HELLIGKEIT_IM_TOLERANZBEREICH;
					}
				}
			}

			zeitzone_match = true;
			break; // for-Schleife abbrechen, sobald eine Zeitzone uebereinstimmt
		}
	}
	
	// ausserhalb aller Zeitzonen?
	if( ! zeitzone_match) p->msgIstVersendet = false; // ab jetzt kann wieder gesendet werden


	if (p->SchwellwertErkanntSecsCnt == 900) // 15 min
	{
		sendMessage(p);
		p->SchwellwertErkanntSecsCnt = 0; // Neustart
	}
}

void rolladenAutomat_can_callback(device_data_rolladenAutomat *p, const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_POWER_GROUP_ON :
			if (frame->data[2] == p->config.automatikEin_schalter_gruppe)
			{
				p->automatikEin = true;
			}
			break;

		case HCAN_HES_POWER_GROUP_OFF :
			if (frame->data[2] == p->config.automatikEin_schalter_gruppe)
			{
				p->automatikEin = false;
			}
			break;

		case HCAN_HES_HELLIGKEITS_INFO :
			if (p->config.helligkeits_gruppe != 255 // Helligkeitssensor vorhanden
				&& frame->data[2] == p->config.helligkeits_gruppe) // Helligkeits-Gruppe
			{
				// Helligkeitswert speichern:
				p->brightness = (frame->data[3] << 8) + frame->data[4]; // [3] = brightness_hi
			}
			break;
	}
}

static inline void sendMessage(device_data_rolladenAutomat *p)
{
	canix_frame message;
	
	if (p->msgIstVersendet)
	{
		// Wir gehen davon aus, dass die zweite Zeitzone einen anderen Tag betrifft.
		return; // Nachricht wurde schon versendet
	}
	else
	{
		message.src = canix_selfaddr();
		message.dst = HCAN_MULTICAST_CONTROL;
		message.proto = HCAN_PROTO_SFP;
		message.data[0] = HCAN_SRV_HES;
		message.data[2] = p->config.rolladen_gruppe;
		message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
		message.data[3] = p->config.rolladen_soll_position; // gewuenschte Rollladenposition: Pos=0=0%=zu
		message.size    = 4;

		canix_frame_send(&message);
		
		p->msgIstVersendet = true;
	}
}
