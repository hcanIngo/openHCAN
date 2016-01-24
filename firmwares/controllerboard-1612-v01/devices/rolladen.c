#include "rolladen.h"
#include <canix/syslog.h>

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

#include "darlingtonoutput.h"

void rolladen_init(device_data_rolladen *p, eds_block_p it)
{
	p->kalibrieren = 1;
	// Wir wissen die aktuelle Position des Rolladen nicht.
	// Daher wird der Status so gesetzt, dass rekalibriert wird
	p->summe_laufzeit = p->config.max_rekalib + 1;
	p->laufzeit = p->config.laufzeit / 2; // und in der Mitte steht
	// sodass sowohl ein AUF- als auch ein AB-Fahrauftrag den Rolladen
	// vollstaendig AUF bzw. ZU faehrt.
	// Bei einem Position-Set-Fahrauftrag wird niemals kalibriert.

	// Abgeschlossen ist die Kalibrierung, wenn der Rolladen in Untenfahrt gestoppt wird.
	// Wird eine Kalibrierfahrt vor Erreichen der Untenlage gestoppt, so liegen die
	// virtuellen Endlagen falsch, der Rolladen kann nicht vollstaendig oeffnen.
	// Vorraussetzung ist selbstverstaendlich die realitaetsnahe Konfiguration der
	// tatsaechlichen Laufzeit.

	p->soll_laufzeit = -1; // kein Soll
	p->stoppuhr = 0;

	p->soll_dir = ROLLADEN_DIR_AUF; // Relais ist aus bei init
	p->last_dir = p->soll_dir; // 1-Taster-Betrieb
	p->dir = p->soll_dir;

	p->soll_power = 0;
	p->power = p->soll_power;

	p->change_dir_counter = -1; // abmelden (1-Taster-Betrieb)
	p->long_pressed_counter = -1; // abmelden (2-Taster-Betrieb)
}

static uint8_t rolladen_get_position(const device_data_rolladen *p)
{
	int32_t pos;
	pos = 100 * p->laufzeit / p->config.laufzeit + 1; // +1 wegen Rundungsfehler
	if (pos < 2) pos = 0;
	if (pos > 98) pos = 100;

	return pos;
}

static void rolladen_send_changed_info(device_data_rolladen *p, uint8_t pos)
{
	canix_frame message;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_INFO;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO;
	message.data[2] = p->config.taster;
	message.data[3] = pos; // aktuelle Position in %
	message.data[4] = 0; // Quelle des cmd: nicht verwendet
	message.data[5] = 0; // Quelle des cmd: nicht verwendet
	message.size = 6;

	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}

// Liefert 1, falls der Rolladen in der gegebenen Gruppe ist
static uint8_t is_group_in_rolladen(const device_data_rolladen *p, uint8_t group)
{
	if (p->config.taster == group) return 1;

	uint8_t i;
	uint8_t *gruppen = (uint8_t *) &(p->config.gruppe0);
	for (i = 0; i < MAX_ROLLADEN_GROUPS; i++)
		if (gruppen[i] == group) return 1;

	return 0; // nicht gefunden
}

static void calibration(device_data_rolladen *p)
{
	if (p->config.feature & (1<<FEATURE_KALIBRIERUNG_OBEN))
	{
		if (p->soll_laufzeit > p->config.laufzeit)
		{
			// in Oben-Lage kalibrieren:
			p->laufzeit = p->config.laufzeit; // Obenlage erreicht
			canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("co"));

			p->summe_laufzeit = 0; // Erst nach Untenabschaltung die Kalibrierung beenden!
			p->kalibrieren = 0;
		}
	}
	else if (p->soll_laufzeit < -1)
	{
		// in Unten-Lage kalibrieren:
		p->laufzeit = 0; // Untenlage erreicht
		canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("cu"));

		p->summe_laufzeit = 0; // Erst nach Untenabschaltung die Kalibrierung beenden!
		p->kalibrieren = 0;
	}
}

