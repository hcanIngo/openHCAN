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
 *  (c) 2017 Ingo Lages, i dot Lages (at) gmx (dot) de
 */
#include "../hcan4mqttpc/mqttHcan.h"

#include "../include/hcan-def.h"
#include <linux/can.h>
#include <string.h>
#include "../hcan4mqttpc/mqttClient.h"

static bool is(const char *topic, const char *key)
{
	if (topic == NULL)
		return 0;

	return 0 == strncmp(topic, key, strlen(key));
}

int getMessage4cb (char * msg, struct can_frame * msg4cb)
{
	char* token = strtok(msg, "/");
	if (NULL != token)
	{
		if (is(token, "ON")) // setPowerON
		{
			msg4cb->data[1] = HCAN_HES_POWER_GROUP_ON;
			token = strtok(NULL, "/"); if(NULL == token) return 0;
			msg4cb->data[2] = atoi(token); // Gruppe
			return 3; // Anzahl der Daten-Bytes des Frames
		}
		else if (is(token, "OFF")) // setPowOFF
		{
			msg4cb->data[1] = HCAN_HES_POWER_GROUP_OFF;
			token = strtok(NULL, "/"); if(NULL == token) return 0;
			msg4cb->data[2] = atoi(token); // Gruppe
			return 3; // Anzahl der Daten-Bytes des Frames
		}

		else if (is(token, "Ro")) // Rollladen
		{
			msg4cb->data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
			token = strtok(NULL, "/"); if(NULL == token) return 0;
			msg4cb->data[2] = atoi(token); // Gruppe
			token = strtok(NULL, "/"); if(NULL == token) return 0;
			uint8_t pos = atoi(token);
			switch (pos)
			{
				case 200:
				case 201:
				case 202:
					msg4cb->data[3] = pos;  // UP, DOWN, STOP
					return 4;
				default:
					msg4cb->data[3] = 100 - pos; // invertieren
					return 4;
			}
		}

		else if (is(token, "H")) // Heizung bzw. Heizmode
		{
			token = strtok(NULL, "/"); if(NULL == token) return 0;
			if (is(token, "aus"))
			{
				msg4cb->data[1] = HCAN_HES_HEIZUNG_SET_MODE_OFF;
				token = strtok(NULL, "/"); if(NULL == token) return 0;
				msg4cb->data[2] = atoi(token); // Heizungs-ID
				return 3;
			}
			else if (is(token, "therm"))
			{
				msg4cb->data[1] = HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;
				token = strtok(NULL, "/"); if(NULL == token) return 0;
				msg4cb->data[2] = atoi(token); // Heizungs-ID
				token = strtok(NULL, "/"); if(NULL == token) return 0;
				uint16_t temp = (uint16_t) (16.0f * atof(token));
				msg4cb->data[3] = temp>>8; // temp_hi : Soll-Temperatur (MSB)
				msg4cb->data[4] = temp; // temp_li : Soll-Temperatur (LSB)

				uint16_t dauer = 0; // 0 = ohne Zeitangabe unbegrenzt (falls der naechste Token fehlt)
				token = strtok(NULL, "/");
				if(NULL != token) dauer = (uint16_t) (3600.0f * atof(token)); // hours -> secs
				msg4cb->data[5] = dauer>>8; // duration_hi : Restdauer (MSB), 0 = unbegrenzt
				msg4cb->data[6] = dauer; // duration_lo : Restdauer (LSB)
				return 7;
			}
			else if (is(token, "auto"))
			{
				msg4cb->data[1] = HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK;
				token = strtok(NULL, "/"); if(NULL == token) return 0;
				msg4cb->data[2] = atoi(token); // Heizungs-ID
				return 3;
			}
		}

		else if (is(token, "RQ")) // Request Device-States
		{
			msg4cb->data[1] = HCAN_HES_DEVICE_STATES_REQUEST;
			return 2;
		}
	}

	return 0; // kein Topic (keine Message) fuer den CAN-Bus
}

static inline uint8_t get_proto(uint32_t extid)
{
	return (extid >> 20) & 0x07;
}

