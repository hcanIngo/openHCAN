#include <inttypes.h>
#include <avr/io.h>
#include <darlingtonoutput.h>
#include <canix/canix.h>
#include <hcan.h>
#include <canix/tools.h>
#include <led.h>

/**
 * Implementiert die USV State Machine. Der State-Handler wird in
 * regelmaessigen Abstaenden aufgerufen.
 *
 * Aufteilung der ADC Channels:
 * - ADC0 : U1 : U(NT) [0..30V]
 * - ADC1 : U2 : U(Bat1) [12 .. 13.6V]
 * - ADC2 : U3 : U(Bat1+Bat2) [24 .. 27.2V]
 * - ADC3 : U4 : U(puffer) [0..27.2V]
 * - ADC4 : U5 : U(I) [0..5V] (Ampere-Meter)
 *
 *
 * - Darlington Output 7 : Relais fuer Netz/USV-Batterie Wechsel
 */

#define U1 0
#define U2 1
#define U3 2
#define U4 3
#define U5 4

uint8_t on_battery = 0;
uint16_t usv_timer = 0;
uint16_t switch_back_counter = 0;
uint8_t led_timer = 5;

#define U_SIZE 5
uint16_t U_min[U_SIZE];
uint16_t U_max[U_SIZE];

uint16_t adc_reach_channel(uint8_t mux)
{
	uint8_t i;
	uint16_t result = 0;
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);   
	// Frequenzvorteiler
	// setzen auf 128 und ADC aktivieren (1)

	ADMUX = mux;                      // Kanal waehlen

	/* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
	   also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */

	ADCSRA |= (1<<ADSC);              // eine ADC-Wandlung
	while ( ADCSRA & (1<<ADSC) );     // auf Abschluss der Konvertierung warten

	/* Eigentliche Messung - Mittelwert aus 4 aufeinanderfolgenden Wandlungen */
	for (i=0;i<16;i++)
	{
		ADCSRA |= (1<<ADSC);            // eine Wandlung "single conversion"
		while ( ADCSRA & (1<<ADSC) );   // auf Abschluss der Konvertierung warten
		result += ADCW;                 // Wandlungsergebnisse aufaddieren
	}
	ADCSRA &= ~(1<<ADEN);             // ADC deaktivieren (2)

	result >>= 4;

	// Messwert korrigieren; aus unerfindlichem Grund ist jeder Messwert
	// ca. 7% zu niedrig...
	result = result * 1.075;

	return result;
}

uint16_t U_bat(void)
{
	return adc_reach_channel(U3);
}

uint16_t U_puffer(void)
{
	return adc_reach_channel(U4);
}

uint16_t U_NT(void)
{
	return adc_reach_channel(U1);
}

void switch_to_battery(void)
{
	canix_frame message;

	// Status merken:
	on_battery = 1;


	// Relais schalten
	darlingtonoutput_setpin(7,1);

	// Timer auf 2 Sekunden setzen, d.h. fruehstens in
	// 2 Sekunden kann die USV wieder reagieren. Das ist noetig
	// damit kein "Prellen" oder "Flackern" passiert
	usv_timer = 200;

	// LEDs anpassen:
	led_set(3,0);
	led_set(6,10);

	// Busteilnehmer informieren:
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_INFO;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_USVS;
	message.data[1] = HCAN_USVS_STATE_INFO;
	message.data[2] = on_battery;
	message.size = 3;
	canix_frame_send(&message);
}

void switch_to_netzbetrieb(void)
{
	canix_frame message;

	// Status merken:
	on_battery = 0;

	// Relais schalten
	darlingtonoutput_setpin(7,0);

	// Auch hier wieder: ein Prellen verhindern
	usv_timer = 10;

	// LEDs anpassen:
	led_set(3,10);
	led_set(6,0);

	// Busteilnehmer informieren:
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_INFO;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_USVS;
	message.data[1] = HCAN_USVS_STATE_INFO;
	message.data[2] = on_battery;
	message.size = 3;
	canix_frame_send(&message);
}

void usv_reset_U_stats(void)
{
	uint8_t i;

	for (i = 0; i < U_SIZE; i++)
	{
		U_min[i] = 1023;
		U_max[i] = 0;
	}
}