static void rolladen_cmd_stop(device_data_rolladen *p)
{
	uint8_t pos = rolladen_get_position(p);
	if(pos == 0 || pos == 100) // untere oder obere Endlage erreicht?
	{
		// canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("r"));
		p->last_dir = !p->dir; // Richtungswechsel (1-Taster-Betrieb)
		p->change_dir_counter = -1; // abmelden (1-Taster-Betrieb)
	}

	if(p->kalibrieren) calibration(p);

	canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("dir=%d"), p->dir);
	canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("t=%d"), p->stoppuhr); // Stopp in 0=AUF, 1=ZU
	p->stoppuhr = 0;
	canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("lz=%d"), p->laufzeit);
	rolladen_send_changed_info(p, pos);

	p->soll_power = 0;
	p->exe_power = 1; // Halt-Auftrag: Power-Relais sofort aus

	p->soll_dir = ROLLADEN_DIR_AUF; // nur um das Relais abzuschalten
	// Wegen Dir-Umschaltung verzoegern?
	if (ROLLADEN_DIR_AB == p->dir)
	{
		p->blockingTimer = 5; // = 0,5 s (Dir-Relais sicher umschalten)
		p->exe_dir = 0; // erst nach Zeitablauf
	}
	// else Dir-Relais liegt schon richtig

	p->soll_laufzeit = -1; // kein Soll
}

static void rolladen_cmd_drive(device_data_rolladen *p)
{
	p->soll_power = 1; // soll fahren

	// Power-Ein verzoegern?
	if (p->soll_dir == ROLLADEN_DIR_AB)
	{
		p->exe_dir = 1; // Fahr-Auftrag: Dir-Relais sofort um-/einschalten

		p->blockingTimer = 5; // = 0,5 s (Dir-Relais sicher umschalten)
		p->exe_power = 0; // erst nach Zeitablauf
	}
	else // ROLLADEN_DIR_AUF: Dir-Relais liegt schon richtig
		p->exe_power = 1; // da das Dir-Relais schon richtig liegt
}

static void extend_soll_laufzeit(device_data_rolladen *p)
{
	if (p->config.feature & (1<<FEATURE_KALIBRIERUNG_OBEN))
	{
		if(ROLLADEN_DIR_AUF == p->soll_dir)
		{
			canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("eo"));
			p->soll_laufzeit = ((int32_t)p->config.laufzeit * 150 / 100); // 100 = oben: hier 150% drauf: 200%
			p->kalibrieren = 1;
		}
	}
	else // unten kalibrieren:
	{
		if(ROLLADEN_DIR_AB == p->soll_dir)
		{
			canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("eu"));
			p->soll_laufzeit = -((int32_t) p->config.laufzeit * 50 / 100); // 0 = unten: hier 50% noch ins negative
			p->kalibrieren = 1;
		}
	}
}

static uint8_t set_soll_laufzeit(device_data_rolladen *p, uint8_t soll_dir)
{
	if(ROLLADEN_DIR_AB == soll_dir)
	{
		p->soll_laufzeit = 0; // ZU = 0 %
		if (p->laufzeit <= p->soll_laufzeit)
		{
			p->soll_laufzeit = -1;
			return 0; // soll-Position liegt schon vor
		}

		p->soll_dir = soll_dir;
		if (p->summe_laufzeit >= p->config.max_rekalib) // kalibrieren?
			extend_soll_laufzeit(p); // Fuer die Kalibrierung
	}
	else // AUF:
	{
		p->soll_laufzeit = p->config.laufzeit; // AUF = 100 %
		if (p->laufzeit >= p->soll_laufzeit)
		{
			p->soll_laufzeit = -1;
			return 0; // soll-Position liegt schon vor
		}

		p->soll_dir = soll_dir;
		if (p->summe_laufzeit >= p->config.max_rekalib) // kalibrieren?
			extend_soll_laufzeit(p); // Fuer die Kalibrierung
	}

	return 1;
}

