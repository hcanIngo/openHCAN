#include <zeitzone.h>
#include <canix/syslog.h>
#include <darlingtonoutput.h>
#include <hcan.h>

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <canix/tools.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include <devices.h>
#include <hcan_multicast.h>

void zeitzone_init(device_data_zeitzone *p, eds_block_p it)
{
	p->it = it;
}

/**
 * Prueft, ob die gegebene Zeitzone passt, d.h. ob die aktuelle Zeit in
 * der gegebenen Zeitzone liegt.
 */
uint8_t zeitzone_matches(uint8_t zeitzone_id)
{
	uint8_t i;

	for (i = 0; i < MAX_PDEVICE_DATA; i++)
	{
		wdt_reset();

		device_data_zeitzone *p = (device_data_zeitzone *) 
			pdevice_data[i];
		if (p && 
				p->type == EDS_zeitzone_BLOCK_ID &&
				p->config.gruppe == zeitzone_id)
		{
			return time_matches(p->config.from_hour, p->config.from_minute,
					p->config.to_hour, p->config.to_minute, 
					p->config.day_pattern);
		}
	}

	return 0;
}

void zeitzone_can_callback(device_data_zeitzone *p,
		const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;

	switch (frame->data[1])
	{
		case HCAN_HES_ZEITZONE_DETAILS_REQUEST :
			if (frame->data[2] == p->config.gruppe)
			{
				answer.data[1] = HCAN_HES_ZEITZONE_DETAILS_REPLAY;
				answer.data[2] = p->config.gruppe;
				answer.data[3] = p->config.day_pattern;
				answer.data[4] = p->config.from_hour;
				answer.data[5] = p->config.from_minute;
				answer.data[6] = p->config.to_hour;
				answer.data[7] = p->config.to_minute;
				answer.size = 8;
				canix_frame_send_with_prio(&answer, HCAN_PRIO_LOW);
			}
			break;
		case HCAN_HES_ZEITZONE_WRITE_DETAILS :
			if (frame->data[2] == p->config.gruppe)
			{
				// EEPROM/EDS Validierungstests:
				if ((p->it != 0) && 
						(EDS_BLOCK_TYPE(p->it) == EDS_zeitzone_BLOCK_ID))
				{
					// neue Daten ins EEPROM schreiben; dazu:
					//  2 Bytes Offset fuer Typ + Size
					//  1 Byte  Offset fuer Ueberspringen der Gruppe
					
					// day_pattern
					p->config.day_pattern = frame->data[3];
					eeprom_write_byte(p->it + 3, frame->data[3]);

					// from_hour
					p->config.from_hour = frame->data[4];
					eeprom_write_byte(p->it + 4, frame->data[4]);

					// from_minute
					p->config.from_minute = frame->data[5];
					eeprom_write_byte(p->it + 5, frame->data[5]);

					// to_hour
					p->config.to_hour = frame->data[6];
					eeprom_write_byte(p->it + 6, frame->data[6]);

					// to_minute
					p->config.to_minute = frame->data[7];
					eeprom_write_byte(p->it + 7, frame->data[7]);

					answer.data[1] = HCAN_HES_ZEITZONE_WRITE_DETAILS_ACK;
					answer.data[2] = p->config.gruppe;
					answer.size = 3;
					canix_frame_send_with_prio(&answer, HCAN_PRIO_LOW);
				}
			}
			break;
		case HCAN_HES_ZEITZONE_TEST_MATCH_REQUERST :
			if (frame->data[2] == p->config.gruppe)
			{
				answer.data[1] = HCAN_HES_ZEITZONE_TEST_MATCH_REPLAY;
				answer.data[2] = p->config.gruppe;
				answer.data[3] = zeitzone_matches(p->config.gruppe) ? 1 : 0;
				answer.size = 4;
				canix_frame_send_with_prio(&answer, HCAN_PRIO_LOW);
			}
			break;
	}
}

