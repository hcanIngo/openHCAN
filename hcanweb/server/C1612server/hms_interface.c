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
 *  (c) 2013 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 *  based on hcan:
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 */
#include <hms_interface.h>

#include <shared_hms_interface.h> // fuer processCmd...

#include "web/ip_config.h" // wg. DEBUG
#include "usart.h"

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
#include <avr/pgmspace.h>
#include <string.h> // fuer strcmp

t_global global;

// msg z.B. HCAN_HES_POWER_GROUP_STATE_QUERY, HCAN_HES_POWER_GROUP_ON, HCAN_HES_POWER_GROUP_OFF
static inline void sendMessage (const uint8_t msg, uint8_t id)
{
	canix_frame message;
	message.src = canix_selfaddr ();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = msg;
	message.data[2] = id; // gruppe
	message.size = 3;
	canix_frame_send_with_prio (&message, HCAN_PRIO_HI);
}

static inline void sendMessage1 (const uint8_t msg, uint8_t id, uint8_t x)
{
	canix_frame message;
	message.src = canix_selfaddr ();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = msg;
	message.data[2] = id; // gruppe
	message.data[3] = x;
	message.size = 4;
	canix_frame_send_with_prio (&message, HCAN_PRIO_HI);
}

static inline void sendMessage4 (const uint8_t msg, uint8_t id, uint8_t a, uint8_t b, uint8_t x, uint8_t y)
{
	canix_frame message;
	message.src = canix_selfaddr ();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = msg;
	message.data[2] = id; // gruppe
	message.data[3] = a;
	message.data[4] = b;
	message.data[5] = x;
	message.data[6] = y;
	message.size = 7;
	canix_frame_send_with_prio (&message, HCAN_PRIO_HI);
}

static void device_can_callback (const canix_frame *frame)
{
	// Die Antwort zur Device State Anfrage ist da
	// Neue Zustaende vom CAN-Bus erhalten?
	if (HCAN_HES_POWER_GROUP_STATE_REPLAY == global.response.deviceQuery)
	{
		if (HCAN_HES_POWER_GROUP_STATE_REPLAY == frame->data[1])
		{
			if (frame->data[2] == global.response.id) // ist eine gesuchte gruppe
			{
				global.response.updated = 1;
				global.response.state = frame->data[3];
		/*			if(frame->data[4] == 0)
					snprintf_P(s,sizeof(s), PSTR("[%s]"), frame->data[3] ? "ein" : "aus");
				else
					snprintf_P(s,sizeof(s), PSTR("[%s %3dmin]"), frame->data[3] ? "aus in" : "bis ein", frame->data[4]);
	*/		}
		}
	}
	else if (HCAN_HES_HEIZUNG_DETAILS_REQUEST == global.response.deviceQuery)
	{
		if (frame->data[2] == global.response.id) // ist eine gesuchte gruppe
		{
			switch (frame->data[1])
			{
				case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS :
				case HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS :
					global.response.updated = 1;
					global.response.state = 0; // Soll-Temperatur
					break;
				case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS :
				case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS :
					global.response.updated = 1;
					global.response.state = ((frame->data[3]<<8)|frame->data[4])/16.0; // Soll-Temperatur
					break;
				//default: // Fehler: Unbekannter Heizungs-Mode
			}
		}
	}
	else if (HCAN_HES_REEDKONTAKT_STATE_REPLAY == global.response.deviceQuery)
	{
		if (HCAN_HES_REEDKONTAKT_STATE_REPLAY == frame->data[1])
		{
			if (frame->data[2] == global.response.id) // ist eine gesuchte gruppe
			{
				global.response.updated = 1;
				global.response.state = frame->data[3]; // 0 = zu, 1 = offen
			}
		}
	}
	else if (HCAN_HES_ROLLADEN_POSITION_REPLAY == global.response.deviceQuery)
	{
		if (HCAN_HES_ROLLADEN_POSITION_REPLAY == frame->data[1])
		{
			if (frame->data[2] == global.response.id) // ist eine gesuchte gruppe
			{
				global.response.updated = 1;
				global.response.state = frame->data[3]; // Rolladenposition im Bereich [0..100]; 100% = oben
			}
		}
	}
	else if (HCAN_HES_1WIRE_TEMPERATURE_REPLAY == global.response.deviceQuery)
	{
		if (HCAN_HES_1WIRE_TEMPERATURE_REPLAY == frame->data[1])
		{
			if (frame->data[2] == global.response.id) // ist eine gesuchte gruppe
			{
				global.response.updated = 1;
				global.response.state = frame->data[3];
			}
		}
	}
}

