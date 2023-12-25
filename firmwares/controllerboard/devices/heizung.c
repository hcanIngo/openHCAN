#include "../../controllerboard/devices/heizung.h"

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "../../controllerboard/darlingtonoutput.h"
#include "../../controllerboard/devices/zeitzone.h"

void heizung_init(device_data_heizung *p, eds_block_p it)
{
	p->it = it;
	p->mode = HEIZUNG_MODE_AUTOMATIK;
	p->manual_rate = 0;
	p->waermebedarf_counter = p->config.id & 0x3f; // Pseudo-Zufall Init
	p->received_interval_counter = 255;
	p->measure_value = 0;
	p->duration_counter = 0;
	p->thermostat_temp = 0;
	p->reed_heiz_stop_counter = 0;
	p->ventilpflege_counter = 0;
	p->pwm_counter = 0;
	p->pwm_width = 0;
	p->pwm_end = p->config.pwm_periode;

  canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("heizung_init: %d"), p->config.id);
#if HEIZUNG_INCL_PID_REGLER_CODE == 1
  if (p->config.feature & (1<<HEIZUNG_FEATURE_PID_REGLER))
  {
    p->last_soll_temp = -1;

    p->pidcounter = ABSTANDPIDMESSUNGEN;
    p->pfaktor = 30; // 1/100; 30 == 0,3
    p->ifaktor = 50; // 1/100; 50 == 0,5
    p->dfaktor = 30; // 1/100; 30 == 0,3
    p->sollistIndex = 0;

    for (uint8_t i = 0; i < SOLLISTWERTE; i++)
    {
        p->sollist[i] = 0;
    }

    canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("heizung_p:%d"), p->pfaktor);
    canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("heizung_i:%d"), p->ifaktor);
    canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("heizung_d:%d"), p->dfaktor);
    canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("heizung_abstandPid:%d"), ABSTANDPIDMESSUNGEN);
    canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("heizung_SOLLISTWERTE:%d"), SOLLISTWERTE);
  }
#endif
}

/**
 * Schaltet die HK-Ventile aus, wenn ein Heizstop vorliegt. Dies sollte
 * in regelmaessigen Abstaenden aufgerufen werden, da eventuell aufgrund der
 * Heizsteuerlogik die PWM nicht regelmaessig upgedatet wird und so das HK-Ventil
 * nicht auf den Reedkontakt reagiert.
 */
void heizung_update_heizstop(device_data_heizung *p)
{
	// Heizstoppmeldungen nur jede Minute verschicken. Aber nicht für alle Heizungen gleichzeitig
	if (canix_rtc_clock.second == (p->config.id & 0x3b))
	{
		canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("Heizstop aktiv (id=%d)"), p->config.id);
	}

	// Heizung ganz ausschalten
	p->pwm_end = p->config.pwm_periode;
	p->pwm_width = 0;
}

void heizung_set_pwm(device_data_heizung *p, uint8_t rate)
{
	// Heiz-Stop Funktion bei offenem Fenster:
	if (p->reed_heiz_stop_counter)
	{
		heizung_update_heizstop(p);
	}
	else
	{
		// Normal-Fall: kein Heiz-Stop
		p->pwm_end = p->config.pwm_periode;
		p->pwm_width = (int32_t)p->pwm_end * (int32_t)rate / 100;
	}

	canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("set_pwm id%d c%d p%d  e%d r%d w%d"),
	  p->config.id, p->pwm_counter, p->config.pwm_periode, p->pwm_end, rate, p->pwm_width);
}

/**
 * Liefert die aktuelle PWM Heizrate in % (0..100). Achtung: Durch kurze
 * config.pwm_periode Werte entstehen bei Hin- und Rueckrechung
 * Quantisierungsfehler!
 */
uint8_t heizung_get_pwm(device_data_heizung *p)
{
	uint8_t rate = 0;

	if (p->mode != HEIZUNG_MODE_OFF)
	{
		if ((uint32_t)p->pwm_end == 0)
		{
			canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("pwm_end ist 0! (id=%d)"), p->config.id);
			return 100; // Ventil oeffnen
		}

		rate = (uint32_t)p->pwm_width * 100 / (uint32_t)p->pwm_end;
	}

	return rate;
}

