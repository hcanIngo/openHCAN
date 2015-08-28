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
#include <timer.h>
#include <darlingtonoutput.h>
#include <onewire.h>

void hauselektrik_callback(const canix_frame *frame)
{
	uint8_t i;

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

	for (i = 0; i < MAX_PDEVICE_DATA; i++)
	{
		uint8_t *p = (uint8_t *) pdevice_data[i];
		if (p)
		{
			switch (*p)
			{
				case EDS_analogComparator_BLOCK_ID:
					analogComparator_can_callback ( (device_data_analogComparator*) p, frame);
					break;
				case EDS_powerportAutomat_BLOCK_ID:
					powerportAutomat_can_callback ( (device_data_powerportAutomat*) p, frame);
					break;
				case EDS_rolladenAutomat_BLOCK_ID:
					rolladenAutomat_can_callback ( (device_data_rolladenAutomat*) p, frame);
					break;
				case EDS_dunstabzugport_BLOCK_ID: 
					dunstabzugport_can_callback ( (device_data_dunstabzugport*) p, frame); 
					break;
				case EDS_heizung_BLOCK_ID: 
					heizung_can_callback ( (device_data_heizung*) p, frame); 
					break;
				case EDS_lichtzone_BLOCK_ID: 
					lichtzone_can_callback ( (device_data_lichtzone*) p, frame); 
					break;
				case EDS_multitaster_BLOCK_ID:
					multitaster_can_callback ( (device_data_multitaster *)p, frame);
					break;
				case EDS_powerport_BLOCK_ID: 
					powerport_can_callback ( (device_data_powerport*) p, frame); 
					break;
				case EDS_reedkontakt_BLOCK_ID: 
					reedkontakt_can_callback ( (device_data_reedkontakt*) p, frame); 
					break;
				case EDS_rolladen_BLOCK_ID: 
					rolladen_can_callback ( (device_data_rolladen*) p, frame); 
					break;
				case EDS_rolladenSchlitzpos_BLOCK_ID:
					rolladenSchlitzpos_can_callback ( (device_data_rolladenSchlitzpos*) p, frame);
					break;
				case EDS_schalter_BLOCK_ID: 
					schalter_can_callback ( (device_data_schalter*) p, frame); 
					break;
				case EDS_zentralheizungspumpe_BLOCK_ID:
					zentralheizungspumpe_can_callback ( (device_data_zentralheizungspumpe*) p, frame);
					break;
				case EDS_tastdimmer_BLOCK_ID: 
					tastdimmer_can_callback ( (device_data_tastdimmer*) p, frame); 
					break;
				case EDS_tempsensor_BLOCK_ID: 
					tempsensor_can_callback ( (device_data_tempsensor*) p, frame); 
					break;
				case EDS_zeitschaltuhr_BLOCK_ID: 
					zeitschaltuhr_can_callback ( (device_data_zeitschaltuhr*) p, frame); 
					break;				
				case EDS_zeitzone_BLOCK_ID: 
					zeitzone_can_callback ( (device_data_zeitzone*) p, frame); 
					break;
			}
		}
	}

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

	// setup timeservice can frame handler
	canix_reg_frame_callback(timeservice_can_callback, HCAN_MULTICAST_INFO,
			HCAN_PROTO_SFP, HCAN_SRV_RTS);

	canix_reg_frame_callback(dcf77_receiver_can_callback, HCAN_MULTICAST_INFO,
	 		HCAN_PROTO_SFP, HCAN_SRV_RTS);
	
	devices_load_config();
	
	canix_reg_rtc_callback(timer_handler);
	canix_reg_idle_callback(idle_handler);
	
	sendSchalterStateQuery(); // alle Schalter am CAN-Bus abfragen
	canix_mainloop();
	return 0;
}

