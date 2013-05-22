#include <rolladen.h>
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
#include <log.h>

/***************************************************************************
 
	Relais-Schaltung fuer den Rolladenantrieb:

                               NC  
                               o-----> Rolladen hoch
             \                /
	L  ----o  o--------------o
          NO  COM          COM
	                           o-----> Rolladen runter
                               NO

      Power-Relais      Direction-Relais


Bedingungen:
------------
- Dir-Relais darf nach Stillstand fruehestens nach 500msec 
  gewechselt werden;  Counter: dir_blocking_counter
- Power-Relais darf nach Dir-Wechsel fruehestens nach 200msec
  eingeschaltet werden; Counter: power_blocking_counter

Steuerung:
----------
Es wird eine "Black-Box" Lowlevel-Steuerung realisiert:

  Steuerbefehle    +----------+  
  -------------->  |          |  ------> Dir-Relais
                   |          |
  Position         |          |
  <--------------  |          |  ------> Power-Relais
                   |          | 
  Status           |          |
  <--------------  |          |
                   +----------+

Steuerbefehle: AUF, AB, STOP, GOTO
Position:      0 = unten, 100 = oben
Status:        AUF, AB, STOP

Darueber wird eine Highlevel-Steuerung gesetzt:

   Taster-Events   +----------+
   ------------->  |          | -------> Steuerbefehle an Lowlevelbox
                   |          |
   Steuer-Events   |          | <------- Status von Lowlevelbox
   ------------->  |          |
                   |          |
				   +----------+
ACHTUNG: 

Die folgende Beschreibung beschreibt den 1-Taster-Betrieb!

Die Highlevel Steuererung funktioniert wie folgt:
- Taster down: Falls der Rolladen STOP ist:
	o falls er nicht am Ende ist, startet er in die zuletzt 
	  gewaehlte Richtung (AUF oder AB)
	o falls er am Ende ist, startet er in die jeweilige Richtung
	  (wenn er unten ist, nach oben, andernfalls umgekehrt)
	o wenn nach x msec noch kein "Taster up" Event kommt, dann
	  wechselt er die Richtung. Dabei wickelt die Lowlevel-Steuerung
	  die Wartezeiten selbststaenig ab, d.h. es wird nur einmal
	  ein Richtungswechsel an die Lowlevel-Steuerung gegeben
	o kommt das "Taster up" Event frueher als die x msec, so
	  wird es ignoriert
- Taster down: Falls der Rolladen laeuft:
	o wird ein STOP Befehl abgesetzt
	o wenn nach x msec noch kein "Taster up" Event kommt, dann
	  wird er in die andere Richtung gestartet. Dabei wickelt die 
	  Lowlevel-Steuerung die Wartezeiten selbststaenig ab, d.h. es 
	  wird nur einmal ein Richtungswechsel an die Lowlevel-Steuerung 
	  gegeben
	o kommt das "Taster up" Event frueher als die x msec, so
	  wird es ignoriert


Die Statemachine, die die Steuerung implementiert, ist in 
./rolladen-statemachine.png dokumentiert.

Erweiterung Positionsanfahren:

- Implementierung Positionsanfahren:
  Wenn der Rolladen steht: es wird die Solllaufzeit gespeichert und
  der Rolladen in die passende Richtung gestartet
- wenn die Solllaufzeit erreicht ist, stoppt der Rolladen
- der manuelle Eingriff hat immer Vorrang, d.h. wenn der Rolladen noch
  am Fahren ist und der Taster gedrueckt wird, wird die Solllaufzeit
  (vorgegeben durch den Positionsanfahren-Befehl) deaktiviert
- ebenso wird, sofern der Rolladen beim Empfang des Positionsanfahren-Befehls
  gerade faehrt, ignoriert

 */

void rolladen_init(device_data_rolladen *p, eds_block_p it)
{
	// wir wissen die aktuelle Position des Rolladen nicht
	// Also wird der Status so gesetzt, dass rekalibriert wird
	p->summe_laufzeit = 0; //p->config.max_rekalib + 1;

	p->laufzeit          = p->config.laufzeit; 
	
	p->soll_laufzeit     = -1; // initial keine Solllaufzeit!
	p->cmdsrc            = 0;
	p->lowlevel_command  = ROLLADEN_READY;
	p->lowlevel_state    = ROLLADEN_STATE_STOP;
	// Zur Sicherheit initial warten:
	p->lowlevel_wait_counter = 6;
	p->lowlevel_last_dir = ROLLADEN_DIR_AB;
	p->hilevel_up_event_counter = -1;
}

