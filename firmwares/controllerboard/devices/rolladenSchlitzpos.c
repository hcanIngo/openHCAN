/*
 *  This file is part of the HCAN tools suite.
 *
 *  HCAN is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  HCAN is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with HCAN; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *  (c) 2010 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 */
#include "../../controllerboard/devices/rolladenSchlitzpos.h"

#include <hcan.h>

#include <canix/canix.h>
#include <canix/tools.h>
#include <canix/syslog.h>
#include <canix/tools.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include <hcan_multicast.h>
#include "../../controllerboard/devices.h"

/** rolladenSchlitzpos ist ein virtuelles device. Es wird als
 *  Mittler zwischen einem Reedkontakt und einem Rollladen konfiguriert.
 *
 *  Falls ein Rollladen geschlossen ist und dann das Fenster(Reedkontakt)
 *  geoeffnet wird, erhaelt der Rollladen einen Auftrag zum Schlitzposition
 *  anfahren. Es wird angenommen, dass gelueftet werden soll.
 *
 *  Ebenso wird der Rollladen wieder zugefahren, wenn das Fenster(Reedkontakt)
 *  wieder geschlossen wird. Zum Aendern der Fensterposition von Kipp auf
 *  ganz geoffnet ist zwei Sekunden Zeit, ohne das die Schlitzposition
 *  des Rollladens verlassen wird. */

void rolladenSchlitzpos_init(device_data_rolladenSchlitzpos *p, eds_block_p it)
{
	p->rolladenSchlitzJob = SCHLITZ_JOB_ABGEMELDET;
	p->state1stReedkontakt = 255;
}

void send_rolladen_position_set(device_data_rolladenSchlitzpos *p, uint8_t soll_position)
{
	canix_frame message;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
	message.data[2] = p->config.rolladen;
	message.data[3] = soll_position; //gewuenschte Rollladenposition: z.B. Pos: 0=zu, SCHLITZ_POS
	message.size = 4;

	canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);
}

inline void rolladenSchlitzpos_timer_handler(device_data_rolladenSchlitzpos *p, uint8_t zyklus)
{
	if (zyklus != 10) return; // 10tel-Sekunden-Zyklus verwendet

	canix_frame message;

	if(p->rolladenSchlitzJob < SCHLITZ_JOB_ABGEMELDET) //ist der Job angemeldet
	{
		p->rolladenSchlitzJob--; //es sind wieder 100 ms vergangen
		//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("p->rolladenSchlitzJob=%d"), p->rolladenSchlitzJob);
		if(p->rolladenSchlitzJob == 0)
		{
			p->rolladenSchlitzJob = SCHLITZ_JOB_ABGEMELDET;

			/* Warte zwei Sekunden. Falls das Fenster wieder geoeffnet wird,
			 * wird der rolladenSchlitzJob wieder abgemeldet,
			 * die HCAN_HES_REEDKONTAKT_STATE_QUERY nicht gesendet
			 * und somit der Rollladen nicht geschlossen.
			 * ...und dann in HCAN_HES_REEDKONTAKT_STATE_REPLAY die Abfrage,
			 * ob der Reedkontakt immer noch zu ist: */
			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_CONTROL;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_HES;

			message.data[1] = HCAN_HES_ROLLADEN_POSITION_REQUEST;
			message.data[2] = p->config.rolladen;
			message.size = 3;
			canix_frame_send_with_prio(&message, HCAN_PRIO_HI); //rolladenIstposition-Abfrage

			//und Reedkontakt(e) abfragen:
			message.data[1] = HCAN_HES_REEDKONTAKT_STATE_QUERY;
			message.data[2] = p->config.reed0;
			message.size = 3;
			canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);

			if(p->config.reed1 != 255) //zweiter Reedkontakt vorhanden?
			{
				message.data[2] = p->config.reed1;
				canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);
			}
		}
	}
}

void rolladenSchlitzpos_can_callback(device_data_rolladenSchlitzpos *p,
		const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_REEDKONTAKT_STATE_CHANGE : //benoetigt Reedkontakt-Modus = 2
			if ( (p->config.reed0 == frame->data[2]) ||
				 (p->config.reed1 == frame->data[2]) ) //Reedkontakt einem Rollladen zugeordnet?
			{
				p->rolladenSchlitzJob = SCHLITZ_JOB_TIMER_LOAD; //Job soll in SCHLITZ_JOB_TIMER_LOAD Sekunden starten
			}
			break;
		case HCAN_HES_ROLLADEN_POSITION_REPLAY :
			if (p->config.rolladen == frame->data[2]) //Rolladen einem Reedkontakt zugeordnet?
			{
				p->rolladenIstposition = frame->data[3];
			}
			break;
		case HCAN_HES_REEDKONTAKT_STATE_REPLAY :
			if ( (p->config.reed0 == frame->data[2]) ||
				 (p->config.reed1 == frame->data[2]) ) //Reedkontakt einem Rollladen zugeordnet?
			{
				if(p->config.reed1 == 255) //Nur reed0 ist konfiguriert?
				{
					if (frame->data[3]) //status: Fenster/Reedkontakt auf
					{
						if( p->rolladenIstposition < (SCHLITZ_POS-5) ) //rolladenIstposition < SCHLITZ_POS
							send_rolladen_position_set(p, SCHLITZ_POS);
					}
					else //status: Fenster/Reedkontakt zu
					{
						if ( (p->rolladenIstposition >= SCHLITZ_POS-2) && //rolladenIstposition == SCHLITZ_POS
							 (p->rolladenIstposition <= SCHLITZ_POS+2) )
							send_rolladen_position_set(p, 0); //Pos=0=Rolladen zu
					}
				}
				else  //zweiter Reedkontakt vorhanden:
				{
					if (p->state1stReedkontakt == 255) //noch kein Reedkontakt hat sich gemeldet?
					{
						p->state1stReedkontakt = frame->data[3]; //nur den Zustand speichern
						break; //erst mit zweiter Reedkontaktrueckmeldung weiter
					}
					else //Rueckmeldung vom zweiten Reedkontakt verarbeiten:
					{
						if ( frame->data[3] || p->state1stReedkontakt ) //status: Mindestens ein Fenster/Reedkontakt ist auf
						{
							if( p->rolladenIstposition < (SCHLITZ_POS-5) ) //rolladenIstposition < SCHLITZ_POS
								send_rolladen_position_set(p, SCHLITZ_POS);
						}
						else //status: Fenstere/Reedkontakte beide zu
						{
							if ( (p->rolladenIstposition >= SCHLITZ_POS-2) && //rolladenIstposition == SCHLITZ_POS
								 (p->rolladenIstposition <= SCHLITZ_POS+2) )
								send_rolladen_position_set(p, 0); //Pos=0=Rolladen zu
						}

						p->state1stReedkontakt = 255; //wieder ruecksetzen
					}
				}
			}
			break;
		/*case HCAN_HES_ROLLADEN_POSITION_SET :
			if (p->config.rolladen == frame->data[2]) //Rolladen einem Reedkontakt zugeordnet?
			{
				if (frame->data[3] == 0) //Rolladen soll ganz schließen
					send_rolladen_position_set(p, SCHLITZ_POS);
			}
			break;*/
	}
}
