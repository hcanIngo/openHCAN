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
 *  (c) 2015 by Ingo Lages, i (dot) lages (at) gmx (dot) de
 */
#include "analogComparator.h"
#include <canix/canix.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef MCU_atmega32
	#define AC_REG SFIOR
	#define AC_INT ADC_vect
#elif MCU_atmega644
	#define AC_REG ADCSRB
	#define AC_INT ANALOG_COMP_vect
#endif


/**
 *  Es ist ein analogComparator pro C1612-Board verfuegbar.
 *  Der positive Comparator-Eingang ist AIN0.
 *  Der negative Comparator-Eingang ist AIN1. */

static inline void sendMessage(uint8_t active, uint8_t anCompPowerport);
static volatile uint8_t abschaltGruppe;

/** analogComparator_init wird fuer den konfigurierten analogComparator einmal aufgerufen. */
void analogComparator_init(device_data_analogComparator *p, eds_block_p it)
{
	AC_REG &= ~(1<<ACME); // Multiplexer deaktivieren
	// -> negativer Comparator-Eingang ist AIN1 = PB3

	canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("I"));
	PORTB &= ~ (0x0f | (1<< PB2) | (1<< PB3)); 	// Darlington Ports ausschalten
	// und fuer PB2 und PB3 den Pullup ausschalten
	DDRB &= ~ (1<< PB2); // Modus Input setzen
	DDRB &= ~ (1<< PB3); // Modus Input setzen
#ifdef MCU_atmega32
#elif MCU_atmega644
	DIDR1 |= (1<< AIN0D); // Deaktivieren des digitalen Eingangspuffers
	DIDR1 |= (1<< AIN1D); //      "
#endif

	abschaltGruppe = p->config.abschaltGruppe;
}

// Interrupt handler for ANALOG_COMP_vect
ISR(AC_INT)
{
	if (ACSR & (1<< ACO)) // AIN0 (+) < AIN1 (-)  →  ACO = 0 (fallende ACO Flanke)
	{
		// canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("F"));
	}
	else // AIN0 (+) > AIN1 (-)  →  ACO = 1 (steigende ACO Flanke)
	{
		ACSR &= ~(1<<ACIE); // Comparator-Interrupt deaktivieren
		ACSR |= (1<<ACD);   // 1 -> Comparator-Vcc Aus
		sendHESMessage (2, HCAN_HES_POWER_GROUP_OFF, abschaltGruppe); // Strom -> Kraft zu gross -> abschalten
	}
}

void analogComparator_can_callback(device_data_analogComparator *p, const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_POWER_GROUP_STATE_INFO : // verschickt der Powerport bei Aenderung
			if (frame->data[2] == p->config.aktivierenGruppe &&
				1 == frame->data[3]) // Powerport aktiviert?
			{
				canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("A"));
				ACSR &= ~(1<<ACD);   // 0 -> Comparator-Vcc Ein
				ACSR &= ~(1<<ACBG);	 // 0 = externe Referenzspannung (an Pin AIN0); 1 = interne (1,23 Volt)
				ACSR &= ~(1<<ACIC);  // 0 = input capture disabled
				ACSR &= ~(1<<ACIS1); // 0 = set interrupt on output toggle
				ACSR &= ~(1<<ACIS0); // 0 =  "      "      "   "      "

				ACSR |=  (1<<ACIE);  // 1 = Comparator-Interrupt aktivieren
			}
			break;
	}
}

static inline void sendMessage(uint8_t active, uint8_t anCompPowerport)
{
	canix_frame message;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = active; // HCAN_HES_POWER_GROUP_ON oder HCAN_HES_POWER_GROUP_OFF
	message.data[2] = anCompPowerport;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}