void rolladen_can_callback(device_data_rolladen *p, const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;

	switch (frame->data[1])
	{
		case HCAN_HES_ROLLADEN_POSITION_SET:
			if (is_group_in_rolladen(p, frame->data[2]))
			{
				switch (frame->data[3])
				{
					case 200: // Taster-Down im 2-Taster-Betrieb: ZU = 0 %
						if (p->blockingTimer) return; // Befehl abblocken

						if (p->power || p->soll_power)
							rolladen_cmd_stop (p); // anhalten, falls der Rolladen faehrt
						else if (set_soll_laufzeit(p, ROLLADEN_DIR_AB))
						{
							// noch nicht in soll-Position:
							p->long_pressed_counter = 10; // 1 s
							rolladen_cmd_drive (p);
						}
						break;

					case 201: // Taster-Down im 2-Taster-Betrieb: AUF = 100 %
						if (p->blockingTimer) return; // Befehl abblocken

						if (p->power || p->soll_power)
							rolladen_cmd_stop (p); // anhalten, falls der Rolladen faehrt
						else if (set_soll_laufzeit(p, ROLLADEN_DIR_AUF)) // noch nicht in soll-Position?
						{
							// noch nicht in soll-Position:
							p->long_pressed_counter = 10; // 1 s
							rolladen_cmd_drive (p);
						}
						break;

					case 202: // HALT ueber Web-Interface
						p->long_pressed_counter = -1; // abmelden (2-Taster-Betrieb)
						rolladen_cmd_stop (p);
						break;

					case 222: // Taster-Up im 2-Taster-Betrieb
						p->long_pressed_counter = -1; // abmelden
						break;

					default: // frame->data[3] = Positionsvorgabe in %
						if (p->blockingTimer) return; // Befehl abblocken

						p->soll_laufzeit = (int32_t)p->config.laufzeit * frame->data[3] / 100;
						canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("s %d"), p->soll_laufzeit);
						if (p->soll_laufzeit < 0) p->soll_laufzeit = 0;
						if (p->soll_laufzeit > p->config.laufzeit) p->soll_laufzeit = p->config.laufzeit;

						if (p->power || p->soll_power || p->soll_laufzeit == p->laufzeit)
						{
							p->soll_laufzeit = -1;
							return; // ignorieren, falls der Rolladen faehrt  oder  die Soll-Pos. schon hat
						}

						p->kalibrieren = 0; // nicht kalibrieren
						if (p->soll_laufzeit < p->laufzeit)	p->soll_dir = ROLLADEN_DIR_AB;
						else p->soll_dir = ROLLADEN_DIR_AUF;
						rolladen_cmd_drive (p);
				}
			}
			break;

		case HCAN_HES_TASTER_DOWN: // Nur im 1-Taster-Betrieb verwendet
			if (is_group_in_rolladen(p, frame->data[2]))
			{
				if (p->blockingTimer) return; // Befehl abblocken

				if (p->power)
				{
					p->change_dir_counter = 7; // 0,7 s (1-Taster-Betrieb: Richtungswechsel, falls lange gedrueckt)
					rolladen_cmd_stop (p); // anhalten, falls der Rolladen faehrt
				}
				else
				{
					set_soll_laufzeit(p, p->last_dir);
					if (p->soll_laufzeit != -1) // liegt soll-Position bereits vor?
						rolladen_cmd_drive (p);
				}
			}
			break;

		case HCAN_HES_TASTER_UP: // Nur im 1-Taster-Betrieb verwendet
			if (is_group_in_rolladen(p, frame->data[2]))
				p->change_dir_counter = -1; // abmelden (1-Taster-Betrieb)
			break;

		case HCAN_HES_ROLLADEN_DEFINE_POSITION:
			{
				// Da die Rolladen keine Rueckmeldung geben, wo sie
				// gerade stehen, kann im Service-Falle (z.B. bei einem
				// Firmware Upgrade die Position der Rollaeden vorher
				// mit HCAN_HES_ROLLADEN_POSITION_REQUEST erfragt und
				// nach dem Upgrade mit HCAN_HES_ROLLADEN_DEFINE_POSITION
				// definiert werden.

				if (is_group_in_rolladen(p, frame->data[2]))
				{
					// Rolladenposition setzen: (pos+1) wegen Rundungsfehler
					p->laufzeit = (int32_t)(frame->data[3]+1) * p->config.laufzeit / 100;

					if (p->laufzeit > p->config.laufzeit) p->laufzeit = p->config.laufzeit;
				}
			}
			break;

		case HCAN_HES_ROLLADEN_POSITION_REQUEST:
			if (is_group_in_rolladen(p, frame->data[2]))
			{
				answer.data[1] = HCAN_HES_ROLLADEN_POSITION_REPLAY;
				answer.data[2] = frame->data[2];
				answer.data[3] = rolladen_get_position(p);
				answer.size = 4;
				canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
			}
			break;
	}
}