/**
 * this one cares about the PWM (pulse width modulation) for
 * achieving heating rates between off and on
 */
void heizung_handle_pwm(device_data_heizung *p)
{
	uint8_t state = 1;
	if (++p->pwm_counter >= (p->pwm_end))
	{
		p->pwm_counter = 0; // und  state = 1
	}

	if (p->pwm_counter >= p->pwm_width)
	{
		state = 0;
	}

	darlingtonoutput_setpin(p->config.port, state);
	canix_syslog_P(SYSLOG_PRIO_DEBUG,
			PSTR("handle_pwm: heiz: %d: %d %d  c%d e%d w%d  %d"), p->config.id,
			darlingtonoutput_getpin(p->config.port), state, p->pwm_counter, p->pwm_end, p->pwm_width, p->measure_value);

}

// Liefert die zeitzone id des aktiven solltemp_line_t zurueck
int8_t heizung_get_matching_zeitzone_id(device_data_heizung *p)
{
	solltemp_line_t *solltemp_line = (solltemp_line_t *) &(p->config.zeitzone0_id);

	for (uint8_t i = 0; i < 8; i++)
	{
		const uint8_t tr_id = solltemp_line[i].id;
		if (zeitzone_matches(tr_id))
		{
			return tr_id; // we got an zeitzone which matches
		}
	}
	return -1;
}

// Liefert den Index des aktiven solltemp_line_t zurueck
int8_t heizung_get_matching_zeitzone_index(device_data_heizung *p)
{
	solltemp_line_t *solltemp_line = (solltemp_line_t *) &(p->config.zeitzone0_id);

	for (uint8_t i = 0; i < 8; i++)
	{
		const uint8_t tr_id = solltemp_line[i].id;
		if (zeitzone_matches(tr_id))
		{
			return i; // we got an zeitzone which matches
		}
	}
	return -1;
}

/**
 * Setzt die PWM Rate in Abhaengigkeit der Soll/Ist-Differenz; hier kann
 * auch ein anderer Algorithmus wie z.B. eine PID-Regelung eingebaut werden.
 *
 * Zur Zeit ist ein einfacher 2-Punkt-Regler mit einer Hysterese realisiert:
 *
 * Ist die Heizung an, so geht sie erst bei
 *   Isttemp > Solltemp + Hysterese aus;
 * Ist die Heizung aus, so geht sie erst bei
 *   Isttemp < Solltemp - Hysterese an
 */
void heizung_set_pwm_by_temp(device_data_heizung *p, int16_t soll, int16_t ist)
{
	// Beispiel: Soll = 20 Grad, Hysterese = 0.25 K (4/16 K)
	// Wenn Heizung an ist und die Temperatur > 20.25 Grad, dann Heizung ausschalten!
	if ((heizung_get_pwm(p) > 0) && (ist > (soll + p->config.hysterese)))
	{
		heizung_set_pwm(p,0); // Ventil schliessen, es ist in ca. 3 Minuten zu
		return;
	}

	// Beispiel: Soll = 20 Grad, Hysterese = 0.25 K (4/16 K)
	// Wenn Heizung aus ist und die Temperatur < 19.45 Grad, dann Heizung einschalten!
	if ((heizung_get_pwm(p) == 0) && (ist < (soll - p->config.hysterese)))
	{
		heizung_set_pwm(p,100); // Ventil oeffnen, es ist in ca. 3 Minuten offen
		p->waermebedarf_counter = HEIZUNG_WAERMEBEDARF_VERZOEGERUNG_s;
		return;
	}
}


#if HEIZUNG_INCL_PID_REGLER_CODE == 1
// Einfacher PID Regler
void heizung_set_pwm_by_pid(device_data_heizung *p, int32_t pidvalue)
{
        if (p->pwm_change_counter != 0)
        {
                return;
        }

        int16_t pwm = pidvalue / 30;

        if (heizung_get_pwm(p) != pwm)
        {
                if (pwm > 0)
                {
                        if (pwm > 100)
                        {
                                pwm = 100;
                        }

                        heizung_set_pwm(p, pwm);
                        canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("ID: %d pwm: %d PID: %d"), p->config.id, pwm, pidvalue);
                        p->waermebedarf_counter = HEIZUNG_WAERMEBEDARF_VERZOEGERUNG_s;
                }
                else if (pwm == 0)
                {
                        heizung_set_pwm(p, 0);
                        canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("ID: %d pwm: 0 PID: 0"),  p->config.id);
                }
                else if (pwm < 0)
                {
                        heizung_set_pwm(p, 0);
                        canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("ID: %d pwm: 0 PID: %d"), p->config.id, pidvalue);
                }
        }
        else
        {
                canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("ID: %d pwm gleich! %d"), p->config.id, pwm);
        }
        p->pwm_change_counter = PWMCHANGEFREQENCY;
        return;
}
#endif