void rolladen_laufzeit_align(device_data_rolladen *p)
{
	if (p->laufzeit < 0)
		p->laufzeit = 0;
	if (p->laufzeit > p->config.laufzeit)
		p->laufzeit = p->config.laufzeit;
}

uint8_t rolladen_is_in_group(const device_data_rolladen *p, uint8_t group)
{
	if (p->config.taster == group)
		return 1;

	uint8_t i;
	uint8_t *gruppen;
	
	gruppen = (uint8_t *) &(p->config.gruppe0);

	for (i = 0; i < MAX_ROLLADEN_GROUPS; i++)
	{
		if (gruppen[i] == group)
			return 1;
	}

	return 0;
}

void rolladen_timer_handler(device_data_rolladen *p)
{
	// Die Lowlevel Steuerung:
	rolladen_lowlevel_timer_handler(p);
}

int32_t rolladen_min_laufzeit(device_data_rolladen *p)
{
	// 0 = unten: hier 50% noch ins negative
	return -((int32_t)p->config.laufzeit * 5 / 10); 
}

int32_t rolladen_max_laufzeit(device_data_rolladen *p)
{ 
	// 100 = oben: hier 50% drauf: 150%
	return ((int32_t)p->config.laufzeit * 15 / 10);
}

void rolladen_set_soll_laufzeit(device_data_rolladen *p, uint8_t percent)
{
	if (percent == 0)
	{
		// min = -50 % des konfigurierten Wertes
		p->soll_laufzeit = rolladen_min_laufzeit(p); 
	}
	else if (percent == 100)
	{
		// max = 150 % des konfigurierten Wertes
		p->soll_laufzeit = rolladen_max_laufzeit(p); 
	}
	else
	{
		p->soll_laufzeit = (int32_t)p->config.laufzeit * 
			(int32_t)percent / 100;
	}
}

void rolladen_send_changed_info(device_data_rolladen *p)
{
	canix_frame message;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_INFO;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO;
	message.data[2] = p->config.gruppe0;
	message.data[3] = rolladen_lowlevel_get_position(p);
	message.data[4] = p->cmdsrc >> 8;
	message.data[5] = p->cmdsrc & 0xff;
	message.size = 6;

	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}

