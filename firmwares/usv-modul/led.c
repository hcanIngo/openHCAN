#include <led.h>
#include <avr/io.h>

/*****************

LED Konfiguration:

Die LEDs sind folgendermassen angeschlossen:

LED# Pin# Farbe     Funktion
----------------------------------------
0    7    gruen     U(puffer) OK
1    6    rot       1/10s Blitzen = Heartbeat
2    5    rot       nicht verwendet
3    4    gruen     NT Betrieb == an, Notstrombetrieb == aus
4    3    gelb      U(bat1+2) < Warning, blinkt im 2 Hz Betrieb
5    2    rot       U(bat1+2) < CRITICAL, blinkt im 2 Hz Betrieb
6    1    rot       USV auf Notstrombetrieb, dauerhaft an

Anordnung:
---------

    LED0   LED3
	LED1   LED4    LED6
	LED2   LED5

*/

#define LED0 7
#define LED1 6
#define LED2 5
#define LED3 4
#define LED4 3
#define LED5 2
#define LED6 1

uint8_t led_state[8];
uint8_t led_timer_counter;

void led_init()
{
	uint8_t i;

	// Port aktivieren; negative Logik: die LEDs bekommen 5V
	// und dert Port schaltet bei aktiver LED auf Masse
	DDRC = 255;
	PORTC = 255;

	led_timer_counter = 0;

	for (i = 0; i < 8; i++)
	{
		led_state[i] = 0;
	}
}

void led_set(uint8_t n, uint8_t state)
{
	// n auf Definitionsbereich eingrenzen:
	if (n > 7)
		n = 0;

	led_state[n] = state;
}

// schaltet die LED an (enable != 0) oder aus
void led_enable(uint8_t n, uint8_t enable)
{
	uint8_t pin = 0;

	// Anhand der LED Nummer den Pin festlegen:
	switch (n)
	{
		case 0 : pin = LED0; break;
		case 1 : pin = LED1; break;
		case 2 : pin = LED2; break;
		case 3 : pin = LED3; break;
		case 4 : pin = LED4; break;
		case 5 : pin = LED5; break;
		case 6 : pin = LED6; break;
		default : return;
	}

	// negative Logik: die LEDs bekommen 5V
	// und dert Port schaltet bei aktiver LED auf Masse
	if (enable != 0)
		PORTC &= ~(1<<pin);
	else
		PORTC |= (1<<pin);
}


void led_timer_handler()
{
	uint8_t i;

	led_timer_counter++;
	if (led_timer_counter >= 10)
	{
		led_timer_counter = 0;
	}

	for (i = 0; i < 7; i++)
	{
		led_enable(i, led_timer_counter < led_state[i]);
	}
}