// hier werden die anstehenden Rolladenauftraege jede 10-tel Sekunde verarbeitet/angestossen
inline void rolladen_timer_handler(device_data_rolladen *p, uint8_t zyklus)
{
	if (zyklus != 10) return; // 10tel-Sekunden-Zyklus verwendet

	//// Im 2-Taster-Betrieb manuell die Kalibrierung anstossen:
	if (p->long_pressed_counter > 0)  p->long_pressed_counter--;
	else if (p->long_pressed_counter == 0)
	{
		p->kalibrieren = 1; // kalibrieren per Taster-"ZU"
		extend_soll_laufzeit (p);
		p->long_pressed_counter = -1; // abmelden
	}

	//// Richtungswechsel im 1-Taster-Betrieb:
	if (p->change_dir_counter > 0)  p->change_dir_counter--;
	else if (p->change_dir_counter == 0)
	{
		p->last_dir = !p->last_dir; // Richtungswechsel
		p->change_dir_counter = -1; // abmelden
	}


	//// Soll-/Endlage-Pruefung und ggf. Abschaltung:
	if (p->power && p->soll_power) // Rolladen faehrt und hat keinen Halt-Auftrag?
	{
		if(ROLLADEN_DIR_AB == p->dir)
		{
			p->laufzeit--; // Rolladen faehrt runter
			if (p->laufzeit <= p->soll_laufzeit) rolladen_cmd_stop (p); // Halt-Auftrag durch das Erreichen der Soll-Lage
		}
		else // AUF:
		{
			p->laufzeit++; // Rolladen faehrt hoch
			if (p->laufzeit >= p->soll_laufzeit) rolladen_cmd_stop (p); // Halt-Auftrag durch das Erreichen der Soll-Lage
		}

		p->summe_laufzeit++; // fuer die Kalibrierung
	}



	//// Relais-Blocking:
	if (p->blockingTimer) p->blockingTimer--; // Blockierung aktiv
	else
	{
		if (p->soll_power) p->exe_power = 1; // Fahr-Auftrag: Power-Relais darf nun eingeschaltet werden
		else p->exe_dir = 1; // Halt-Auftrag: Dir-Relais darf nun umgeschaltet werden
	}


	//// Relais-Execution:
	if (p->exe_dir && (p->dir != p->soll_dir)) // Aenderung?
	{
		darlingtonoutput_setpin(p->config.port_dir, p->soll_dir);
		p->dir = p->soll_dir; // Relais nur bei Aenderung ansteuern
		p->exe_dir = 0; // Default: Relais nicht ansprechen
	}

	if (p->exe_power && (p->power != p->soll_power)) // Aenderung?
	{
		darlingtonoutput_setpin(p->config.port_power, p->soll_power);
		p->power = p->soll_power; // Relais nur bei Aenderung ansteuern
		p->exe_power = 0; // Default: Relais nicht ansprechen
	}

	if (p->power) p->stoppuhr++;
}
