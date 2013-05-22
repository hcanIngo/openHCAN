#include <lichtzone.h>

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

#include <tasterinput.h>
#include <darlingtonoutput.h>
#include <hcan_multicast.h>

/**
 * Sendet Updates an alle Action-Gruppen
 */
void inform_action_groups(const device_data_lichtzone *p)
{
	canix_frame answer;
	uint8_t *action_group;
	uint8_t i;

	action_group = (uint8_t *)&(p->config.action_gruppe0);

	for (i = 0; i < 2; i++)
	{
		if (action_group[i] != 255)
		{
			answer.src = canix_selfaddr();
			answer.dst = HCAN_MULTICAST_CONTROL;
			answer.proto = HCAN_PROTO_SFP;
			answer.data[0] = HCAN_SRV_HES;
			answer.data[1] = p->state != 0 ? HCAN_HES_POWER_GROUP_ON : HCAN_HES_POWER_GROUP_OFF;
			answer.data[2] = action_group[i];
			answer.size = 3;
			canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
		}
	}
}

void lichtzone_setup_timer(device_data_lichtzone *p)
{
	// Timer initialisieren: Wenn das MSB == 1, dann
	// ist der Timerwert in Sekunden, ansonsten in Minuten
	//
	if (p->config.timer_delay & 0x80)
	{
		// der Wert ist in Sekunden:
		p->timer_counter = p->config.timer_delay & 0x7f;
	}
	else
	{
		p->timer_counter = (p->config.timer_delay * 60);
	}

}

