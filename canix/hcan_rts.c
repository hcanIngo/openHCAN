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

#include <hcan_rts.h>
#include <canix.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <hcan.h>
#include <rtc.h>
#include <stdlib.h>

#ifndef CANIX_RTS
#error "CANIX_RTS muss definiert werden"
#endif

void canix_SFP_RTS_handler(const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_RTS;

	switch (frame->data[1])
	{
		case HCAN_RTS_TIME_INFO :
			{
				/*
				 * Dieses Frame wird normalerweise auf der HCAN_MULTICAST_RCS
				 * Adresse empfangen:
				 *
				 * data[2] : level
				 * data[3] : day of week
				 * data[4] : hour
				 * data[5] : minute
				 * data[6] : second
				 *
				 */

				int32_t our_time, their_time;

				// RTC Wartezaehler zuruecksetzen 
				canix_rtc_clock.last_date_update = 0;

				// Nun schauen, ob unsere Zeit zu stark von der
				// empfangenen Zeit abweicht
				our_time = canix_rtc_clock.second +
					canix_rtc_clock.minute * 60 +
					canix_rtc_clock.hour * 3600;
				their_time = frame->data[6] +
					frame->data[5] * 60 +
					frame->data[4] * 3600;

				// Falls sie zu stark abweicht, dann neu setzen
				if (abs(their_time - our_time) > 3)
				{
					canix_rtc_clock.second = frame->data[6];
					canix_rtc_clock.minute = frame->data[5];
					canix_rtc_clock.hour   = frame->data[4];
					canix_rtc_clock.day_of_week = frame->data[3];
				}
			}
			break;

		case HCAN_RTS_DATE_INFO :
			{
				// Das emfpangene Dateum uebernehmen wir auf jeden Fall
				canix_rtc_clock.day_of_month = frame->data[3];
				canix_rtc_clock.month_of_year = frame->data[4];
				canix_rtc_clock.year = frame->data[5];
			}
			break;

		case HCAN_RTS_TIME_REQUEST : 
			{
				// Achtung: Diese Anfrage beantworten wir nur, wenn sie
				// an unsere Unicast-Adresse ging
				if (frame->dst == canix_selfaddr())
				{
					answer.data[1] = HCAN_RTS_TIME_INFO;
					answer.data[2] = 0xff; // undefined Level
					answer.data[3] = canix_rtc_clock.day_of_week;
					answer.data[4] = canix_rtc_clock.hour;
					answer.data[5] = canix_rtc_clock.minute;
					answer.data[6] = canix_rtc_clock.second;
					answer.size = 7;
					canix_frame_send(&answer);
				}
			}
			break;

		case HCAN_RTS_DATE_REQUEST :
			{
				// Achtung: Diese Anfrage beantworten wir nur, wenn sie
				// an unsere Unicast-Adresse ging
				if (frame->dst == canix_selfaddr())
				{
					answer.data[1] = HCAN_RTS_DATE_INFO;
					answer.data[2] = 0xff; // undefined Level
					answer.data[3] = canix_rtc_clock.day_of_month;
					answer.data[4] = canix_rtc_clock.month_of_year;
					answer.data[5] = canix_rtc_clock.year;
					answer.size = 6;
					canix_frame_send(&answer);
				}
			}
			break;

	};
}