void heizung_check_ventilpflege(device_data_heizung *p)
{
	// Pruefen: ist die Heizung aus und keine Ventilpflege aktiv?
	// Nur dann pruefen wir weiter, ob eine Ventilpflege ansteht:
	if ( (p->ventilpflege_counter == 0) && ( (p->mode == HEIZUNG_MODE_OFF)
		|| (p->config.feature & (1<<HEIZUNG_FEATURE_VENTIL_IMMER_PFLEGEN)) ) )
	{
		// Startzeitpunkt der Ventilpflege ist immer:
		// Sonntags, 11:00 + Heizungs-ID * 3 (in Minuten)
		//
		// Beispiel: ID: 1;   -> Startzeitpunkt: 11:03
		// Beispiel: ID: 67;  -> Startzeitpunkt: 14:21
		// Beispiel: ID: 160; -> Startzeitpunkt: 19:00
		// Beispiel: ID: 254; -> Startzeitpunkt: 23:42

		uint16_t time = canix_rtc_clock.hour * 60 + canix_rtc_clock.minute;

		if ((canix_rtc_clock.day_of_week == 7) && // Sonntag
				(time == (p->config.id * 3) + (11*60 + 0)))
		{
			p->ventilpflege_counter = 600; // Ventilpflege aktivieren
			canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("heizung %d: ventilpflege start"), p->config.id);
		}
	}
}

static void heizung_send_details(device_data_heizung *p, const uint16_t answerDst)
{
	canix_frame answer;
	answer.src = canix_selfaddr();
	answer.dst = answerDst; // an den Anfragenden zuruecksenden per empf->src, oder HCAN_MULTICAST_INFO
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;

	switch (p->mode)
	{
		case HEIZUNG_MODE_OFF :
			answer.data[1] = HCAN_HES_HEIZUNG_MODE_OFF_DETAILS;
			answer.data[2] = p->config.id;
			answer.size = 3;
			canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
			break;

		case HEIZUNG_MODE_MANUAL :
			answer.data[1] = HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS;
			answer.data[2] = p->config.id;
			answer.data[3] = p->manual_rate;
			answer.data[4] = p->duration_counter >> 8;
			answer.data[5] = p->duration_counter;
			answer.size = 6;
			canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
			break;

		case HEIZUNG_MODE_THERMOSTAT :
			answer.data[1] = HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS;
			answer.data[2] = p->config.id;
			answer.data[3] = p->thermostat_temp >> 8;
			answer.data[4] = p->thermostat_temp;
			answer.data[5] = p->duration_counter >> 8;
			answer.data[6] = p->duration_counter;
			answer.size = 7;
			canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
			break;

		case HEIZUNG_MODE_AUTOMATIK :
			{
				int8_t index = heizung_get_matching_zeitzone_index(p);
				if (index != -1)
				{
					// we got an zeitzone which matches
					int16_t master_value = 0;
					solltemp_line_t *zeitzone =	(solltemp_line_t *)	&(p->config.zeitzone0_id);

					master_value = zeitzone[index].temp;
					answer.data[1] = HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS;
					answer.data[2] = p->config.id;
					answer.data[3] = master_value >> 8;
					answer.data[4] = master_value;
					answer.data[5] = heizung_get_matching_zeitzone_id(p);
					answer.data[6] = p->measure_value >> 8;
					answer.data[7] = p->measure_value;
					answer.size = 8;
				}
				else
				{
					canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("zeitzone: no match! (id=%d)"), p->config.id);

					answer.data[1] = HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS;
					answer.data[2] = p->config.id;
					answer.data[3] = 0;
					answer.data[4] = 0;
					answer.data[5] = 0;
					answer.size = 6;
				}
				canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
			}
			break;
	}
}

