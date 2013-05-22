#include <timeservice.h>
#include <canix/syslog.h>
#include <canix/rtc.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include <devices.h>
#include <hcan_multicast.h>

// wird alle 1 sec aufgerufen, fuer jede timeservice-Instanz einmal
void timeservice_timer_handler(device_data_timeservice *p)
{

	// Increment our counter
	if (p->last_time_frame_received < 255)
		p->last_time_frame_received++;

	// Check, if the time we wait for an time frame has elapsed
	if (p->last_time_frame_received >= p->config.takeover_time)
	{
		// time has elapsed: We did not receive any time frame
		// which had a lower level than us in the given takeover_time
		//
		// So, we will become the time master.
		 
		if (p->last_time_frame_sent++ == p->config.interval)
		{
			canix_frame message;

			// The end of the interval has been reached, so send a new
			// time frame because we are the master !
			p->last_time_frame_sent = 0;

			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_INFO;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_RTS;
			message.data[1] = HCAN_RTS_TIME_INFO;
			message.data[2] = p->config.level;
			message.data[3] = canix_rtc_clock.day_of_week;
			message.data[4] = canix_rtc_clock.hour;
			message.data[5] = canix_rtc_clock.minute;
			message.data[6] = canix_rtc_clock.second;
			message.size = 7;
			canix_frame_send(&message);

			// ... and send the date
			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_INFO;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_RTS;
			message.data[1] = HCAN_RTS_DATE_INFO;
			message.data[2] = p->config.level;
			message.data[3] = canix_rtc_clock.day_of_month;
			message.data[4] = canix_rtc_clock.month_of_year;
			message.data[5] = canix_rtc_clock.year;
			message.size = 6;
			canix_frame_send(&message);
		}
	}
}


void timeservice_can_callback(const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;


	switch (frame->data[1])
	{
		case HCAN_RTS_TIME_INFO : 
			{
				uint8_t i;
				for (i = 0; i < MAX_PDEVICE_DATA; i++)
				{
					device_data_timeservice *p = (device_data_timeservice *) 
						pdevice_data[i];

					// if this is a timeservice instance:
					if (p && p->type == EDS_timeservice_BLOCK_ID)
					{
						// now check, if the level is lower than our
						// level:
						
						if (frame->data[2] < p->config.level)
						{
							// yes, there is a master outside in the CAN
							// universe, so reset our counter:

							p->last_time_frame_received = 0;
						}
					}
				}
			}
			break;
	}
}

