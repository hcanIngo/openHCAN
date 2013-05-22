#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/eeprom.h>

#include <heizung.h>
#include <darlingtonoutput.h>
#include <zeitzone.h>

void heizung_init(device_data_heizung *p, eds_block_p it)
{
	p->it = it;
	p->mode = HEIZUNG_MODE_OFF;
	p->manual_rate = 0;
	p->pwm_counter = 0;
	p->pwm_width = 0;
	p->pwm_end = 0;
	p->waermebedarf_counter = p->config.id & 0x3f; // Pseudo-Zufall Init
	p->received_interval_counter = 255;
	p->measure_value = 0;
	p->duration_counter = 0;
	p->thermostat_temp = 0;
	p->reed_heiz_stop_counter = 0;
	p->ventilpflege_counter = 0;
	canix_syslog_P(SYSLOG_PRIO_DEBUG,
			PSTR("heizung_init: %d"), p->config.id);
}

/** 
 * Schaltet die HK-Ventile aus, wenn ein Heizstop vorliegt. Dies sollte
 * in regelmaessigen Abstaenden aufgerufen werden, da eventuell aufgrund der
 * Heizsteuerlogik die PWM nicht regelmaessig upgedatet wird und so das HK-Ventil
 * nicht auf den Reedkontakt reagiert.
 */
void heizung_update_heizstop(device_data_heizung *p)
{
	if (p->reed_heiz_stop_counter)
	{
		canix_syslog_P(SYSLOG_PRIO_DEBUG,
				PSTR("Heizstop aktiv (id=%d)"), p->config.id);


		// Heizung ganz ausschalten
		p->pwm_end = p->config.pwm_periode;
		p->pwm_width = 0;
	}
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
	if (++p->pwm_counter >= (p->pwm_end))
	{
		p->pwm_counter = 0;
		darlingtonoutput_setpin(p->config.port, 1);
	}

	if (p->pwm_counter >= p->pwm_width)
	{
		darlingtonoutput_setpin(p->config.port, 0);
	}
	/*
	canix_syslog_P(SYSLOG_PRIO_DEBUG,
			PSTR("handle_pwm: heiz: %d: %d"), p->config.id, 
			darlingtonoutput_getpin(p->config.port));
	*/
}

/**
 * Liefert die zeitzone id des aktiven solltemp_line_t zurueck
 */
int8_t heizung_get_matching_zeitzone_id(device_data_heizung *p)
{
	uint8_t i;
	solltemp_line_t *solltemp_line = (solltemp_line_t *) 
		&(p->config.zeitzone0_id);

	for (i = 0; i < 8; i++)
	{
		const uint8_t tr_id = solltemp_line[i].id;
		if (zeitzone_matches(tr_id))
		{
			// we got an zeitzone which matches
			return tr_id;
		}
	}
	return -1;
}
/**
 * Liefert den Index des aktiven solltemp_line_t zurueck
 */
