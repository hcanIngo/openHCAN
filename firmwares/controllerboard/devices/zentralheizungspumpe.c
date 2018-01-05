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
#include "../../controllerboard/devices/zentralheizungspumpe.h"

#include <canix/syslog.h>
#include <hcan.h>

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

static inline void sendMessage(device_data_zentralheizungspumpe *p, uint8_t zustandToSend);
static inline void waermebedarfSpeichern(device_data_zentralheizungspumpe *p, uint8_t id, uint8_t waermebedarf);
static inline uint8_t getMittlererWaermebedarf(device_data_zentralheizungspumpe *p);

void zentralheizungspumpe_init(device_data_zentralheizungspumpe *p, eds_block_p it)
{
	uint8_t n;
	p->state = 0;
	p->update_counter = 0;
	for(n=0; n<MAX_ANZ_HEIZUNGEN; n++)
	{
		p->heizung[n].id = 255; // Kennzeichnet ein leeres Feld
		p->heizung[n].waermebedarf = 0;
	}
}

inline void zentralheizungspumpe_timer_handler(device_data_zentralheizungspumpe *p, uint8_t zyklus) {}

void zentralheizungspumpe_can_callback(device_data_zentralheizungspumpe *p, const canix_frame *frame)
{
	uint8_t mittlererWaermebedarf = 0;

	if(frame->data[1] == HCAN_HES_HEIZUNG_WAERMEBEDARF_INFO)
	{
		waermebedarfSpeichern(p, frame->data[2], frame->data[3]); //id, waermebedarf

		mittlererWaermebedarf = getMittlererWaermebedarf(p);
			
		if (p->state == 0)
		{
			if (mittlererWaermebedarf >= EINSCHALT_SCHWELLE)
			{
				p->state = 1;
				sendMessage(p, HCAN_HES_POWER_GROUP_ON); // Heizkoerperumwaelzpumpe einschalten
			}
		}
		else // p->state == 1:
		{
			if (mittlererWaermebedarf <= AUSSCHALT_SCHWELLE)
			{
				p->state = 0;
				sendMessage(p, HCAN_HES_POWER_GROUP_OFF); // Heizkoerperumwaelzpumpe ausschalten
			}
		}

		// die regelmaessigen Update-Infos versenden (Achtung: Mutlicast
		// ist trotzdem CONTROL !)
		
		if (p->update_counter == 0)
		{
			p->update_counter = ZENTRALHEIZUNGSPUMPE_UPDATE_INTERVAL;

			if (p->state) 	sendMessage(p, HCAN_HES_POWER_GROUP_ON);
			else 		sendMessage(p, HCAN_HES_POWER_GROUP_OFF);
		}

		p->update_counter--;
	}
}

static inline void waermebedarfSpeichern(device_data_zentralheizungspumpe *p, uint8_t id, uint8_t waermebedarf)
{
	uint8_t n;

	// 1. Durchgang: id finden und ggf. den Waermebedarf speichern:
	for (n=0; n<MAX_ANZ_HEIZUNGEN; n++)
	{
		if (p->heizung[n].id == id)   // id gefunden
		{
			p->heizung[n].waermebedarf = waermebedarf;
			return; // Eintrag geschrieben
		}
	}

	// 2. Durchgang, falls die id noch nicht angelegt ist:
	for (n=0; n<MAX_ANZ_HEIZUNGEN; n++)
	{
		if (p->heizung[n].id == 255) // erstes noch ungenutztes Feld (id noch leer)
		{
			p->heizung[n].id = id; // fuer Ersteintrag mit id
			p->heizung[n].waermebedarf = waermebedarf;
			return; // Eintrag erzeugt und geschrieben
		}
	}
}

static inline uint8_t getMittlererWaermebedarf(device_data_zentralheizungspumpe *p)
{
	uint16_t gesamtWaermebedarf = 0;
	uint8_t i = 0;
	uint8_t n = 0;
	for(n=0; n<MAX_ANZ_HEIZUNGEN; n++)
	{
		if (p->heizung[n].waermebedarf > 0)
		{
			gesamtWaermebedarf += p->heizung[n].waermebedarf;
			i++;
		}
	}

	if(gesamtWaermebedarf == 0)
	{
		return 0;
	}
	else
	{
		uint8_t mittlereWaermebedarf = gesamtWaermebedarf / i;
		canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("WB_%d/%i=%d"), gesamtWaermebedarf, i, mittlereWaermebedarf);
		return mittlereWaermebedarf;
	}
}

static inline void sendMessage(device_data_zentralheizungspumpe *p, uint8_t zustandToSend)
{
	canix_frame message;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = zustandToSend; // Zentralheizungspumpe schalten
	message.data[2] = p->config.gruppe;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}