static inline uint8_t isItTopic4Broker(uint8_t msg4broker)
{
	switch (msg4broker)
	{
		case HCAN_HES_POWER_GROUP_STATE_REPLAY:
		case HCAN_HES_POWER_GROUP_STATE_INFO:

		case HCAN_HES_ROLLADEN_POSITION_REPLAY:
		case HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO:

		case HCAN_HES_1WIRE_TEMPERATURE:
		case HCAN_HES_1WIRE_TEMPERATURE_REPLAY:

		//case HCAN_HES_REEDKONTAKT_OFFEN:
		case HCAN_HES_REEDKONTAKT_STATE_REPLAY:
		case HCAN_HES_REEDKONTAKT_STATE_CHANGE:

		case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS:
		case HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS:
		case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS:
		case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS:

		case HCAN_HES_HEIZUNG_TIST_REPLAY:
			return 1; // Topic fuer Broker

		default:
			return 0; // nicht fuer Broker
	}
}

size_t catHesTopic4Broker(char * str, const struct can_frame * cf)
{
	float temp = 0;
	float dauer = 0;
	const size_t maxSize = 30;
	switch (cf->data[1])
	{
		case HCAN_HES_POWER_GROUP_STATE_REPLAY:
		case HCAN_HES_POWER_GROUP_STATE_INFO:
			return snprintf(str, maxSize, "%d/%s", cf->data[2], cf->data[3] ? "ON":"OFF");

		case HCAN_HES_ROLLADEN_POSITION_REPLAY:
		case HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO:
			switch (cf->data[3])
			{
				case 200:
					return snprintf(str, maxSize, "%d/Ro/%s", cf->data[2], "DOWN");
				case 201:
					return snprintf(str, maxSize, "%d/Ro/%s", cf->data[2], "UP");
				case 202:
					return snprintf(str, maxSize, "%d/Ro/%s", cf->data[2], "STOP");
				default:
					return snprintf(str, maxSize, "%d/Ro/%d", cf->data[2], 100 - cf->data[3]); // invertieren
			}
			break;

		case HCAN_HES_1WIRE_TEMPERATURE:
		case HCAN_HES_1WIRE_TEMPERATURE_REPLAY:
			temp =	(((signed char)cf->data[3]<<8)|cf->data[4]) / 16.0; // Temperaturwert
			return snprintf(str, maxSize, "%d/T/%.1f", cf->data[2], temp); // mit einer Nachkommastelle

		/*case HCAN_HES_REEDKONTAKT_OFFEN:
			return snprintf(str, maxSize, "%d/OPEN", cf->data[2]);*/
		case HCAN_HES_REEDKONTAKT_STATE_REPLAY:
		case HCAN_HES_REEDKONTAKT_STATE_CHANGE:
			return snprintf(str, maxSize, "%d/%s", cf->data[2], cf->data[3] ? "OPEN":"CLOSED");


		case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS:
			return snprintf(str, maxSize, "%d/H/aus", cf->data[2]);
		case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS:
			temp =	(float)((cf->data[3]<<8)|cf->data[4]) / 16.0; // Temperaturwert
			dauer =	(float)((cf->data[5]<<8)|cf->data[6]) / 3600.0; // Dauer
			return snprintf(str, maxSize, "%d/H/therm/%.1f/%.1f", cf->data[2], temp, dauer);
		case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS:
			temp =	((cf->data[3]<<8)|cf->data[4]) / 16.0; // Temperaturwert
			return snprintf(str, maxSize, "%d/H/auto/%.1f", cf->data[2], temp);
/*
		case HCAN_HES_HEIZUNG_TIST_REPLAY:
			return snprintf(str, maxSize, "HES/HeizTemp");  // TODO, da ungenutzt in OH2
*/
		default:
			TRACE("ERROR: canData[1]=%d\n", cf->data[1]);
			return 0;
	}
}

uint8_t msgOfInterest4broker(const struct can_frame * cf)
{
	if (2 > cf->can_dlc)
	{
		TRACE("can_dlc=%d NG\n", cf->can_dlc);
		return 0;
	}

	uint8_t msgOfInterest =    (1 == get_proto(cf->can_id)) // proto = 1 (=SFP) ?
							&& (5 == cf->data[0]) 		 // 5=HES  von SFP
							&& isItTopic4Broker(cf->data[1]);
	if (!msgOfInterest)
		TRACE("not for broker cf[0]=%d\n", cf->data[0]);

	return msgOfInterest;
}