// this timer handler is called every second
inline void heizung_timer_handler(device_data_heizung *p, uint8_t zyklus)
{
	if (zyklus != 1) return; // 1s-Zyklus verwendet

	if (p->ventilpflege_counter > 0)
		p->ventilpflege_counter--;

#if HEIZUNG_INCL_PID_REGLER_CODE == 1
	if (p->config.feature & (1<<HEIZUNG_FEATURE_PID_REGLER))
	{
		if (p->pwm_change_counter > 0)
		{
			p->pwm_change_counter--;
		}
	}
#endif

	// Ventilpflege aktivieren, falls der richtige Zeitpunkt dazu ist:
	heizung_check_ventilpflege(p);

	if (p->received_interval_counter != 0xff)
		p->received_interval_counter++;

	heizung_handle_pwm(p);

	if (p->duration_counter > 0)
	{
		p->duration_counter -= 1;
		if (p->duration_counter == 0)
		{
			// Zeit ist abgelaufen; nun wieder auf Automatik-Modus stellen:
			p->mode = HEIZUNG_MODE_AUTOMATIK;
			heizung_send_details(p, HCAN_MULTICAST_INFO); // Mode-Wechsel melden
		}
	}

	if (p->reed_heiz_stop_counter > 0)
	{
		p->reed_heiz_stop_counter -= 1;

		if (p->reed_heiz_stop_counter == 0)
		{
			canix_syslog_P(SYSLOG_PRIO_DEBUG,
					PSTR("heiz-stop ende (heizung %d)"), p->config.id);
		}
		/*
		 * Schaltet die HK-Ventile aus, wenn ein Heizstop vorliegt. Dies passiert
		 * in regelmaessigen Abstaenden, da eventuell aufgrund der Heizsteuerlogik
		 * die PWM nicht regelmaessig upgedatet wird und so das HK-Ventil nicht auf
		 * den Reedkontakt reagiert.
		 */
		heizung_update_heizstop(p);
	}

	if (p->timer_counter++ >= 5)
	{
		p->timer_counter = 0;

		if (p->ventilpflege_counter)
		{
			// Sonderfall: Ventilpflege ist aktiv; dazu
			// Ventil einschalten, sonst aber nichts unternehmen:
			p->manual_rate = 0;

			// Ventil einschalten:
			darlingtonoutput_setpin(p->config.port, 1); //heizung_set_pwm(p,100);
			canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("heizung: %d ventilpflege aktiv"), p->config.id);
			return; // Funktion verlassen, da keine Waermebedarfsmeldung und PID berechnung nötig sind.
		}

		switch (p->mode)
		{
			case HEIZUNG_MODE_OFF :
				p->manual_rate = 0;
				heizung_set_pwm(p,0);
				// Ventil ausschalten, falls es an ist:
				if (darlingtonoutput_getpin(p->config.port))
						darlingtonoutput_setpin(p->config.port, 0);
				break;

			case HEIZUNG_MODE_MANUAL :
				heizung_set_pwm(p, p->manual_rate);
				break;

			case HEIZUNG_MODE_AUTOMATIK :
				// Wenn ein gueltiger Messwert vorliegt
				// (d.h. wenn innerhalb eines definierten Zeitraumes
				// ein Sensor-Messwert eingetroffen ist:
				if (p->received_interval_counter < 255)
				{
					int8_t index = heizung_get_matching_zeitzone_index(p);
					if (index != -1) // passende Zeitzone?
					{
						solltemp_line_t *solltemp_line = (solltemp_line_t*) &(p->config.zeitzone0_id);
#if HEIZUNG_INCL_PID_REGLER_CODE == 1
						if (p->config.feature & (1<<HEIZUNG_FEATURE_PID_REGLER))
						{
							heizung_set_pwm_by_pid(p, p->pidvalue);
							p->destination_value = solltemp_line[index].temp;
						}
						else
#endif
						{
						  heizung_set_pwm_by_temp(p, solltemp_line[index].temp, p->measure_value); // 2-Punkt-Regler
						}

						if (p->last_soll_temp != solltemp_line[index].temp) // Solltemperatur-Aenderung?
						{
							heizung_send_details(p, HCAN_MULTICAST_INFO); // infomiere darueber
							p->last_soll_temp = solltemp_line[index].temp;
						}
					}
					else // keine passende Zeitzone gefunden:
					{
						heizung_set_pwm(p,0);
					}
				}
				else
				{
					canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("keine Sensor-Messwerte vorhanden! (id=%d)"), p->config.id);
				}
				break;

			case HEIZUNG_MODE_THERMOSTAT :
				// Wenn ein gueltiger Messwert vorliegt
				// (d.h. wenn innerhalb eines definierten Zeitraumes
				// ein Sensor-Messwert eingetroffen ist:
				if (p->received_interval_counter < 255)
				{
#if HEIZUNG_INCL_PID_REGLER_CODE == 1
					if (p->config.feature & (1<<HEIZUNG_FEATURE_PID_REGLER))
					{
						heizung_set_pwm_by_pid(p, p->pidvalue);
						p->destination_value = p->thermostat_temp;
					}
					else
#endif
					{
					  heizung_set_pwm_by_temp(p, p->thermostat_temp, p->measure_value); // 2-Punkt-Regler
					}
				}
				else
				{
					canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("keine Sensor-Messwerte vorhanden! (id=%d)"), p->config.id);
				}
				break;
		}
	}

	if ((p->received_interval_counter == 255) && (HEIZUNG_MODE_MANUAL != p->mode)) // keine Sensor-Messwert?
	{
		return; // keine Waermebedarfsmeldung versenden  und  keine PID Berechnung
	}

	// Waermebedarfsmeldung versenden:
	if (p->waermebedarf_counter-- == 0)
	{
		p->waermebedarf_counter = 60; // Waermebedarfsmeldungen alle 60 Sekunden
		uint8_t rate = heizung_get_pwm(p); // den aktuellen Ventilstand / Heizrate ermitteln

		if (p->config.feature & (1<<HEIZUNG_FEATURE_DOPPEL_WAERMEBEDARF))
			rate = rate << 1; // Waermebedarf verdoppeln

		// Der Waermebedarf ist proportional zur Ventilstellung, d.h.
		// der Bedarf ist hoch, wenn das Ventil voll offen ist, und ist
		// nicht vorhanden, wenn das Ventil zu ist.
		canix_frame message;
		message.src = canix_selfaddr();
		message.dst = HCAN_MULTICAST_INFO;
		message.proto = HCAN_PROTO_SFP;
		message.data[0] = HCAN_SRV_HES;
		message.data[1] = HCAN_HES_HEIZUNG_WAERMEBEDARF_INFO;
		message.data[2] = p->config.id;
		message.data[3] = rate;
		message.size = 4;
		canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);
	}

