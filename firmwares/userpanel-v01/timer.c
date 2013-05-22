#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "timer.h"
#include <keys.h>
#include <lcdstatemachine.h>

volatile uint8_t timer_counter_100th = 0;
volatile uint8_t timer_counter_10th = 0;
volatile uint8_t timer_counter_1 = 0;

volatile uint8_t timer_event_100th = 0;
volatile uint8_t timer_event_10th = 0;
volatile uint8_t timer_event_1 = 0;

// Achtung: RTC Timer Handler werden von einer ISR aufgerufen
// daher hier nichts machen, was wiederum Interrupts ausloesen koennte
void timer_handler(void)
{
	timer_counter_100th++;
	timer_event_100th = 1;

	if (timer_counter_100th == 10)
	{
		// 10msec Takt
		timer_counter_10th++;
		timer_counter_100th = 0;
		timer_event_10th = 1;
	}

	if (timer_counter_10th == 10)
	{
		// 100msec Takt
		timer_counter_1++;
		timer_counter_10th = 0;
		timer_event_1 = 1;
	}
}

void idle_handler(void)
{

	// 10msec Takt:
	if (timer_event_100th)
	{
		// Event Flag zuruecksetzen
		timer_event_100th = 0;
		keys_handle_pressed();	
	}

	// 100msec Takt
	if (timer_event_10th)
	{
		// Event Flag zuruecksetzen
		timer_event_10th = 0;
	}

	// 1sec Takt
	if (timer_event_1)
	{
		// Event Flag zuruecksetzen
		timer_event_1 = 0;

		lcdstatemachine_timer_handler();
	}
}

