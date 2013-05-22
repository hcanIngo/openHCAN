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
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 */

#include <avr/wdt.h>
#include <canix/syslog.h>
#include <canix/canix.h>
#include <hcan.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifndef CANIX_SYSLOG
#error "CANIX_SYSLOG muss definiert werden"
#endif

uint8_t canix_debug_level = 0;

void canix_set_syslog_level(uint8_t level)
{
	canix_debug_level = level;
}

void canix_syslog_P(uint8_t prio, const char *fmt, ...)
{
	if (canix_debug_level >= prio)
	{
		canix_frame message;
		uint8_t s_counter, payload_counter;
		char s[128];
		va_list ap;

		va_start(ap, fmt);
		vsnprintf_P(s, sizeof(s)-2, fmt, ap);
		va_end(ap);

		// An das Ende ein Retrun Zeichen anhaengen; somit kann die
		// empfangende Seite erkennen, wann die Meldung beendet ist.
		//
		// Hier benoetigen wir keinen Laengen-Check, da wir oben
		// sizeof(s)-2 verwendet haben.
		strcat(s,"\n");

		message.src = canix_selfaddr();
		message.dst = HCAN_MULTICAST_SYSLOG;
		message.proto = HCAN_PROTO_SYSLOG;

		s_counter = 0;

		message.data[0] = prio;
		payload_counter = 1;

		while (s_counter < strlen(s))
		{
			wdt_reset();

			while ((payload_counter < 8) && (s_counter < strlen(s)))
			{
				message.data[payload_counter++] = s[s_counter++];
			}

			message.size = payload_counter;
			payload_counter = 0;
			canix_frame_send(&message);
		}

		canix_sleep_100th(1);
	}
}