void hauselektrik_handler (void)
{
	t_query * query = &global.query;

	processCmd (query);
	// Trennung von cmd und query (Hintergrund: ein Rolladen braucht bis er seine z.B. 80 % erreicht hat)
	/* ggf. im Client: query.queryStates  ggf. nur noch nach got focus/betaetigung und 2s-Timeout */
	processQuery (query);
}

void hauselektrik_callback (const canix_frame *frame)
{
	// Dieser Handler wird fuer alle Destination Adressen ausgefuehrt
	// daher muss gefiltert werden, was uns betrifft und was nicht:
	if ((frame->proto != HCAN_PROTO_SFP) || (frame->data[0] != HCAN_SRV_HES))
		return; // Diese Message ist nicht interessant, daher ignorieren

	if ( (frame->dst != canix_selfaddr()) && (
		 (frame->dst != HCAN_MULTICAST_CONTROL) ||
		 (frame->dst != HCAN_MULTICAST_INFO)) )
			return; // Diese Message ist nicht interessant, daher ignorieren

	device_can_callback (frame);
}

static void receiveMessage (uint8_t deviceQuery, uint8_t id, uint16_t * status, uint16_t * x)
{
	// selektieren, auf welche CAN-Nachricht gewartet wird:
	global.response.deviceQuery = deviceQuery; // quasi select (...)
	global.response.updated = 0; // init mit nicht aktualisiert
	global.response.id = id; // quasi select (...)

	uint16_t counter = 65535; // x µs
	while (counter--)
	{
		wdt_reset();
		canix_process_messages_withoutIdle (); // somit kommt der hauselektrik_callback dran
		// ... und withoutIdle, damit der web_handler hier nicht aufgerufen wird
		if (global.response.updated)
		{
			*status = global.response.state;
			// ggf. x = &global.response.timer;
			return; // Antwort in weniger als counter x ms erhalten
		}
	}
}

uint16_t queryState_specific (uint8_t query, uint8_t id)
{
	uint16_t status;
	if (HCAN_HES_POWER_GROUP_STATE_QUERY == query)
	{
		uint8_t timer;
		sendMessage (HCAN_HES_POWER_GROUP_STATE_QUERY, id);
		receiveMessage (HCAN_HES_POWER_GROUP_STATE_REPLAY, id, &status, (uint16_t *)&timer);
		return (uint16_t)status;
	}
	else if (HCAN_HES_HEIZUNG_DETAILS_REQUEST == query)
	{
		uint8_t timer;
		sendMessage (HCAN_HES_HEIZUNG_DETAILS_REQUEST, id);
		receiveMessage (HCAN_HES_HEIZUNG_DETAILS_REQUEST, id, &status, (uint16_t *)&timer);
		return (uint16_t)status;
	}
	else if (HCAN_HES_ROLLADEN_POSITION_REQUEST == query)
	{
		uint8_t timer;
		sendMessage (HCAN_HES_ROLLADEN_POSITION_REQUEST, id);
		receiveMessage (HCAN_HES_ROLLADEN_POSITION_REPLAY, id, &status, (uint16_t *)&timer);
		return (uint16_t)status;
	}
	else if (HCAN_HES_REEDKONTAKT_STATE_QUERY == query)
	{
		uint8_t timer;
		sendMessage (HCAN_HES_REEDKONTAKT_STATE_QUERY, id);
		receiveMessage (HCAN_HES_REEDKONTAKT_STATE_REPLAY, id, &status, (uint16_t *)&timer);
		return (uint16_t)status;
	}
	else if (HCAN_HES_1WIRE_TEMPERATURE_QUERY == query)
	{
		uint8_t timer;
		sendMessage (HCAN_HES_1WIRE_TEMPERATURE_QUERY, id);
		receiveMessage (HCAN_HES_1WIRE_TEMPERATURE_REPLAY, id, &status, (uint16_t *)&timer);
		return (uint16_t)status;
	}

	return 0; // Fehler
}

void executeCmd_specific (uint8_t id, uint8_t cmd, uint16_t x, uint16_t y)
{
	if (HCAN_HES_POWER_GROUP_ON == cmd)
		sendMessage (cmd, id);

	else if (HCAN_HES_POWER_GROUP_OFF == cmd)
		sendMessage (cmd, id);

	else if (HCAN_HES_HEIZUNG_SET_MODE_OFF == cmd)
		sendMessage (cmd, id);

	else if (HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK == cmd)
		sendMessage (cmd, id);

	else if (HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS == cmd)
	{
		uint16_t dauer = x;
		uint16_t temp = y;
		sendMessage4 (cmd, id, temp>>8, temp, dauer>>8, dauer);
	}
	else if (HCAN_HES_ROLLADEN_POSITION_SET == cmd)
	{
		sendMessage1 (cmd, id, x); // x = Position zwischen 0 und 100
	}
	else
	{
		// "executeCmd_specific"-Fehler: page=" + page
	}
}