void rolladen_can_callback(device_data_rolladen *p, const canix_frame *frame)
{
	canix_frame answer;
	uint8_t sollPos;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;


	switch (frame->data[1])
	{
		case HCAN_HES_TASTER_DOWN :
			if (p->config.taster == frame->data[2])
			{
				switch (p->lowlevel_state)
				{
					case ROLLADEN_STATE_STOP :
						switch (p->lowlevel_last_dir)
						{
							case ROLLADEN_DIR_AUF :
								p->lowlevel_command = ROLLADEN_COMMAND_AB;
								p->soll_laufzeit = -1;
								p->cmdsrc = 0; // Taster gedrueckt
								break;
							case ROLLADEN_DIR_AB :
								p->lowlevel_command = ROLLADEN_COMMAND_AUF;
								p->soll_laufzeit = -1;
								p->cmdsrc = 0; // Taster gedrueckt
								break;
						}
						break;
					case ROLLADEN_STATE_AUF :
						p->lowlevel_command = ROLLADEN_COMMAND_STOP;
						p->soll_laufzeit = -1;
						p->cmdsrc = 0; // Taster gedrueckt
						break;
					case ROLLADEN_STATE_AB :
						p->lowlevel_command = ROLLADEN_COMMAND_STOP;
						p->soll_laufzeit = -1;
						p->cmdsrc = 0; // Taster gedrueckt
						break;
				}
				p->hilevel_up_event_counter = 7; // 0.7 sec
			}
			break;

		case HCAN_HES_TASTER_UP :
			if (p->config.taster == frame->data[2])
			{
				p->hilevel_up_event_counter = -1;
			}
			break;

		case HCAN_HES_ROLLADEN_POSITION_SET :
			if ( rolladen_is_in_group(p,frame->data[2]) || 
			     // 2-Taster-Betrieb: Rollladen-auf/zu-Nachricht: 
			     ((frame->data[3] >= 200) && (p->config.taster == frame->data[2] || rolladen_is_in_group(p,frame->data[2]))) )
			{
				switch (p->lowlevel_state)
				{
					case ROLLADEN_STATE_STOP :
						if (frame->data[3] == 200)
						{
							sollPos = 0;
							p->cmdsrc = 0; // Taster gedrueckt
						}
						else if (frame->data[3] == 201) 
				{
							sollPos = 100;
							p->cmdsrc = 0; // Taster gedrueckt
						}
						else
						{
							sollPos = frame->data[3]; // in Prozent
							p->cmdsrc = frame->src; // Absender des Befehls vormerken
						}
					// Die Soll-Position ergibt sich aus der Gesamtstrecke
					// und der prozentualen Position:
						rolladen_set_soll_laufzeit(p, sollPos);

					// Nun den Rolladen in die richtige Richtung starten:
						if (rolladen_lowlevel_get_position(p) < sollPos)
					{
						// Wenn er ueber der Solllaufzeit ist:
						p->lowlevel_command = ROLLADEN_COMMAND_AUF;
					}
						else if (rolladen_lowlevel_get_position(p) > sollPos+1)
					{
						// Wenn er unter der Solllaufzeit ist:
						p->lowlevel_command = ROLLADEN_COMMAND_AB;
					}
					else
					{
						// er muss gleich sein; hier nichts machen!
					}
						break;

					case ROLLADEN_STATE_AUF :
					case ROLLADEN_STATE_AB :
						if (frame->data[3] < 200) // "normales" HCAN_HES_ROLLADEN_POSITION_SET
							break; /* Ein Position-Set-Befehl an eine Gruppe wird nur
								  ausgefuehrt, wenn der Rolladen nicht laeuft! */
						// Ausser es handelt sich um eine Taster-Rollladen-auf/zu-Nachricht,
						// im 2-Taster-Betrieb:
						p->lowlevel_command = ROLLADEN_COMMAND_STOP;
						p->soll_laufzeit = -1;
						p->cmdsrc = 0; // Taster gedrueckt
						break;
				}
			}
			break;

		case HCAN_HES_ROLLADEN_DEFINE_POSITION :
			{
				// Da die Rolladen keine Rueckmeldung geben, wo sie
				// gerade stehen, kann im Service-Falle (z.B. bei einem
				// Firmware Upgrade die Position der Rollaeden vorher
				// mit HCAN_HES_ROLLADEN_POSITION_REQUEST erfragt und
				// nach dem Upgrade mit HCAN_HES_ROLLADEN_DEFINE_POSITION
				// definiert werden.

				if (rolladen_is_in_group(p, frame->data[2]))
				{
					rolladen_lowlevel_define_position(p,frame->data[3]);
				}
			}
			break;

		case HCAN_HES_ROLLADEN_POSITION_REQUEST :
			if (rolladen_is_in_group(p, frame->data[2]))
			{
				answer.data[1] = HCAN_HES_ROLLADEN_POSITION_REPLAY;
				answer.data[2] = frame->data[2];
				answer.data[3] = rolladen_lowlevel_get_position(p);
				answer.size = 4;
				canix_frame_send_with_prio(&answer, HCAN_PRIO_HI);
			}
			break;
	}
}

/*
 * Lowlevel Steuerung - Details zur Implementierung:
 *
 * Fuer die oben beschriebenen Wartezeiten wird der dir_blocking_counter
 * und der power_blocking_counter verwendet. Beide werden alle 100msec
 * im timer handler aktualisiert: Wenn sie auf 0 stehen, sind sie abgelaufen
 * und die Aktion ist dann nicht mehr gesperrt.
 */

uint8_t rolladen_lowlevel_get_state(const device_data_rolladen *p)
{
	return p->lowlevel_state;
}

uint8_t rolladen_lowlevel_get_position(const device_data_rolladen *p)
{
	int32_t pos;

	pos = 100 * p->laufzeit / p->config.laufzeit + 1; // +1 wegen Rundungsfehler
	if (pos < 0)
		pos = 0;
	if (pos > 100)
		pos = 100;

	return pos;
}

void rolladen_lowlevel_define_position(device_data_rolladen *p, uint8_t pos)
{
	// (pos+1) wegen Rundungsfehler!
	p->laufzeit = (int32_t)(pos+1) * p->config.laufzeit / 100;

	if (p->laufzeit > p->config.laufzeit)
		p->laufzeit = p->config.laufzeit;
}


/* 
 * Die Statemachine, die die Steuerung implementiert, ist in 
 * ./rolladen-statemachine.png dokumentiert.
*/

