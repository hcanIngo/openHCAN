#include "../../controllerboard/devices/powerport.h"

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


void powerport_init(device_data_powerport *p, eds_block_p it)
{
	p->mute = 0; // aktiv
	p->countDownTimer = 0; // Timer aus
}

void powerport_setup_timer(device_data_powerport *p)
{
	// Timer initialisieren: Wenn das MSB == 1, dann
	// ist der Timerwert in Sekunden, ansonsten in Minuten
	if (p->config.timer & 0x80)
	{
		// der Wert ist in Sekunden:
		p->countDownTimer = p->config.timer & 0x7f;
	}
	else
	{
		p->countDownTimer = (p->config.timer * 60);
	}
}

void set_powerport_or_timer(device_data_powerport *p, uint8_t Flanke)
{
	if( Flanke == STEIGENDE_FLANKE )
	{
		if ( p->config.feature & (1<<POWERPORT_FEATURE_AUTO_AUS) )
		{
			powerport_on(p); // sofort einschalten
			powerport_setup_timer(p); // neu laden -> Timer aktiv
		}
		else if ( p->config.feature & (1<<POWERPORT_FEATURE_VERZOEGERT_EIN) )
		{
			powerport_setup_timer(p); // neu laden -> Timer aktiv
		}
	else
		{
			powerport_on(p); // sofort einschalten
			p->countDownTimer = 0; // Timer aus
		}
	}
	else if ( Flanke == FALLENDE_FLANKE )
	{
		if ( p->config.feature & (1<<POWERPORT_FEATURE_NACHLAUF) )
		{
			powerport_setup_timer(p); //neu laden -> Timer aktiv
		}
	else
		{
			powerport_off(p); // sofort ausschalten
			p->countDownTimer = 0; // Timer aus
		}
	}
}

void powerport_on(device_data_powerport *p)
{
	if (p->config.feature & (1<<POWERPORT_FEATURE_BISTABLE_RELAIS))
	{
		darlingtonoutput_setpin(p->config.port, 1);
		canix_sleep_100th(8); // wait for 800 ms until relais had switched
		darlingtonoutput_setpin(p->config.port, 0);

		p->bistableRelaisState ^= 0x01; // toggle state of relais (toggle bit 0)
	}
	else darlingtonoutput_setpin(p->config.port, 1); // EIN

	powerport_send_state_info(p);
}

void powerport_off(device_data_powerport *p)
{
	darlingtonoutput_setpin(p->config.port, 0); // AUS

	powerport_send_state_info(p);
}

void powerport_toggle(device_data_powerport *p)
{
	// Achtung: darlingtonoutput_getpin liefert bei Bi-Relais immer 0, da pin nicht aktiv!!
	if (darlingtonoutput_getpin(p->config.port)) // Pin ist aktiv
	{
		set_powerport_or_timer(p, FALLENDE_FLANKE);
	}
	else
	{
		set_powerport_or_timer(p, STEIGENDE_FLANKE);
	}
}

/** Der powerport_timer_handler wird einmal pro Sekunde aufgerufen
 *  und ist zustaendig fuer das Ein- und Ausschalten des Powerports. */
inline void powerport_timer_handler(device_data_powerport *p, uint8_t zyklus)
{
	if (zyklus != 1) return; // 1s-Zyklus verwendet

	if (p->countDownTimer > 0) // Timer aktiv
	{
		if (p->countDownTimer == 1) // Timer abgelaufen
		{
			//Timer abgelaufen:
			if ( p->config.feature & (1<<POWERPORT_FEATURE_WECK_VERZOEGERT_EIN) )
			{
				powerport_on(p); // sofort einschalten
				p->config.feature &= ~(1<<POWERPORT_FEATURE_WECK_VERZOEGERT_EIN); // weck-feature wieder loeschen
				powerport_setup_timer(p); // neu laden -> Timer aktiv, falls z.B. AUTO_AUS konfiguriert ist
			}
			else if ( p->config.feature & (1<<POWERPORT_FEATURE_WECK_AUTO_AUS) )
			{
				powerport_off(p); // sofort ausschalten
				p->config.feature &= ~(1<<POWERPORT_FEATURE_WECK_AUTO_AUS); // weck-feature wieder loeschen
			}
			else if ( p->config.feature & (1<<POWERPORT_FEATURE_VERZOEGERT_EIN) )
			{
				powerport_on(p); // sofort einschalten
			}
			else if ( (p->config.feature & (1<<POWERPORT_FEATURE_AUTO_AUS)) ||
					  (p->config.feature & (1<<POWERPORT_FEATURE_NACHLAUF)) )
			{
				powerport_off(p); // sofort ausschalten
			}
		}

		p->countDownTimer--;
		// canix_syslog_P(SYSLOG_PRIO_ERROR,PSTR("countDownTimer=%d"), p->countDownTimer);
	}
}

void powerport_set(device_data_powerport *p, uint8_t state)
{
	darlingtonoutput_setpin(p->config.port, state);
}

// Liefert den aktuellen Zustand des Powerports
uint8_t powerport_get(const device_data_powerport *p)
{
	if (p->config.feature & (1<<POWERPORT_FEATURE_BISTABLE_RELAIS))
		return p->bistableRelaisState;
	else
	return darlingtonoutput_getpin(p->config.port);
}