#if HEIZUNG_INCL_PID_REGLER_CODE == 1
	if (p->config.feature & (1<<HEIZUNG_FEATURE_PID_REGLER))
	{
		if (p->destination_value < p->measure_value)
		{
			// wenn die gewuenschte Temperatur unter der gemessenen ist, Heizung direkt auf 0% schalten.
			p->pidvalue = 0;
			//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("PID fix:0"));
		}
		// 80 = 5 Grad mal 16!
		else if ((p->measure_value + 80) < p->destination_value)
		{
			// wenn die gewuenschte Temperatur 5° über der gemessenen liegt, Heizung direkt auf 100% schalten.
			p->pidvalue = 3000;
			//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("PID fix:3000"));
		}
		else
		{
			if (p->pidcounter-- == 0)
			{
					//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("pidcounter:0;_neu:%d"), ABSTANDPIDMESSUNGEN);
					p->pidcounter = ABSTANDPIDMESSUNGEN;
					if (p->destination_value != 0 && p->measure_value != 0)
					{
						p->sollist[p->sollistIndex] = p->destination_value - p->measure_value;
						//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("dest:%d measure:%d"), p->destination_value, p->measure_value);

						uint8_t letzter = p->sollistIndex;
						uint8_t vorletzter = 0;
						uint8_t vorvorletzter = 0;

						switch (letzter)
						{
								case 0:
								vorletzter = 2;
								vorvorletzter = 1;
								break;
								case 1:
								vorletzter = 0;
								vorvorletzter = 2;
								break;
								case 2:
								vorletzter = 1;
								vorvorletzter = 0;
								break;
								default:
								/* Your code here */
								break;
						}
						//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("letzter:%d vorletzter:%d vorvorletzter:%d"), p->sollist[letzter], p->sollist[vorletzter], p->sollist[vorvorletzter]);

						int32_t pwert = ( p->pfaktor * (p->sollist[letzter]) );
						int32_t iwert = ( p->ifaktor * (p->sollist[letzter] + p->sollist[vorletzter] + p->sollist[vorvorletzter]) );
						int32_t dwert = ( p->dfaktor * (p->sollist[letzter] - p->sollist[vorletzter]) );
						//int32_t pidwert = pwert + iwert + dwert;
						p->pidvalue = pwert + iwert + dwert;

						//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("P:%d I:%d D:%d PID:%d"), pwert, iwert, dwert, pidwert);
						//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("P:%d"), pwert);
						//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("I:%d"), iwert);
						//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("D:%d"), dwert);
						//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("PID:%d"), p->pidvalue );

						if (++p->sollistIndex >= SOLLISTWERTE)
						{
								p->sollistIndex = 0;
						}
					}
			}
		}
	}