void lichtzone_can_callback(device_data_lichtzone *p, 
		const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_TASTER_DOWN : 
			{
				uint8_t i;
				uint8_t *toggle_group;
				uint8_t *off_group;
				uint8_t *on_group;

				// fuer extended on Modus:
				if (p->exton_sec_counter == 0)
				{
					p->exton_sec_counter = 1;
				}
				p->exton_taster_down_counter++;

				// ueber alle 4 Toggle Gruppen iterieren
				toggle_group = (uint8_t *) &(p->config.toggle_gruppe0);
				for (i = 0; i < 4; i++)
				{
					if (frame->data[2] == toggle_group[i])
					{
						// Pruefen, ob in den ext on Modus geschaltet werden soll:
						if ((p->exton_taster_down_counter >= 3) && 
								(p->exton_sec_counter > 0) &&
								(p->exton_sec_counter < LICHTZONE_EXT_SEC_COUNTER_MAX))
						{
							p->exton_sec_counter = 0;
							p->exton_taster_down_counter = 0;
							p->state = LICHTZONE_STATE_ON_EXT;
							p->timer_counter = LICHTZONE_EXT_TIMERINIT;
							inform_action_groups(p);
							// ganz rausspringen, damit innerhalb der naechsten Interation der Schleife
							// nicht den State wieder zurueckgestetzt wird!
							return;
						}

						// Normaler Modus: wenn Timer aktiviert...
						if (p->config.timer_delay)
						{
							switch (p->state)
							{
								case LICHTZONE_STATE_OFF :
									lichtzone_setup_timer(p);
									p->state = LICHTZONE_STATE_ON;
									inform_action_groups(p);
									break;
								case LICHTZONE_STATE_ON : 
									lichtzone_setup_timer(p);
									break;
								case LICHTZONE_STATE_ON_EXT : 
									p->state = LICHTZONE_STATE_OFF;
									p->exton_sec_counter = 0;
									p->exton_taster_down_counter = 0;
									p->timer_counter = 0;
									inform_action_groups(p);
									break;
							}
						}
						// Normaler Modus: wenn kein Timer aktiviert...
						else
						{
							switch (p->state)
							{
								case LICHTZONE_STATE_OFF :
									p->state = LICHTZONE_STATE_ON;
									break;
								case LICHTZONE_STATE_ON : 
									p->state = LICHTZONE_STATE_OFF;
									break;
								case LICHTZONE_STATE_ON_EXT : 
									p->state = LICHTZONE_STATE_OFF;
									break;
							}

							inform_action_groups(p);
						}
					}
				}

				// wenn eine Off-Gruppe passt, die Action Gruppen
				// informieren. Das geschieht unabhaengig vom Status;
				// denn damit ist eine globale Haus-Licht-Aus-Funktion
				// realisierbar
				off_group = (uint8_t *) &(p->config.off_gruppe0);
				for (i = 0; i < 2; i++)
				{
					if ((frame->data[2] == off_group[i]))
					{
						p->state = LICHTZONE_STATE_OFF;
						p->timer_counter = 0;
						inform_action_groups(p);
					}
				}

				// wenn eine On-Gruppe passt, die Action Gruppen
				// informieren. Das geschieht unabhaengig vom Status;
				// denn damit ist eine globale Haus-Licht-An-Funktion
				// realisierbar
				on_group = (uint8_t *) &(p->config.on_gruppe0);
				for (i = 0; i < 2; i++)
				{
					if ((frame->data[2] == on_group[i]))
					{
						p->state = LICHTZONE_STATE_ON;

						// If it has a timer, reset the timer:
						if (p->config.timer_delay) 
							lichtzone_setup_timer(p);

						inform_action_groups( p);
					}
				}
			}
			break;

		case HCAN_HES_POWER_GROUP_ON : 
			{
				// schauen, ob eine der Action Groups passt; falls
				// ja, internen Status updaten und ggfls Timer starten
				uint8_t *action_group;
				uint8_t i;

				action_group = (uint8_t *)&(p->config.action_gruppe0);

				for (i = 0; i < 2; i++)
				{
					if ((action_group[i] != 255) && 
							(action_group[i] == frame->data[2]))
					{
						switch (p->state)
						{
							case LICHTZONE_STATE_OFF :
							case LICHTZONE_STATE_ON :
								p->state = LICHTZONE_STATE_ON;
								if (p->config.timer_delay)
									lichtzone_setup_timer(p);
								// inform_action_groups(..) entfaellt, da
								// wir ja eine Botschaft, die direkt an die
								// Action group ging, abgefangen haben. Daher
								// haben wir "nur" den State aktualisiert.
								break;
							case LICHTZONE_STATE_ON_EXT :
								// in diesem Falls nichts machen!
								break;
						}
					}
				}
			}
			break;

		case HCAN_HES_POWER_GROUP_OFF : 
			{
				// schauen, ob eine der Action Groups passt; falls
				// ja, internen Status updaten
				uint8_t *action_group;
				uint8_t i;

				action_group = (uint8_t *)&(p->config.action_gruppe0);

				for (i = 0; i < 2; i++)
				{
					if ((action_group[i] != 255) && 
							(action_group[i] == frame->data[2]))
					{
						p->state = LICHTZONE_STATE_OFF;
						p->timer_counter = 0;

						// inform_action_groups(..) entfaellt, da
						// wir ja eine Botschaft, die direkt an die
						// Action group ging, abgefangen haben. Daher
						// haben wir "nur" den State aktualisiert.
						
						break;
					}
				}
			}
			break;

		case HCAN_HES_POWER_GROUP_TIMER_QUERY : 
			{
				canix_frame answer;
				uint8_t *action_group;
				uint8_t i;

				action_group = (uint8_t *)&(p->config.action_gruppe0);

				for (i = 0; i < 2; i++)
				{
					if (action_group[i] == frame->data[2])
					{
						answer.src = canix_selfaddr();
						answer.dst = frame->src;
						answer.proto = HCAN_PROTO_SFP;
						answer.data[0] = HCAN_SRV_HES;
						answer.data[1] = HCAN_HES_POWER_GROUP_TIMER_REPLAY;
						answer.data[2] = action_group[i];
						answer.data[3] = p->timer_counter >> 8;
						answer.data[4] = p->timer_counter & 0xff;
						answer.size = 5;
						canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
						break;
					}
				}
			}
			break;

		case HCAN_HES_POWER_GROUP_SET_TIMER : 
			{
				uint8_t *action_group;
				uint8_t i;

				action_group = (uint8_t *)&(p->config.action_gruppe0);

				for (i = 0; i < 2; i++)
				{
					if (action_group[i] == frame->data[2])
					{
						p->timer_counter = (frame->data[3] << 8) |
							frame->data[4];
						break;
					}
				}
			}
			break;

	}
}

// wird alle 1 sec aufgerufen, fuer jede lichtzone-Instanz einmal
void lichtzone_timer_handler(device_data_lichtzone *p)
{
	// falls der Timer aktiviert ist und noch nicht abgelaufen ist:
	if (p->config.timer_delay && (p->timer_counter > 0))
	{
		// Timer dekrementieren
		p->timer_counter--;

		// wenn er abgelaufen ist:
		if (p->timer_counter == 0)
		{
			// Timer ist abgelaufen; Licht ausschalten
			p->state = LICHTZONE_STATE_OFF;
			inform_action_groups(p);
		}
	}

	// Falls der 3fach-Click geprueft wird:
	if (p->exton_sec_counter >= 1)
	{
		p->exton_sec_counter++;
	}

	// Falls die Wartezeit fuer den 3fach-Click abgelaufen ist, Zaehler zuruecksetzten
	if (p->exton_sec_counter >= LICHTZONE_EXT_SEC_COUNTER_MAX)
	{
		p->exton_sec_counter = 0;
		p->exton_taster_down_counter = 0;
	}
}