static uint8_t is_in_group(const device_data_powerport *p, uint8_t group)
{
	uint8_t i;
	uint8_t *gruppen;
	uint8_t maxDeviceGruppen = MAX_POWERPORT_GROUPS;

	// die 255 ist der Ersatzwert und wird ignoriert!
	if (group == 255)
		return 0;
	
	gruppen = (uint8_t *) &(p->config.gruppe0);

	for (i = 0; i < maxDeviceGruppen; i++)
	{
		if (gruppen[i] == group)
			return 1;
	}

	return 0;
}

void powerport_can_callback(device_data_powerport *p, const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;

	if (is_in_group(p, frame->data[2]))
	{
		switch (frame->data[1])
		{
			case HCAN_HES_TASTER_DOWN :
				if (!p->mute) powerport_toggle(p);
				break;

			case HCAN_HES_POWER_GROUP_ON :
			case HCAN_HES_SCHALTER_ON :
				if (!p->mute) set_powerport_or_timer(p, STEIGENDE_FLANKE); //es soll eingeschaltet werden
				break;

			case HCAN_HES_POWER_GROUP_OFF :
			case HCAN_HES_SCHALTER_OFF :
				if (!p->mute) set_powerport_or_timer(p, FALLENDE_FLANKE); //es soll abgeschaltet werden
				break;

			// Achtung: Impulsausgaben liefern 0,
			// obwohl bistabile Relais ggf. an sind!
			// -> Bistabile Relais nur fuer Steckdosen verwenden!
			// -> Eine Gruppenansteuerung (GROUP_ON _OFF)
			// ist fuer bistabile Relais nicht sinnvoll.
			// Ggf. den Zustand im EEPROM sichern!!!
			case HCAN_HES_POWER_GROUP_STATE_QUERY :
				{
					answer.data[1] = HCAN_HES_POWER_GROUP_STATE_REPLAY;
					answer.data[2] = frame->data[2];
					answer.data[3] = powerport_get(p) != 0;
					// UI-PowerPortPage kann einen Timerwert bis zum Schalten anzeigen: 
					if(p->countDownTimer) answer.data[4] = (p->countDownTimer/60) + 1;
					else answer.data[4] = 0;
					answer.size = 5;
					canix_frame_send_with_prio(&answer,HCAN_PRIO_HI);
				}
				break;

			case HCAN_HES_DEVICES_CONFIGS_REQUEST :
			case HCAN_HES_POWER_GROUP_CONFIG_RQ :
				{
					answer.data[1] = HCAN_HES_POWER_GROUP_CONFIG_REPLAY;
					answer.data[2] = p->config.gruppe0;
					answer.size = 3;
					canix_frame_send_with_prio(&answer,HCAN_PRIO_HI);
				}
				break;

			case HCAN_HES_WECKER_SET :
				if (p->config.gruppe0 == frame->data[2]) //=weck_id
				{
					// Achtung: Das config.feature wird ueberschrieben.
					// D.h. der Wecker sollte nur für Powerports ohne aktives Feature verwendet werden
					p->config.feature |= 1<<frame->data[3]; // zusaetzlich POWERPORT_FEATURE_WECK_VERZOEGERT_EIN oder POWERPORT_FEATURE_WECK_VERZOEGERT_AUS
					// canix_syslog_P(SYSLOG_PRIO_ERROR,PSTR("2_SetWecker,tFeature=%d,feature=%d"), frame->data[3], p->config.feature);
					p->countDownTimer = (frame->data[4] << 8) | frame->data[5];
					// Da der countDownTimer gesetzt ist wird dieser in powerport_timer_handler dekrementiert usw.

					if ( p->config.feature & (1<<POWERPORT_FEATURE_WECK_AUTO_AUS))
					{
						powerport_on(p); // sofort einschalten
					}
				}
				break;

			case HCAN_HES_WECKER_DETAILS_REQUEST :
				{
					if (p->config.gruppe0 == frame->data[2]) // =weck_id
					{
						answer.data[1] = HCAN_HES_WECKER_DETAILS;
						answer.data[2] = p->config.gruppe0;
						answer.data[3] = p->countDownTimer >> 8; // MSB
						answer.data[4] = p->countDownTimer; // LSB
						answer.size = 5;
						canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
					}
				}
				break;
		}
	}
	else if (HCAN_HES_DEVICE_STATES_REQUEST == frame->data[1])
	{
		if(p->config.gruppe0 != 255)
		{
			wdt_reset();
			canix_sleep_100th(10); // 100msec Pause

			answer.data[1] = HCAN_HES_POWER_GROUP_STATE_REPLAY;
			answer.data[2] = p->config.gruppe0; // wird in main.c fuer jedes Device einmal aufgerufen
			answer.data[3] = powerport_get(p) != 0;
			// UI-PowerPortPage kann einen Timerwert bis zum Schalten anzeigen:
			if(p->countDownTimer) answer.data[4] = (p->countDownTimer/60) + 1;
			else answer.data[4] = 0;
			answer.size = 5;
			canix_frame_send_with_prio(&answer,HCAN_PRIO_HI);
		}
	}

	if (p->config.mute == frame->data[2])
	{
		if (HCAN_HES_MUTE_OFF == frame->data[1])
			p->mute = 0; // Powerport aktiv
		else if (HCAN_HES_MUTE_ON == frame->data[1])
			p->mute = 1; // Powerport passiv (per Taster nicht aenderbar)
	}
}

void powerport_send_state_info(device_data_powerport *p)
{
	canix_frame message;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_INFO;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_POWER_GROUP_STATE_INFO;
	message.data[2] = p->config.gruppe0; // Primaere Gruppe verwenden!
	message.data[3] = powerport_get(p) != 0;
	
	message.size = 4;
	canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);
}