#endif
}

void case_reedkontakt_offen(device_data_heizung *p, const canix_frame *frame)
{
	// Schauen, ob wir einen Heiz-Stop einlegen muessen, weil
	// eines unserer Fenster geoeffnet wurde:
	for (uint8_t i = 0; i < 8; i++) // es gibt 8 Reedkontakt-Eintrage...
	{
		uint8_t reed_gruppe = (&p->config.reed0)[i];
		if (reed_gruppe == frame->data[2])
		{
			//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("HSp h=%d,r=%d"), p->config.id, reed_gruppe);
			if (p->reed_heiz_stop_counter == 0)
			{
				// nur zu Debugging Zwecke: das Fenster wurde
				// jetzt geoeffnet, der Heiz-Stop ist ab jetzt aktiv:
				canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("heiz-stop begin (heizung %d, reed %d)"),
					p->config.id, reed_gruppe);
			}

			// ja, es ist eines unserer Fenster; also Heiz-Stop
			// fuer eine Minute einlegen - falls das Fenster
			// weiter offen bleibt, kommt die naechste Reed-
			// Meldung auf jeden Fall innerhalb einer Minute
			// (Normalfall: alle 10 Sekunden)!
			p->reed_heiz_stop_counter = 60;
			return; // wenn mindst. ein Fenster offen ist, hat sich die Sache jetzt erledigt
		}
	}
}