void rolladen_lowlevel_timer_handler(device_data_rolladen *p)
{
	// Wait Counter decrementieren:
	if (p->lowlevel_wait_counter)
		p->lowlevel_wait_counter--;

	switch (p->lowlevel_state)
	{
		case ROLLADEN_STATE_STOP:
			if (p->lowlevel_command == ROLLADEN_COMMAND_AUF)
			{
				p->lowlevel_command = ROLLADEN_READY;
				darlingtonoutput_setpin(p->config.port_power, 1);
				p->lowlevel_wait_counter = 3;
				p->lowlevel_state = ROLLADEN_STATE_AUF_WAIT;
			}
			if (p->lowlevel_command == ROLLADEN_COMMAND_AB)
			{
				p->lowlevel_command = ROLLADEN_READY;
				darlingtonoutput_setpin(p->config.port_dir, 1);
				p->lowlevel_wait_counter = 2;
				p->lowlevel_state = ROLLADEN_STATE_AB_WAIT_1;
			}
			// Falls der Rolladen steht und in Gegenrichtung gestartet
			// werden soll:
			if (p->lowlevel_command == ROLLADEN_COMMAND_DIR_CHANGE)
			{
				p->lowlevel_command = ROLLADEN_READY;
				if (p->lowlevel_last_dir == ROLLADEN_DIR_AB)
					p->lowlevel_command = ROLLADEN_COMMAND_AUF;
				if (p->lowlevel_last_dir == ROLLADEN_DIR_AUF)
					p->lowlevel_command = ROLLADEN_COMMAND_AB;
			}
			break;
		case ROLLADEN_STATE_AUF_WAIT :
			if (p->lowlevel_wait_counter == 0)
			{
				p->lowlevel_state = ROLLADEN_STATE_AUF;
				p->lowlevel_last_dir = ROLLADEN_DIR_AUF;
			}
			break;
		case ROLLADEN_STATE_AUF :
			if (p->lowlevel_command == ROLLADEN_COMMAND_STOP)
			{
				p->lowlevel_command = ROLLADEN_READY;
				darlingtonoutput_setpin(p->config.port_power, 0);
				p->lowlevel_wait_counter = 3;
				p->lowlevel_state = ROLLADEN_STATE_STOP_WAIT_1;
			}
			if (p->lowlevel_command == ROLLADEN_COMMAND_DIR_CHANGE)
			{
				p->lowlevel_command = ROLLADEN_READY;
				darlingtonoutput_setpin(p->config.port_power, 0);
				p->lowlevel_wait_counter = 6;
				p->lowlevel_state = ROLLADEN_STATE_DIR_CHANGE_WAIT_1;
			}
			break;
		case ROLLADEN_STATE_STOP_WAIT_1 :
			if (p->lowlevel_wait_counter == 0)
			{
				p->lowlevel_state = ROLLADEN_STATE_STOP;
				rolladen_send_changed_info(p);

				// Wenn die Postion < 0 oder > Strecke, dann sie
				// an der Grenze alignen:
				rolladen_laufzeit_align(p);
			}
			break;
		case ROLLADEN_STATE_AB_WAIT_1 :
			if (p->lowlevel_wait_counter == 0)
			{
				darlingtonoutput_setpin(p->config.port_power, 1);
				p->lowlevel_wait_counter = 3;
				p->lowlevel_state = ROLLADEN_STATE_AB_WAIT_2;
			}
			break;
		case ROLLADEN_STATE_AB_WAIT_2 :
			if (p->lowlevel_wait_counter == 0)
			{
				p->lowlevel_state = ROLLADEN_STATE_AB;
				p->lowlevel_last_dir = ROLLADEN_DIR_AB;
			}
			break;
		case ROLLADEN_STATE_AB :
			if (p->lowlevel_command == ROLLADEN_COMMAND_STOP)
			{
				p->lowlevel_command = ROLLADEN_READY;
				darlingtonoutput_setpin(p->config.port_power, 0);
				p->lowlevel_wait_counter = 6;
				p->lowlevel_state = ROLLADEN_STATE_STOP_WAIT_2;
			}
			if (p->lowlevel_command == ROLLADEN_COMMAND_DIR_CHANGE)
			{
				p->lowlevel_command = ROLLADEN_READY;
				darlingtonoutput_setpin(p->config.port_power, 0);
				p->lowlevel_wait_counter = 6;
				p->lowlevel_state = ROLLADEN_STATE_DIR_CHANGE_WAIT_2;
			}
			break;
		case ROLLADEN_STATE_STOP_WAIT_2 :
			if (p->lowlevel_wait_counter == 0)
			{
				darlingtonoutput_setpin(p->config.port_dir, 0);
				p->lowlevel_wait_counter = 3;
				p->lowlevel_state = ROLLADEN_STATE_STOP_WAIT_3;
			}
			break;
		case ROLLADEN_STATE_STOP_WAIT_3 :
			if (p->lowlevel_wait_counter == 0)
			{
				p->lowlevel_state = ROLLADEN_STATE_STOP;
				rolladen_send_changed_info(p);

				// Wenn die Postion < 0 oder > Strecke, dann sie
				// an der Grenze alignen:
				rolladen_laufzeit_align(p);
			}
			break;
		case ROLLADEN_STATE_DIR_CHANGE_WAIT_1 :
			if (p->lowlevel_wait_counter == 0)
			{
				darlingtonoutput_setpin(p->config.port_dir, 1);
				p->lowlevel_wait_counter = 3;
				p->lowlevel_state = ROLLADEN_STATE_AB_WAIT_1;
			}
			break;
		case ROLLADEN_STATE_DIR_CHANGE_WAIT_2 :
			if (p->lowlevel_wait_counter == 0)
			{
				darlingtonoutput_setpin(p->config.port_dir, 0);
				p->lowlevel_wait_counter = 3;
				p->lowlevel_state = ROLLADEN_STATE_DIR_CHANGE_WAIT_3;
			}
			break;
		case ROLLADEN_STATE_DIR_CHANGE_WAIT_3 :
			if (p->lowlevel_wait_counter == 0)
			{
				darlingtonoutput_setpin(p->config.port_power, 1);
				p->lowlevel_wait_counter = 3;
				p->lowlevel_state = ROLLADEN_STATE_AUF_WAIT;
			}
			break;
	}


	// Strecken mitzaehlen
	if ((p->lowlevel_state == ROLLADEN_STATE_AUF_WAIT) ||
			(p->lowlevel_state == ROLLADEN_STATE_AUF))
	{
		p->summe_laufzeit++;
		p->laufzeit++;

		// Wenn eine Solllaufzeit angefahren werden soll und die aktuelle
		// Position groesser/gleich der Solllaufzeit ist (Rolladen faehrt ja
		// gerade hoch), dann anhalten und Solllaufzeit deaktivieren
		if ((p->soll_laufzeit != -1) && (p->laufzeit >= p->soll_laufzeit))
		{
			p->soll_laufzeit = -1;
			p->lowlevel_command = ROLLADEN_COMMAND_STOP;
		}


		if(p->summe_laufzeit < p->config.max_rekalib)
		{
			// pruefen, ob obere Endposition erreicht: 
			if(p->laufzeit >= p->config.laufzeit)
			{
				// obere Endabschaltung:
				p->lowlevel_command = ROLLADEN_COMMAND_STOP;
				return;
			}
		}
		else // obere Endabschaltung und Kalibrieren: 
		{
			// Wenn der Rolladen die 150% Hoehe erreicht hat,
			// ist er sicherlich schon oben und abgeschaltet worden.
			// Das kann als Kalibierwert verwendet werden:
			if (p->laufzeit > rolladen_max_laufzeit(p))
			{
				p->laufzeit = p->config.laufzeit;
				p->summe_laufzeit = 0;
				p->lowlevel_command = ROLLADEN_COMMAND_STOP;
				return;
			}
		}
	}
	if ((p->lowlevel_state == ROLLADEN_STATE_AB_WAIT_1) ||
			(p->lowlevel_state == ROLLADEN_STATE_AB_WAIT_2) ||
			(p->lowlevel_state == ROLLADEN_STATE_AB))
	{
		p->summe_laufzeit++;
		p->laufzeit--;

		// Wenn eine Solllaufzeit angefahren werden soll und die aktuelle
		// Position kleiner/gleich der Solllaufzeit ist (Rolladen faehrt ja
		// gerade runter), dann anhalten und Solllaufzeit deaktivieren
		if ((p->soll_laufzeit != -1) && (p->laufzeit <= p->soll_laufzeit))
		{
			p->soll_laufzeit = -1;
			p->lowlevel_command = ROLLADEN_COMMAND_STOP;
		}

		if (p->summe_laufzeit < p->config.max_rekalib)
		{
			// pruefen, ob untere Endposition erreicht: 
			if(p->laufzeit <= 0)
			{
				//untere Endabschaltung:
				p->lowlevel_command = ROLLADEN_COMMAND_STOP;
				return;
			}
		}
		else // untere Endabschaltung und Kalibrieren: 
		{   
			if (p->laufzeit < rolladen_min_laufzeit(p))
			{
				p->laufzeit = 0;
				p->summe_laufzeit = 0;
				p->lowlevel_command = ROLLADEN_COMMAND_STOP;
				return;
			}
		}
	}

	if (p->hilevel_up_event_counter > 0)
	{
		p->hilevel_up_event_counter--;
		if (p->hilevel_up_event_counter == 0)
		{
			// es ist kein HCAN_HES_TASTER_UP Event eingetroffen; daher
			// muss ein Richtungswechsel vorgenommen werden:
			p->lowlevel_command = ROLLADEN_COMMAND_DIR_CHANGE;
		}
	}
}

