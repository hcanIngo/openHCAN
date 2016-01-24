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

#include <devices.h>
#include "onewire.h"
#include <timer.h>
#include <timeservice.h>
#include <dcf77_receiver.h>
#include <darlingtonoutput.h>


void controllerboard_callback(const canix_frame *frame)
{
	// Dieser Handler wird fuer alle Destination Adressen ausgefuehrt
	// daher muss gefiltert werden, was uns betrifft und was nicht:
	if ( (frame->data[0] != HCAN_SRV_HES))
		// Diese Message ist nicht interessant, daher ignorieren
		return;

	if ((frame->dst != canix_selfaddr()) && (
				(frame->dst != HCAN_MULTICAST_CONTROL) &&
				(frame->dst != HCAN_MULTICAST_INFO)))
		// Diese Message ist nicht interessant, daher ignorieren
		return;

	switch (frame->data[1])
	{
		case HCAN_HES_CONFIG_RELOAD :
			devices_load_config();
			return;
		case HCAN_HES_CONFIG_RAM_USAGE_REQUEST :
			{
				canix_frame answer;

				answer.src = canix_selfaddr();
				answer.dst = frame->src;
				answer.proto = HCAN_PROTO_SFP;
				answer.data[0] = HCAN_SRV_HES;
				answer.data[1] = HCAN_HES_CONFIG_RAM_USAGE_REPLAY;
				answer.data[2] = device_data_ram_usage >> 8;
				answer.data[3] = device_data_ram_usage;
				answer.size = 4;

				canix_frame_send(&answer);
			}
			return;
		case HCAN_HES_1WIRE_DISCOVER :
			{
				uint8_t old = ow_getpin();
				// frame->data[2] ist die IO Pin ID
				ow_setpin(frame->data[2]);
				ow_discover(frame);
				ow_setpin(old);
			}
			break;
	}
}

static inline void sendSchalterStateQuery(void)
{
	canix_frame message;
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_SCHALTER_STATE_QUERY;
	message.size = 2;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}

int main(void)
{
	darlingtonoutput_init();
	canix_init();

	// Haus-Elektrik Service Handler installieren
	canix_reg_frame_callback(hauselektrik_callback, -1, 
			HCAN_PROTO_SFP, HCAN_SRV_HES);

	canix_reg_frame_callback(controllerboard_callback, -1,
			HCAN_PROTO_SFP, HCAN_SRV_HES);


	// setup timeservice can frame handler
	canix_reg_frame_callback(timeservice_can_callback, HCAN_MULTICAST_INFO,
			HCAN_PROTO_SFP, HCAN_SRV_RTS);

	// canix_reg_frame_callback(dcf77_receiver_can_callback, HCAN_MULTICAST_INFO,
	// 		HCAN_PROTO_SFP, HCAN_SRV_RTS);

	devices_load_config();
	
	canix_reg_rtc_callback(timer_handler);
	canix_reg_idle_callback(idle_handler);
	
	sendSchalterStateQuery(); // alle Schalter am CAN-Bus abfragen
	canix_mainloop();
	return 0;
}