void heizung_can_callback(device_data_heizung *p, const canix_frame *frame)
{
	canix_frame answer;
	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;

	switch (frame->data[1])
	{
		case HCAN_HES_1WIRE_TEMPERATURE :
			if (frame->data[2] == p->config.sensor_id)
			{
				// we have a heizung and a temp sensor frame
				// now look if if fits to the heizung

				// temperature is a fixed point number mit 4 LSB Bits
				int16_t sensor_temp = frame->data[3] << 8 |	frame->data[4];
				p->measure_value = sensor_temp; // speichern
				p->received_interval_counter = 0;
			}
			break;

		case HCAN_HES_HEIZUNG_SET_MODE_OFF :
			if (p->config.id == frame->data[2])
			{
				p->mode = HEIZUNG_MODE_OFF;
				heizung_send_details(p, HCAN_MULTICAST_INFO);
			}
			break;

		case HCAN_HES_HEIZUNG_SET_MODE_MANUAL :
			if (p->config.id == frame->data[2])
			{
				p->mode = HEIZUNG_MODE_MANUAL;
				p->manual_rate = frame->data[3];
				p->duration_counter = (frame->data[4] << 8) | frame->data[5];
				heizung_send_details(p, HCAN_MULTICAST_INFO);
			}
			break;

		case HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS :
			if (p->config.id == frame->data[2])
			{
				p->mode = HEIZUNG_MODE_THERMOSTAT;
				p->thermostat_temp = (frame->data[3] << 8) | frame->data[4];
				p->duration_counter = (frame->data[5] << 8) | frame->data[6];
				heizung_send_details(p, HCAN_MULTICAST_INFO);
			}
			break;

		case HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK:
			if (p->config.id == frame->data[2])
			{
				p->mode = HEIZUNG_MODE_AUTOMATIK;
				heizung_send_details(p, HCAN_MULTICAST_INFO);
			}
			break;

		case HCAN_HES_DEVICE_STATES_REQUEST:
			if((p->type == EDS_heizung_BLOCK_ID) && (p->config.id != 255))
			{
				wdt_reset();
				canix_sleep_100th(10); // 100msec Pause

				heizung_send_details(p, frame->src);
			}
			break;

		case HCAN_HES_HEIZUNG_DETAILS_REQUEST :
			if (p->config.id == frame->data[2])
			{
				heizung_send_details(p, frame->src);
			}
			break;

		case HCAN_HES_DEVICES_CONFIGS_REQUEST :
		case HCAN_HES_HEIZUNG_CONFIG_RQ :
			answer.data[1] = HCAN_HES_HEIZUNG_CONFIG_REPLAY;
			answer.data[2] = p->config.id;
			answer.data[3] = p->config.sensor_id;
			answer.size = 4;
			canix_frame_send_with_prio(&answer,HCAN_PRIO_HI);
			break;

		case HCAN_HES_HEIZUNG_TIST_REQUEST :
			if (frame->data[2] == p->config.id)
			{
				answer.data[1] = HCAN_HES_HEIZUNG_TIST_REPLAY;
				answer.data[2] = p->config.id;
				answer.data[3] = p->measure_value >> 8;
				answer.data[4] = p->measure_value;
				answer.size = 5;
				canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
			}
			break;

		case HCAN_HES_HEIZUNG_SOLLTEMP_LINE_REQUEST :
			if ((frame->data[2] == p->config.id) && (frame->data[3] <= 7)) // Idx pruefen
			{
				solltemp_line_t *solltemp_line;
				uint8_t idx = frame->data[3];

				answer.data[1] = HCAN_HES_HEIZUNG_SOLLTEMP_LINE_REPLAY;
				answer.data[2] = p->config.id;
				answer.data[3] = idx;
				solltemp_line = (solltemp_line_t *)
					&(p->config.zeitzone0_id);
				answer.data[4] = solltemp_line[idx].temp >> 8;
				answer.data[5] = solltemp_line[idx].temp & 0xff;
				answer.data[6] = solltemp_line[idx].id;
				answer.size = 7;
				canix_frame_send_with_prio(&answer, HCAN_PRIO_LOW);
			}
			break;

		case HCAN_HES_HEIZUNG_SOLLTEMP_WRITE_LINE :
			if ((frame->data[2] == p->config.id) && (frame->data[3] <= 7)) // Idx pruefen
			{
				uint8_t idx = frame->data[3];
				solltemp_line_t *solltemp_line;

				// EEPROM/EDS Validierungstests:
				if ((p->it != 0) && (EDS_BLOCK_TYPE(p->it) == EDS_heizung_BLOCK_ID))
				{
					// neue Daten ins EEPROM schreiben; dazu:
					//  2 Bytes Offset fuer Typ + Size
					//  4 Byte  Offset fuer Ueberspringen der ersten
					//  Felder

					solltemp_line = (solltemp_line_t *)	&(p->config.zeitzone0_id);

					// ACHTUNG: zeitzone_id wird bisher NICHT geschrieben
					// Grund: bisher bei HomeCenter keine Notwendigkeit; somit
					// kann weniger kaputt gehen!
					// solltemp_line0_id (1 Byte)
					// solltemp_line[idx].id = frame->data[6];
					// eeprom_write_byte(p->it + 6 + (idx *3), frame->data[6]);

					solltemp_line[idx].temp = (frame->data[4] << 8) | frame->data[5]; // solltemp_line0_temp (2 Bytes)
					eeprom_write_word((uint16_t*)(p->it + 6 + (idx *3) + 1), solltemp_line[idx].temp);
					answer.data[1] = HCAN_HES_HEIZUNG_SOLLTEMP_WRITE_LINE_ACK;
					answer.data[2] = p->config.id;
					answer.data[3] = idx;
					answer.size = 4;
					canix_frame_send_with_prio(&answer, HCAN_PRIO_LOW);
				}
			}
			break;

		case HCAN_HES_REEDKONTAKT_OFFEN :
			case_reedkontakt_offen(p, frame);
			break;
	}
}
