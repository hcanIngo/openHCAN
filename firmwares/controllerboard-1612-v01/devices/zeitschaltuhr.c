#include "zeitschaltuhr.h"
#include <canix/syslog.h>
#include <darlingtonoutput.h>
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

#include <darlingtonoutput.h>
#include <devices.h>

static inline void sendMessage(device_data_zeitschaltuhr *p, uint8_t zustandToSend);

void zeitschaltuhr_init(device_data_zeitschaltuhr *p, eds_block_p it)
{
	p->state = 0;
	p->update_counter = 1 + (p->config.power_gruppe & 0x0f);
	
	/* Wir gehen davon aus, das die Automatik aktiviert ist. Ist ein Schalter konfiguriert,  
	 * so wird er auch seinen Zustand melden, wenn irgendein C1612-Board einen Reboot durchfuehrt. */
	p->automatikEin = true;
}

inline void zeitschaltuhr_timer_handler(device_data_zeitschaltuhr *p, uint8_t zyklus)
{
	if (zyklus != 1) return; // 1s-Zyklus verwendet

	uint8_t i,found;
	uint8_t* zeitzonen_ids;

	if(!p->automatikEin) return; // kein Automatikbetrieb
	
	zeitzonen_ids = &(p->config.zeitzone0_id);

	// Wenn State == 0, ueber alle Zeitzonen iterieren und, falls eine
	// passende gefunden wird, die Gruppe einschalten; im else Block
	// das Gegenteil: wenn keine gefunden wurde und State == 1, dann
	// ausschalten.
	
	if (p->state == 0)
	{
		for (i = 0; i < 8; i++)
		{
			// wenn eine Zeitzone beginnt

			if ((p->state == 0) && zeitzone_matches(zeitzonen_ids[i]))
			{
				// Beginn einer Zeitzone
				p->state = 1;
				sendMessage(p, HCAN_HES_POWER_GROUP_ON); // Power Gruppe einschalten
				return;
			}
		}
	}
	else 
	{
		// p->state == 1:
		
		found = 0;

		for (i = 0; i < 8; i++)
		{
			if ((p->state == 1) && zeitzone_matches(zeitzonen_ids[i]))
				found = 1;
		}

		if (found == 0)
		{
			// in der oberen Iteration ueber die Zeitzonen keine passende
			// Zeitzone gefunden; falls der Port an ist, diesen jetzt 
			// ausschalten:
			p->state = 0;
			sendMessage(p, HCAN_HES_POWER_GROUP_OFF);
			return;
		}
	}

	// die regelmaessigen Update-Infos versenden (Achtung: Mutlicast
	// ist trotzdem CONTROL !)
	
	if (p->update_counter == 0)
	{
		p->update_counter = ZEITSCHALTUHR_UPDATE_INTERVAL;

		if (p->state) 	sendMessage(p, HCAN_HES_POWER_GROUP_ON);
		else 		sendMessage(p, HCAN_HES_POWER_GROUP_OFF);
	}

	p->update_counter--;
}

void zeitschaltuhr_can_callback(device_data_zeitschaltuhr *p, const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_SCHALTER_ON :
			if (frame->data[2] == p->config.automatikEin_schalter_gruppe)
			{
				p->automatikEin = true;
			}
			break;

		case HCAN_HES_SCHALTER_OFF :
			if (frame->data[2] == p->config.automatikEin_schalter_gruppe)
			{
				p->automatikEin = false;
				sendMessage(p, HCAN_HES_POWER_GROUP_OFF); // Automatik deaktiviert -> sofort ausschalten
			}
			break;
	}
}

static inline void sendMessage(device_data_zeitschaltuhr *p, uint8_t zustandToSend)
{
	canix_frame message;
	
		message.src = canix_selfaddr();
		message.dst = HCAN_MULTICAST_CONTROL;
		message.proto = HCAN_PROTO_SFP;
		message.data[0] = HCAN_SRV_HES;
	message.data[1] = zustandToSend;
		message.data[2] = p->config.power_gruppe;
		message.size    = 3;
		canix_frame_send(&message);
}
