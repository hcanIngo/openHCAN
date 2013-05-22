#include <dunstabzugport.h>
#include <canix/syslog.h>
#include <darlingtonoutput.h>
#include <hcan.h>

void dunstabzugport_init(device_data_dunstabzugport *p, eds_block_p it)
{
	p->counter = 0;
	p->state_reed0 = 0;
	p->state_reed1 = 0;
	p->state_dunst = 0;
	darlingtonoutput_setpin(p->config.port,0);
}

void dunstabzugport_can_callback(device_data_dunstabzugport *p, 
		const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;

	switch (frame->data[1])
	{
		case HCAN_HES_REEDKONTAKT_OFFEN :
			if (p->config.reed_gruppe0 == frame->data[2])
			{
				p->state_reed0 = 1;

				// fuer die naechsten 7 Sekunden ist die Dunstabzugshaube an
				p->counter = 7;
				p->state_dunst = 1;
				darlingtonoutput_setpin(p->config.port,1);
				break;
			}

			if (p->config.reed_gruppe1 == frame->data[2])
			{
				p->state_reed1 = 1;

				// fuer die naechsten 7 Sekunden ist die Dunstabzugshaube an
				p->counter = 7;
				p->state_dunst = 1;
				darlingtonoutput_setpin(p->config.port,1);
				break;
			}
			break;

		case HCAN_HES_REEDKONTAKT_STATE_CHANGE : 
			if (p->config.reed_gruppe0 == frame->data[2])
			{
				if (frame->data[3]) // Kontakt ist geoeffnet worden
				{
					p->counter = 7;
					p->state_dunst = 1;
					darlingtonoutput_setpin(p->config.port,1);
				}
				else if (! p->state_reed1)
				{
					// Kontakt ist zugegangen UND reed1 ist nicht offen;
					// damit muss nicht erst auf den Timeout gewartet werden!
					p->counter = 0;
					p->state_dunst = 0;
					darlingtonoutput_setpin(p->config.port,0);
				}
			}

			if (p->config.reed_gruppe1 == frame->data[2])
			{
				if (frame->data[3]) // Kontakt ist geoeffnet worden
				{
					p->counter = 7;
					p->state_dunst = 1;
					darlingtonoutput_setpin(p->config.port,1);
				}
				else if (! p->state_reed0)
				{
					// Kontakt ist zugegangen UND reed0 ist nicht offen;
					// damit muss nicht erst auf den Timeout gewartet werden!
					p->counter = 0;
					p->state_dunst = 0;
					darlingtonoutput_setpin(p->config.port,0);
				}
			}
			break;
			
		case HCAN_HES_TASTER_DOWN :
			if (p->config.freischalt_taster == frame->data[2])
			{
				// Den Dunstabzug fuer X Minuten freischalten:
				p->counter = p->config.freischalt_zeit * 60;
				p->state_dunst = 1;
				darlingtonoutput_setpin(p->config.port,1);
			}
			break;
	}
}

void dunstabzugport_timer_handler(device_data_dunstabzugport *p)
{
	if (p->counter > 0)
	{
		p->counter--;

		// Wenn die Zeit abgelaufen ist, den Dunstabzug ausschalten
		if (p->counter == 0)
		{
			p->state_dunst = 0;
			darlingtonoutput_setpin(p->config.port,0);
		}
	}
}

