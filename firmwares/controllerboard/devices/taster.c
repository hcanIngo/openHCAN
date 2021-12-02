#include "../../controllerboard/devices/taster.h"

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

#include "../../controllerboard/darlingtonoutput.h"
#include "../../controllerboard/input.h"

void taster_init(device_data_taster *p, eds_block_p it) {}

inline void taster_timer_handler(device_data_taster *p, uint8_t zyklus)
{
	if (zyklus != 100) return; // 100tel-Sekunden-Zyklus verwendet

	canix_frame message;
	uint8_t time;

	// Wenn Taste gedrueckt ist, dann ist der Pin 0, ansonsten 1
	uint8_t status = !inputport_read(1, p->config.port);

	// Message schon mal vorbereiten:
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	// message.data[1] wird unten ausgefuellt
	message.data[2] = p->config.gruppe;
	// message.data[3] wird unten ausgefuellt
	message.size = 4;

	if (status) // gedrueckt
	{
		// Entprellschutz:
		if (p->pressed < 255)
			p->pressed++;
	}
	else
	{
		// Wenn Taster-Down schon gesendet wurde,
		// dann ein Taster-Up Event senden:
		if (p->pressed > 3)
		{
			// ...und kein Flankenbetrieb konfiguriert ist:
			if (p->config.feature &
					 ((1<<FEATURE_TASTER_ROLLADEN_ZU)
					| (1<<FEATURE_TASTER_ROLLADEN_AUF)) )
			{
				message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
				message.data[3] = 222; // "Taster up" im Rolladen 2-Taster-Betrieb
				canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
			}
			else if ( !(p->config.feature &
					 ((1<<FEATURE_TASTER_POWERPORT_AN)
					| (1<<FEATURE_TASTER_POWERPORT_AUS)  )) )
			{
				message.data[1] = HCAN_HES_TASTER_UP;
				message.data[3] = p->config.port;
				canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
			}
		}

		p->pressed = 0; // Taste wurde losgelassen, also Zaehler zuruecksetzen
	}

	if (p->config.feature & (1<<FEATURE_TASTER_ENTPRELL_1S))
		time = 6;
	else
		time = 3;

	if (p->pressed == time) // mind. 30msec gedrueckt
	{
		if (p->config.feature & (1<<FEATURE_TASTER_ROLLADEN_ZU))
		{
			message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
			message.data[3] = 200; // gewuenschte Rollladenposition: Pos=200=0%=zu
		}
		else if (p->config.feature & (1<<FEATURE_TASTER_ROLLADEN_AUF))
		{
			message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
			message.data[3] = 201; // gewuenschte Rollladenposition: Pos=201=100%=auf
		}
		else if (p->config.feature & (1<<FEATURE_TASTER_POWERPORT_AN))
		{
			message.data[1] = HCAN_HES_POWER_GROUP_ON;
			message.size = 3;
		}
		else if (p->config.feature & (1<<FEATURE_TASTER_POWERPORT_AUS))
		{
			message.data[1] = HCAN_HES_POWER_GROUP_OFF;
			message.size = 3;
		}
		else // kein z.B. Rollladentaster im 2-Tasterbetrieb
		{
			message.data[1] = HCAN_HES_TASTER_DOWN;
			message.data[3] = p->config.port;
		}

		canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
	}
}

void taster_can_callback(device_data_taster *p, const canix_frame *frame) {}