// Messbereich: 0..30V; 1023 -> 30V
#define _21_V 760
#define _23_V 784
#define _24_V 819
#define U_BAT_WARNING 836
#define U_BAT_CRITICAL 829

// Wird 100 mal pro Sekunde aufgerufen
void usv_timer_handler(void)
{
	static uint8_t timer_counter = 0;

	// wird mit 100Hz = 10msec decrementiert
	if (usv_timer > 0)
		usv_timer--;

	// alle 100msec den LED timer handler aufrufen:
	if (led_timer-- == 0)
	{
		led_timer = 9;

		led_timer_handler();
	}

	// Sekundenrythmus:
	if (timer_counter-- == 0)
	{
		timer_counter = 100;

		// switch_back_counter jede Sekunde dekrementieren:
		if (switch_back_counter > 0)
			switch_back_counter--;
	}
}

void usv_state_handler(void)
{
	// Stats aktualisieren:
	//
	
	uint8_t i;
	for (i = 0; i < U_SIZE; i++)
	{
		uint16_t U;
		U = adc_reach_channel(i);
		if (U > U_max[i])
			U_max[i] = U;
		if (U < U_min[i])
			U_min[i] = U;
	}

	if (usv_timer)
		return;

	// Bricht die Versorungsspannung am Puffer-Kondensator zusmmen ?
	if (U_puffer() <= _23_V)
	{
		// ja; ist die Batterie ausreichend geladen ?
		if (U_bat() > _24_V)
		{
			// ja; also, falls noch nicht geschehen, auf 
			// Batteriebetrieb schalten
			if (! on_battery)
			{
				switch_to_battery();

				// Fuehestens nach 30sec zurueckschalten
				// um Takten bei Ueberlast
				// zu verhindern
				switch_back_counter = 30;
			}
		}
		else
		{
			// nein; auf Netzbetrieb schalten, denn sonst wird die Batterie
			// leerlaufen - wenn der Strom wiederkommt, sollte wieder
			// alles hochkommen !
			if ( on_battery)
				switch_to_netzbetrieb();
		}
	}
	else
	{
		// Ist der Strom da ?
		if ((U_NT() > _24_V) && (switch_back_counter == 0)) // Hysterese
		{
			// ja; also wieder auf Netzbetrieb schalten, sofern 
			// wir auf Batteriebetrieb sind:
			if ( on_battery)
				switch_to_netzbetrieb();
		}
	}

	// LEDs aktualisieren:
	led_set(0, U_puffer() >= _23_V ? 10 : 0);
	led_set(2, U_NT() < _23_V ? 10: 0 );
	led_set(4, U_bat() < U_BAT_WARNING ? 3 : 0 );
	led_set(5, U_bat() < U_BAT_CRITICAL ? 5 : 0 );
}

void usv_can_callback(const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_USVS;

	switch (frame->data[1])
	{
		case HCAN_USVS_STATE_REQUEST :
			{
				answer.data[1] = HCAN_USVS_STATE_INFO;
				answer.data[2] = on_battery;
				answer.size = 3;

				canix_frame_send(&answer);
			}
			return;
		case HCAN_USVS_VOLTAGE_REQUEST :
			{
				// "- 1" : U1-U5 wird abgebildet auf 0..4
				const uint8_t n = frame->data[2] - 1;

				if (n < 5)
				{
					uint16_t U = adc_reach_channel(n);
					answer.data[1] = HCAN_USVS_VOLTAGE_REPLAY;
					answer.data[2] = U >> 8;
					answer.data[3] = U;
					answer.size = 4;

					canix_frame_send(&answer);
				}
			}
			return;
		case HCAN_USVS_VOLTAGE_STATS_REQUEST :
			{
				// "- 1" : U1-U5 wird abgebildet auf 0..4
				const uint8_t n = frame->data[2] - 1;

				if (n < 5)
				{
					answer.data[1] = HCAN_USVS_VOLTAGE_STATS_REPLAY;
					answer.data[2] = frame->data[2];
					answer.data[3] = U_min[n] >> 8;
					answer.data[4] = U_min[n];
					answer.data[5] = U_max[n] >> 8;
					answer.data[6] = U_max[n];
					answer.size = 7;

					canix_frame_send(&answer);
				}
			}
			return;
		case HCAN_USVS_VOLTAGE_STATS_RESET :
			{
				usv_reset_U_stats();
			}
			return;
	}
}

