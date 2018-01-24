#include "../../controllerboard/devices/tastdimmer.h"

#include <canix/syslog.h>
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

#include "../../controllerboard/darlingtonoutput.h"

void tastdimmer_init(device_data_tastdimmer *p, eds_block_p it) {}

uint8_t tastdimmer_is_in_group(const device_data_tastdimmer *p, uint8_t group)
{
	uint8_t i;

	// die 255 ist der Ersatzwert und wird ignoriert!
	if (group == 255)
		return 0;
	
	for (i = 0; i < MAX_TASTDIMMER_GROUPS; i++)
	{
		uint8_t *gruppen = (uint8_t *) &(p->config.gruppe0);
		if (gruppen[i] == group)
		{
			return 1;
		}
	}

	return 0;
}

void tastdimmer_can_callback(device_data_tastdimmer *p, const canix_frame *frame)
{

	if (tastdimmer_is_in_group(p, frame->data[2]))
	{
		switch (frame->data[1])
		{
			case HCAN_HES_TASTER_DOWN :
				darlingtonoutput_setpin(p->config.port, 1);
				break;
			case HCAN_HES_TASTER_UP :
				darlingtonoutput_setpin(p->config.port, 0);
				break;
		}
	}
}

inline void tastdimmer_timer_handler(device_data_tastdimmer *p, uint8_t zyklus) {}