int8_t heizung_get_matching_zeitzone_index(device_data_heizung *p)
{
	uint8_t i;
	solltemp_line_t *solltemp_line = (solltemp_line_t *) 
		&(p->config.zeitzone0_id);

	for (i = 0; i < 8; i++)
	{
		const uint8_t tr_id = solltemp_line[i].id;
		if (zeitzone_matches(tr_id))
		{
			// we got an zeitzone which matches
			return i;
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
 *
 */
void heizung_set_pwm_by_temp(device_data_heizung *p, int16_t soll, int16_t ist)
{
	// Beispiel: Soll = 20 Grad, Hysterese = 0.5 Grad
	// Wenn Heizung an ist und die Temperatur > 20.5 Grad, dann
	// Heizung ausschalten!
	if ((heizung_get_pwm(p) > 0) && (ist > (soll + p->config.hysterese)))
	{
		// Ventil schliessen, es ist in ca. 3 Minuten zu
		heizung_set_pwm(p,0);
		return;
	}

	// Beispiel: Soll = 20 Grad, Hysterese = 0.5 Grad
	// Wenn Heizung aus ist und die Temperatur < 19.5 Grad, dann
	// Heizung ein schalten!
	if ((heizung_get_pwm(p) == 0) && (ist < (soll - p->config.hysterese)))
	{
		// Ventil oeffnen, es ist in ca. 3 Minuten offen
		heizung_set_pwm(p,100);

		// die Ventilstellung erst in 3:30 Minuten bekanntgeben, damit die
		// Therme aufgrund der noch geschlossenen Ventile nicht taktet:
		p->waermebedarf_counter = 210;
		return;
	}
}

void heizung_check_ventilpflege(device_data_heizung *p)
{
	// Pruefen: ist die Heizung aus und keine Ventilpflege aktiv? 
	// Nur dann pruefen wir weiter, ob eine Ventilpflege ansteht:
	if ((p->mode == HEIZUNG_MODE_OFF) && (p->ventilpflege_counter == 0))
	{
		// Startzeitpunkt der Ventilpflege ist immer:
		// Sonntags, 12:00 + Heizungs-ID * 3
		//
		// Beispiel: ID: 67;  -> Startzeitpunkt: 3:21
		// Beispiel: ID: 160; -> Startzeitpunkt: 8:00

		uint16_t time = canix_rtc_clock.hour * 60 + canix_rtc_clock.minute;

		if ((canix_rtc_clock.day_of_week == 7) && // Sonntag
				(time == (p->config.id * 3) + (12*60 + 0)))
		{
			// Ventilpflege aktivieren:
			p->ventilpflege_counter = 600;
			canix_syslog_P(SYSLOG_PRIO_DEBUG,
					PSTR("heizung %d: ventilpflege"), p->config.id);
		}
	}
}

/**
 * this timer handler is called every second
 */
void heizung_timer_handler(device_data_heizung *p)
{
	if (p->ventilpflege_counter > 0)
		p->ventilpflege_counter--;

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
			// Zeit ist abgelaufen; nun wieder auf Automatik-Modus
			// stellen:
			//
			p->mode = HEIZUNG_MODE_AUTOMATIK;
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
	}

	/*
	 * Schaltet die HK-Ventile aus, wenn ein Heizstop vorliegt. Dies passiert
	 * in regelmaessigen Abstaenden, da eventuell aufgrund der Heizsteuerlogik
	 * die PWM nicht regelmaessig upgedatet wird und so das HK-Ventil nicht auf
	 * den Reedkontakt reagiert.
	 */
	heizung_update_heizstop(p);

	if (p->timer_counter++ >= 5)
	{
		p->timer_counter = 0;

		switch (p->mode)
		{
			case HEIZUNG_MODE_OFF : 
				{
					if (p->ventilpflege_counter)
					{
						// Sonderfall: Ventilpflege ist aktiv; dazu
						// Ventil einschalten, sonst aber nichts
						// unternehmen:
						
						p->manual_rate = 0;

						// Ventil einschalten:
						heizung_set_pwm(p,100);
						darlingtonoutput_setpin(p->config.port, 1);

						canix_syslog_P(SYSLOG_PRIO_DEBUG, 
								PSTR("ventilpflege: 1"));
					}
					else
					{
						// das ist Normalfall: keine Ventilpflege aktiv.
						p->manual_rate = 0;
						heizung_set_pwm(p,0);

						// Ventil ausschalten, falls es an ist:
						if (darlingtonoutput_getpin(p->config.port))
							darlingtonoutput_setpin(p->config.port, 0);
					}
					return;
				}
			case HEIZUNG_MODE_MANUAL :
				{
					heizung_set_pwm(p, p->manual_rate);
				}
				break;
			case HEIZUNG_MODE_AUTOMATIK :
				{
					// Wenn ein gueltiger Messwert vorliegt
					// (d.h. wenn innerhalb eines definierten Zeitraumes
					// ein Sensor-Messwert eingetroffen ist:
					if (p->received_interval_counter < 255)
					{
						int8_t index;
						index = heizung_get_matching_zeitzone_index(p);
						if (index != -1)
						{
							// we got an zeitzone which matches
							solltemp_line_t *solltemp_line = (solltemp_line_t*) 
								&(p->config.zeitzone0_id);
							heizung_set_pwm_by_temp(p, 
									solltemp_line[index].temp,
									p->measure_value);
						}
						else
						{
							// keine passende Zeitzone gefunden
							heizung_set_pwm(p,0);
						}
					}
					else
					{
						canix_syslog_P(SYSLOG_PRIO_ERROR, 
								PSTR("keine Sensor-Messwerte vorhanden!"));
					}
				}
				break;
			case HEIZUNG_MODE_THERMOSTAT :
				{
					// Wenn ein gueltiger Messwert vorliegt
					// (d.h. wenn innerhalb eines definierten Zeitraumes
					// ein Sensor-Messwert eingetroffen ist:
					if (p->received_interval_counter < 255)
					{
						heizung_set_pwm_by_temp(p, p->thermostat_temp,
								p->measure_value);
					}
					else
					{
						canix_syslog_P(SYSLOG_PRIO_ERROR, 
								PSTR("keine Sensor-Messwerte vorhanden!"));
					}
				}
				break;
		}
	}

	// Waermebedarfsmeldung versenden:
	if (p->waermebedarf_counter-- == 0)
	{
		canix_frame message;

		// Waermebedarfsmeldungen alle 60 Sekunden
		p->waermebedarf_counter = 60;

		// den aktuellen Ventilstand / Heizrate ermitteln:
		uint8_t rate = heizung_get_pwm(p);

		// Feature-Bit testen, und ggfls Waermebedarf verdoppeln:
		if (p->config.feature & (1<<HEIZUNG_FEATURE_DOPPEL_WAERMEBEDARF))
			rate = rate << 1;

		// Der Waermebedarf ist proportional zur Ventilstellung, d.h.
		// der Bedarf ist hoch, wenn das Ventil voll offen ist, und ist
		// nicht vorhanden, wenn das Ventil zu ist.

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
				int16_t sensor_temp = frame->data[3] << 8 | 
					frame->data[4];

				// store the measure value
				p->measure_value = sensor_temp;
				p->received_interval_counter = 0;
			}
			break;

		case HCAN_HES_HEIZUNG_SET_MODE_OFF :
			if (p->config.id == frame->data[2])
				p->mode = HEIZUNG_MODE_OFF;
			break;

		case HCAN_HES_HEIZUNG_SET_MODE_MANUAL :
			if (p->config.id == frame->data[2])
			{
				p->mode = HEIZUNG_MODE_MANUAL;
				p->manual_rate = frame->data[3];
				p->duration_counter = (frame->data[4] << 8) | frame->data[5];
			}
			break;

		case HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS :
			if (p->config.id == frame->data[2])
			{
				p->mode = HEIZUNG_MODE_THERMOSTAT;
				p->thermostat_temp = (frame->data[3] << 8) | frame->data[4];
				p->duration_counter = (frame->data[5] << 8) | frame->data[6];
			}
			break;

		case HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK:
			if (p->config.id == frame->data[2])
				p->mode = HEIZUNG_MODE_AUTOMATIK;
			break;

		case HCAN_HES_HEIZUNG_DETAILS_REQUEST :
			{
				if (p->config.id == frame->data[2])
				{
					switch (p->mode)
					{
						case HEIZUNG_MODE_OFF :
							answer.data[1] = 
								HCAN_HES_HEIZUNG_MODE_OFF_DETAILS;
							answer.data[2] = p->config.id;
							answer.size = 3;
							canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
							break;

						case HEIZUNG_MODE_MANUAL :
							answer.data[1] = 
								HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS;
							answer.data[2] = p->config.id;
							answer.data[3] = p->manual_rate;
							answer.data[4] = p->duration_counter >> 8;
							answer.data[5] = p->duration_counter;
							answer.size = 6;
							canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
							break;

						case HEIZUNG_MODE_THERMOSTAT :
							answer.data[1] = 
								HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS;
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
								int8_t index;
								index = heizung_get_matching_zeitzone_index(p);
								if (index != -1)
								{
									// we got an zeitzone which matches
									int16_t master_value = 0;
									solltemp_line_t *zeitzone = 
										(solltemp_line_t *) 
										&(p->config.zeitzone0_id);

									master_value = zeitzone[index].temp;
									answer.data[1] = 
										HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS;
									answer.data[2] = frame->data[2];
									answer.data[3] = master_value >> 8;
									answer.data[4] = master_value;
									answer.data[5] = 
										heizung_get_matching_zeitzone_id(p);
									answer.size = 6;
								}
								else
								{
									canix_syslog_P(SYSLOG_PRIO_ERROR,
											PSTR("zeitzone: no match!"));

									answer.data[1] = 
										HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS;
									answer.data[2] = frame->data[2];
									answer.data[3] = 0;
									answer.data[4] = 0;
									answer.data[5] = 0;
									answer.size = 6;
								}
								canix_frame_send_with_prio(&answer, 
										HCAN_PRIO_HI);
							}
							break;
					}
				}
			}
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
			if ((frame->data[2] == p->config.id) &&
					(frame->data[3] <= 7)) // Idx pruefen
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
			if ((frame->data[2] == p->config.id) &&
					(frame->data[3] <= 7)) // Idx pruefen
			{	
				uint8_t idx = frame->data[3];
				solltemp_line_t *solltemp_line;

				// EEPROM/EDS Validierungstests:
				if ((p->it != 0) && 
						(EDS_BLOCK_TYPE(p->it) == EDS_heizung_BLOCK_ID))
				{
					// neue Daten ins EEPROM schreiben; dazu:
					//  2 Bytes Offset fuer Typ + Size
					//  4 Byte  Offset fuer Ueberspringen der ersten
					//  Felder

					solltemp_line = (solltemp_line_t *) 
						&(p->config.zeitzone0_id);

					// ACHTUNG: zeitzone_id wird bisher NICHT geschrieben
					// Grund: bisher bei HomeCenter keine Notwendigkeit; somit
					// kann weniger kaputt gehen!
					// solltemp_line0_id (1 Byte)
					// solltemp_line[idx].id = frame->data[6];
					// eeprom_write_byte(p->it + 6 + (idx *3), frame->data[6]);

					// solltemp_line0_temp (2 Bytes)
					solltemp_line[idx].temp = (frame->data[4] << 8) |
						frame->data[5];
					eeprom_write_word((uint16_t*)(p->it + 6 + (idx *3) + 1),
							solltemp_line[idx].temp);

					answer.data[1] = HCAN_HES_HEIZUNG_SOLLTEMP_WRITE_LINE_ACK;
					answer.data[2] = p->config.id;
					answer.data[3] = idx;
					answer.size = 4;
					canix_frame_send_with_prio(&answer, HCAN_PRIO_LOW);
				}
			}
			break;

		case HCAN_HES_REEDKONTAKT_OFFEN : 
			{
				// Schauen, ob wir einen Heiz-Stop einlegen muessen, weil
				// eines unserer Fenster geoeffnet wurde:
				
				uint8_t i;

				for (i = 0; i < 8; i++) // es gibt 8 Reedkontakt-Eintrage...
				{
					uint8_t reed_gruppe;

					reed_gruppe = (&p->config.reed0)[i];


					if (reed_gruppe == frame->data[2])
					{	
						//canix_syslog_P(SYSLOG_PRIO_DEBUG,
						//	PSTR("HSp h=%d,r=%d"), p->config.id, reed_gruppe);

						if (p->reed_heiz_stop_counter == 0)
						{
							// nur zu Debugging Zwecke: das Fenster wurde
							// jetzt geoeffnet, der Heiz-Stop ist ab jetzt
							// aktiv:

							canix_syslog_P(SYSLOG_PRIO_DEBUG,
								PSTR("heiz-stop begin (heizung %d, reed %d)"),
								p->config.id, reed_gruppe);
						}

						// ja, es ist eines unserer Fenster; also Heiz-Stop
						// fuer eine Minute einlegen - falls das Fenster
						// weiter offen bleibt, kommt die naechste Reed-
						// Meldung auf jeden Fall innerhalb einer Minute
						// (Normalfall: alle 10 Sekunden)!
						p->reed_heiz_stop_counter = 60;

						// wenn mindst. ein Fenster offen ist, hat sich
						// die Sache jetzt erledigt
						return;
					}
				}
			}
			break;
	}
}

