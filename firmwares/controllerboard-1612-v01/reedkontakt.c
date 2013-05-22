#include <reedkontakt.h>
#include <canix/syslog.h>
#include <darlingtonoutput.h>
#include <hcan.h>

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <hcan.h>
#include <hcan_multicast.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include <tasterinput.h>

void reedkontakt_init(device_data_reedkontakt *p, eds_block_p it)
{
	// Pseudozufaelligen Start-Wert fuer den Counter
	// aus der id ableiten:
	p->timer_counter = p->config.gruppe & 0x07;

	p->last_state = tasterport_read(p->config.port) != 0;
}

void reedkontakt_timer_handler(device_data_reedkontakt *p)
{
	// Schauen, ob der Reedkontakt offen ist
	//
	// Wenn der Magnet am Kontakt ist, ist der Reedkontakt geschlossen
	// und somit der Input Pin auf GND gezogen (interner Pullup ist
	// 20-50kOhm, da fliesst kein nennenswerter Strom).
	//
	// Wenn der Kontakt aufgeht, dann ist der Pin high und es werden
	// Meldungen versandt.
	//
	// Es gibt 2 Meldungstypen:
	// 1. Die HCAN_HES_REEDKONTAKT_STATE_CHANGE Meldung; sie wird bei
	//    Aenderungen versendet und wird verwendet, um nicht 100% sichere
	//    Entscheidungen zu treffen
	// 2. HCAN_HES_REEDKONTAKT_OFFEN Meldung; sie wird alle 10 Sekunden
	//    versendet und wird bei wichtigen Abhaengigkeiten verwendet - geht
	//    ein Paket verloren, so kann ueber einen Timeout eine Reaktion
	//    erfolgen
	//
	// Beispiel Dunstabzugsport:
	//
	// Es laeuft ein Timer fuer die HCAN_HES_REEDKONTAKT_OFFEN Meldungen; aber
	// es wird (damit der Dunstabzug schneller reagiert auch auf die 
	// HCAN_HES_REEDKONTAKT_STATE_CHANGE Meldungen reagiert.
	
	// Etwas seltsamer Vergleich; ist aber noetig, damit "wahre Werte", die
	// ungleich 1 sind, verarbeitet werden koennen!
	if ((tasterport_read(p->config.port) != 0) !=  p->last_state)
	{
		// Zustand hat sich geaendert; also Meldung versenden:

		canix_frame message;

		p->last_state = tasterport_read(p->config.port) != 0;
		message.src = canix_selfaddr();
		message.dst = HCAN_MULTICAST_INFO;
		message.proto = HCAN_PROTO_SFP;
		message.data[0] = HCAN_SRV_HES;
		message.data[1] = HCAN_HES_REEDKONTAKT_STATE_CHANGE;
		message.data[2] = p->config.gruppe;
		message.data[3] = tasterport_read(p->config.port) != 0;
		message.size    = 4;
		canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
	}

	// Hier wird jede Sekunde der Kontakt geprueft, allerdings nur alle
	// 10sec ein "Open" gesendet
	if ((p->config.modus) && (tasterport_read(p->config.port)))
	{
		if (p->timer_counter == 0)
		{
			p->timer_counter = 5;
			canix_frame message;
			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_INFO;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_HES;
			message.data[1] = HCAN_HES_REEDKONTAKT_OFFEN;
			message.data[2] = p->config.gruppe;
			message.size    = 3;
			canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);
		}
		p->timer_counter--;
	}
	else
		p->timer_counter = 0;

}

void reedkontakt_can_callback(device_data_reedkontakt *p, 
		const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;

	if (p->config.gruppe == frame->data[2])
	{
		switch (frame->data[1])
		{
			case HCAN_HES_REEDKONTAKT_STATE_QUERY:
				{
					answer.data[1] = HCAN_HES_REEDKONTAKT_STATE_REPLAY;
					answer.data[2] = frame->data[2];
					answer.data[3] = tasterport_read(p->config.port) != 0;
					answer.size    = 4;
					canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
				}
				break;
		}
	}
}

