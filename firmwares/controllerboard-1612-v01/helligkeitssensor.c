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
#include <canix/canix.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include "helligkeitssensor.h"

/** Von helligkeitssensor.c werden helligkeitssensortelegramme gesendet. Inhalt ist der aktuelle ADC-Wert
 *  und die Gruppe des jeweilige konfigurierten helligkeitssensors.
 * 
 *  Es sind max. acht helligkeitssensors pro C1612-Board verfuegbar.
 *  (Acht ADC-Pins sind an den Ports PA0-PA7 = IN15-IN8 verfuegbar)
 */

inline static void enableAdc(void);
inline static void disableAdc(void);
static void setADMUX(uint8_t config_port);
static int16_t getADCvalue_inverted(void);

/** helligkeitssensor_init wird fuer alle konfigurierten helligkeitssensors einmal aufgerufen. */
void helligkeitssensor_init(device_data_helligkeitssensor *p, eds_block_p it)
{
	ADCSRA = (1<<ADPS1) | (1<<ADPS2); // set ADC prescaler to 3,686MHz / 64 =  57,6kHz
	p->secsUntilNextSend = (uint16_t) 60 * p->config.sendCycleMins;
	initFloatAvg(&(p->filterBrightness), 511); // initialisieren und mit 1023/2 fuellen
}

/** Der helligkeitssensor_timer_handler wird alle 1 s aufgerufen: Er liest den ADC-Wert des
 *  konfigurierten helligkeitssensors und sendet diesen auf den CAN-Bus. */
void helligkeitssensor_timer_handler(device_data_helligkeitssensor *p)
{		
	uint16_t currentADCaverage;
	
	if (p->secsUntilNextSend)
	{
		p->secsUntilNextSend--;

		// nur die letzten n Werte vorm Senden sind wichtig:
		if(p->secsUntilNextSend <= SIZE_OF_AVG)
		{
			setADMUX(p->config.port);
			addToFloatAvg(&(p->filterBrightness), getADCvalue_inverted());
		}
	}
	else
	{
		currentADCaverage = getOutputValue(&(p->filterBrightness));

		// sende das Helligkeitssensortelegramm:
		canix_frame message;
		message.src = canix_selfaddr();
		message.dst = HCAN_MULTICAST_CONTROL;
		message.proto = HCAN_PROTO_SFP;
		message.data[0] = HCAN_SRV_HES;

		message.data[1] = HCAN_HES_HELLIGKEITS_INFO; //z.B. fuer rolladenAutomat-Device
		message.data[2] = p->config.gruppe;
		message.data[3] = (uint8_t)((currentADCaverage & 0xFF00) >> 8); //high value
		message.data[4] = (uint8_t)currentADCaverage; //low value
		message.size = 5;
		canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);

		p->secsUntilNextSend = (uint16_t) 60 * p->config.sendCycleMins;
	}
}

/** setADMUX stellt den uC-internen Multiplexer entsprechend der Port- 
 *  Konfiguration des EDS-helligkeitssensor-Objekts ein.
 *  Acht ADC-Pins sind an den Ports PA0-PA7 = IN15-IN8 verfuegbar. */
static void setADMUX(uint8_t config_port)
{
	/* Defines for ADC-Mux with int. ref.-voltage:
	 * REFS0=1: internal 2.56V voltage reference */
	switch (config_port)
	{
		case 15 :
			ADMUX = (1<<REFS0) | 0; // ADC0 = 0 = 0b000
			break;
		case 14 :
			ADMUX = (1<<REFS0) | 1; // ADC1 = 1 = 0b001
			break;
		case 13 :
			ADMUX = (1<<REFS0) | 2; // ADC2 = 2 = 0b010
			break;
		case 12 :
			ADMUX = (1<<REFS0) | 3; // ADC3 = 3 = 0b011
			break;
		case 11 :
			ADMUX = (1<<REFS0) | 4; // ADC4 = 4 = 0b100
			break;
		case 10 :
			ADMUX = (1<<REFS0) | 5; // ADC5 = 5 = 0b101
			break;
		case  9 :
			ADMUX = (1<<REFS0) | 6; // ADC6 = 6 = 0b110
			break;
		case  8 :
			ADMUX = (1<<REFS0) | 7; // ADC7 = 7 = 0b111
			break;
		default: 
			canix_syslog_P(SYSLOG_PRIO_ERROR,
					PSTR("helligkeitssensor-port=%d. Richtig waere IN8 bis IN15."),
					config_port);
			break;
	}
}

static int16_t getADCvalue_inverted(void)
{
    uint8_t i;
	int16_t ADCresult;
    
	enableAdc(); // Aktivieren des ADC
	// Nach Aktivierung des ADC ein "Dummy-Readout" zum "Warmlaufen" lassen:
	ADCSRA |= (1<<ADSC); // eine ADC-Wandlung
    while( !(ADCSRA & (1<<ADIF)) ); // auf Abschluss der Konvertierung warten
    ADCresult = ADCW;


    ADCresult = 0;
    for(i=0; i<4; i++) // ADC-Wandlung 4 Mal durchfuehren -> bessere Genauigkeit
    {
		ADCSRA |= (1<<ADSC);
		while( !(ADCSRA & (1<<ADIF)) );

		ADCresult += ADCW; // Wandlungsergebnisse aufaddieren
    }

    ADCresult = ADCresult >> 2; // mitteln der 4 Messwerte
    //canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("adcRes=%d"), ADCresult);
	disableAdc();
    return 1023 - ADCresult; // schaltungsbedingt invertieren
}

inline static void enableAdc(void)
{
	// Aktivieren des ADC: Die interne Ref.spg. von 2.56V wird mit aktiviert
	ADCSRA |= (1<<ADEN);
}

inline static void disableAdc(void)
{
	// Deaktivieren, um Strom zu sparen: Die ADC-Ref. wird ebenfalls ausgeschaltet
	ADCSRA &= ~(1<<ADEN);
}
