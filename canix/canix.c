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
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 *
 *  Adapted for ATmega644:
 *  (c) 2010 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 */
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include <canix/canix.h>
#include <canix/mcp2515.h>
#include <canix/led.h>
#include <canix/rtc.h>
#include <hcan.h>

#ifdef CANIX_HMS
#include <canix/hcan_hms.h>
#endif

#ifdef CANIX_RTS
#include <canix/hcan_rts.h>
#endif

#ifdef CANIX_SYSLOG
#include <canix/syslog.h>
#endif

#ifdef CANIX_EDS
#include <canix/eds.h>
#endif

#include <hcan_multicast.h>

/**
 * Canix stellt ein (H)CAN Interface zur Verfuegung. Jeder Client, der
 * HCAN-Frames empfangen moechte, kann einen ueber
 * canix_reg_frame_callback(..) registieren. Die Filter-Daten (Ziel,
 * Protokoll, Service) werden zusammen mit der Adresse der Callback-Funktion
 * in Instanzen dieser Struktur gespeichert.
 */

typedef struct
{
	canix_frame_callback callback;
	int dst;
	uint8_t proto;
	uint8_t srv;
} canix_frame_callback_entry;

/**
 * Dieses Array haelt alle registrierten HCAN Callback Funktionen inklusive
 * den Filter-Kriterien.
 */
static canix_frame_callback_entry frame_callback_entry[MAX_FRAME_CALLBACKS];

/**
 * Dieses Array haelt alle registrierten Idle-Handler Callback Funktionen.
 */
static canix_idle_callback idle_callback[MAX_IDLE_CALLBACKS];

/**
 * Dieses Array haelt alle registrierten RTC Callback Funktionen.
 */
static canix_rtc_callback  rtc_callback[MAX_RTC_CALLBACKS];


/**
 * Hier sind die privaten Daten der RTC abgelegt.
 */
volatile canix_rtc_clock_t canix_rtc_clock;

volatile int32_t canix_sleep_ticker;

volatile uint16_t canix_loop_throughput_counter;
volatile uint16_t canix_loop_throughput;

volatile uint32_t canix_rx_received = 0;
volatile uint32_t canix_rx_lost = 0;
volatile uint32_t canix_tx_sent = 0;
volatile uint32_t canix_tx_dropped = 0;

volatile canix_rx_queue_entry_t canix_rx_queue[CANIX_QUEUE_SIZE];

uint8_t canix_rxq_size(void)
{
	uint8_t i; 
	uint8_t c = 0;

	for (i = 0; i < CANIX_QUEUE_SIZE; i++)
		if (canix_rx_queue[i].used)
			c++;

	return c;
}

/**
 * Liefert einen index auf der RX Queue, an der sich ein Frame befindet.
 * Dieser kann dann ausgelesen werden
 */
uint8_t canix_rxq_fetch_one(void)
{
	uint8_t i; 

	for (i = 0; i < CANIX_QUEUE_SIZE; i++)
		if (canix_rx_queue[i].used)
			return i;

	// Falls nichts drin ist, die erste Stelle liefern; es muss
	// eh mit canix_rxq_size() vorher geprueft werden!
	return 0;
}

/**
 * Liefert einen index auf der RX Queue, an der sich kein Frame befindet.
 * An dieser Stelle kann dann ein neuer Frame gespeichert werden.
 */
uint8_t canix_rxq_put_one(void)
{
	uint8_t i; 

	for (i = 0; i < CANIX_QUEUE_SIZE; i++)
		if (!canix_rx_queue[i].used)
			return i;

	// Falls nichts mehr frei ist, die erste Stelle liefern; es sollte
	// eh mit canix_rxq_size() vorher auf Platz geprueft werden!
	return 0;
}

#define CANIX_VERSION "0.9.5"

/**
 * CANIX Versionsstring
 */
const char canix_version[] PROGMEM = "CANIX_VERSION_" CANIX_VERSION "_";

/**
 * Diese Funktion wird von canix_init() aufgerufen und initialisiert
 * den RTC Timer.
 */


/** 
 * Der RTC Timer verwendet das TCNT0 Register, das mit diesem Wert geladen
 * wird. Dadurch entsteht eine IRQ-Frequenz von ca. 3.6kHz, d.h.  die ISR wird
 * alle 271 us aufgerufen.
 */

#define F_CPU_REFERENCE      3686400
volatile uint8_t myClockBasedTCNT0 = 0;
volatile uint8_t myWaitingPeriod   = 0;

void canix_rtc_init(void)
{
	canix_rtc_clock.uptime_sec_100th = 0;
	canix_rtc_clock.uptime_sec = 0;

	canix_rtc_clock.day_of_week = 0;
	canix_rtc_clock.hour = 0;
	canix_rtc_clock.minute = 0;
	canix_rtc_clock.second = 0;
	canix_rtc_clock.last_date_update = 255;
	canix_rtc_clock.day_of_month = 0;
	canix_rtc_clock.month_of_year = 0;
	canix_rtc_clock.year = 0;

	// Timer programmieren:
#ifdef MCU_atmega32
	TCCR0 = (1<<CS00)|(1<<CS02); // Divider 1024
	TIMSK = (1<<TOIE0); // Timer0 Interrupt einschalten
#elif MCU_atmega644
	TCCR0B = (1<<CS00)|(1<<CS02); // Divider
	TIMSK0 = (1<<TOIE0); // Timer0 Interrupt einschalten
#endif
	// Timter Counter loest immer bei 255 -> 0 den
	// Interrupt aus; f = 3686 Hz
	myClockBasedTCNT0 = (uint8_t)((uint32_t)(F_CPU+F_CPU_REFERENCE-1)/(uint32_t)F_CPU_REFERENCE); 

	TCNT0 = -myClockBasedTCNT0;

	myWaitingPeriod = (uint8_t)(F_CPU/1024/myClockBasedTCNT0/100) - 2;
}

/**
 * Diese Funktion aktualisiert die Zeit und das Datum.
 * Aufgerufen wird sie jede Sekunde durch die ISR des Timer0.
 */
void canix_update_datetime(void)
{
	if (++canix_rtc_clock.second == 60)
	{
		canix_rtc_clock.second = 0;
		if (++canix_rtc_clock.minute == 60)
		{
			canix_rtc_clock.minute = 0;
			if (canix_rtc_clock.last_date_update < 255)
				canix_rtc_clock.last_date_update++;

			if (++canix_rtc_clock.hour == 24)
			{
				canix_rtc_clock.hour = 0;

				// ein neuer Tag; jetzt das Datum aktualisieren
				if (++canix_rtc_clock.day_of_week == 8)
					canix_rtc_clock.day_of_week = 1;

				canix_rtc_clock.day_of_month++;

				// testen, ob ein Monatsueberlauf aufgetreten ist
				uint8_t ndays = 0;
				switch (canix_rtc_clock.month_of_year)
				{
					case 1 : ndays = 31; break;
					case 2 : ndays = 28; break;
					case 3 : ndays = 31; break;
					case 4 : ndays = 30; break;
					case 5 : ndays = 31; break;
					case 6 : ndays = 30; break;
					case 7 : ndays = 31; break;
					case 8 : ndays = 31; break;
					case 9 : ndays = 30; break;
					case 10 : ndays = 31; break;
					case 11 : ndays = 30; break;
					case 12 : ndays = 31; break;
				}
				if (canix_rtc_clock.day_of_month > ndays)
				{
					canix_rtc_clock.day_of_month = 1;
					if (++canix_rtc_clock.month_of_year > 12)
					{
						canix_rtc_clock.month_of_year = 1;
						canix_rtc_clock.year++;
					}
				}
			}
		}
	}
}

/**
 * Liefert zurueck, ob die Zeit / das Datum "frisch" ist, d.h. ob
 * innerhalb des erlaubten Intervals (254 Sekunden) ein Zeitframe
 * empfangen wurde. Falls die Zeit nicht frisch ist, haengt die 
 * Praezession allein von der Genauigkeit des Quarzes ab.
 */
uint8_t canix_is_date_valid(void)
{
	if (canix_rtc_clock.last_date_update != 255)
		return 1;
	else
		return 0;
}

/**
 * Timer0 ISR. Hier wird werden alle 100 Ticks die Sekunde inkrementiert,
 * canix_update_datetime() aufgerufen und jeden Tick (also alle 10msec)
 * alle RTC Callbacks aufgerufen. Dadurch koennen Clients auch
 * zeitgesteuerte Dinge in einer Frequenz bis 100Hz durchfuehren.
 *
 * Die Routine selbst wird mit 3600 Hz aufgerufen; das ist noetig, um
 * den MCP zu pollen: Design-"Schwaeche": kein IRQ vom MCP zum Atmega
 * geroutet :-(
 *
 */
ISR(TIMER0_OVF_vect)
{
	uint8_t i;

	TCNT0 = -myClockBasedTCNT0;

	/* 
	 * MCP pollen:
	 *
	 * Durch das Pollen mit 3.6kHz gehen im Mittel (ueber einige
	 * Wochen gemittelt) nur rund 0.002% Pakete verloren (!!)
	 *
	 */
	if (mcp2515_message_available())
	{
		uint32_t extended;
		uint8_t size;
		uint8_t data[8];
		uint8_t idx;

		size = mcp2515_read_message(&extended, 
				(char *)&data[0]);

		if (canix_rxq_size() < CANIX_QUEUE_SIZE)
		{
			idx = canix_rxq_put_one();

			canix_rx_queue[idx].used = 1;
			canix_rx_queue[idx].extended = extended;
			canix_rx_queue[idx].size = size;
			for (i = 0; i < size; i++)
				canix_rx_queue[idx].data[i] = data[i];

			canix_rx_received++;
		}
		else
			canix_rx_lost++;
	}
	

	// alle 35 Calls den RTC Kram behandlen -> somit ergibt sich eine
	// Rate von 100 Hz!
	//
	//
	static uint8_t rtc_clock_counter = 0;

	if (rtc_clock_counter++ > myWaitingPeriod)
	{
		// Counter zuruecksetzen
		rtc_clock_counter = 0;

		if ((++canix_rtc_clock.uptime_sec_100th) == 100)
		{
			canix_rtc_clock.uptime_sec_100th = 0;
			canix_rtc_clock.uptime_sec++;

			canix_loop_throughput = canix_loop_throughput_counter;
			canix_loop_throughput_counter = 0;

			canix_update_datetime();
		}

		// canix rtc callbacks aufrufen:

		for (i = 0; i < MAX_RTC_CALLBACKS; i++)
		{
			if (rtc_callback[i])
				rtc_callback[i]();
		}

		if (canix_sleep_ticker > 0)
			canix_sleep_ticker--;
	}
}

uint8_t canix_init(void)
{
	uint8_t i;
	uint8_t res;

	// RTC init
	canix_rtc_init();

	// init callbacks
	for (i = 0; i < MAX_FRAME_CALLBACKS; i++)
		frame_callback_entry[i].callback = (canix_frame_callback) 0;
	for (i = 0; i < MAX_IDLE_CALLBACKS; i++)
		idle_callback[i] = (canix_idle_callback) 0;
	for (i = 0; i < MAX_RTC_CALLBACKS; i++)
		rtc_callback[i] = (canix_rtc_callback) 0;

#ifdef CANIX_LED_SETUP1
	canix_led_init_ports();
#endif
#ifdef CANIX_LED_SETUP2
	canix_led_init_ports();
#endif

	// Eingangsbuffer leeren
	for (i = 0; i < CANIX_QUEUE_SIZE; i++)
		canix_rx_queue[i].used = 0;

	spi_init();
	res = can_open_interface();

#ifdef CANIX_HMS
	// HMS Stuff
	canix_reg_frame_callback(canix_SFP_HMS_handler, canix_selfaddr(),
			HCAN_PROTO_SFP, HCAN_SRV_HMS);
	canix_reg_frame_callback(canix_SFP_HMS_handler, HCAN_MULTICAST_INFO,
			HCAN_PROTO_SFP, HCAN_SRV_HMS);
#endif

#ifdef CANIX_RTS
	// RTS Stuff (Real Time Service, just passive receiving)
	canix_reg_frame_callback(canix_SFP_RTS_handler, HCAN_MULTICAST_INFO,
			HCAN_PROTO_SFP, HCAN_SRV_RTS);
	canix_reg_frame_callback(canix_SFP_RTS_handler, canix_selfaddr(),
			HCAN_PROTO_SFP, HCAN_SRV_RTS);
#endif

#ifdef CANIX_SYSLOG
	// enable syslog debug messages
	canix_set_syslog_level(eeprom_read_byte((uint8_t *)EEPR_DEBUG_LEVEL));
#endif

	sei();

#ifdef CANIX_EDS
	eds_init();

	canix_reg_frame_callback(eds_can_callback, canix_selfaddr(), 
			HCAN_PROTO_SFP, HCAN_SRV_EDS);
#endif

	return res;
}

uint16_t canix_selfaddr(void)
{
	static uint16_t my_address = 0;

	if (!my_address)
	{
		my_address = (eeprom_read_byte((uint8_t *)EEPR_SELF_ADDR) |
		eeprom_read_byte((uint8_t *)(EEPR_SELF_ADDR + 1)) << 8) & 0x3ff;
	}
	return my_address;
}

void canix_deliver_frame(const canix_frame *frame)
{
	uint8_t i;

	// nachschauen, ob dafuer ein callback registiert ist:
	for (i = 0; i < MAX_FRAME_CALLBACKS; i++)
	{
		if (frame_callback_entry[i].callback)
		{
			// dieser Callback ist installiert; nun schauen,
			// ob er passt:
			if ((frame_callback_entry[i].dst == -1) ||
					((frame_callback_entry[i].dst != -1) &&
					 (frame_callback_entry[i].dst == frame->dst)))
			{
				// Adressfilter haben gematched oder stehen auf ANY;
				
				// Schauen, ob das Protokoll und der
				// Service passen:
				if (
						((frame_callback_entry[i].proto == 0) || 
						(frame_callback_entry[i].proto == frame->proto)) &&
						((frame_callback_entry[i].srv == 0) ||
						(frame_callback_entry[i].srv == frame->data[0])))
				{
					frame_callback_entry[i].callback(frame);
				}
			}
		}
	}
}

void canix_frame_send_with_prio(const canix_frame *frame, uint8_t prio)
{
	// Nachschauen, ob der TX Buffer des MCP frei ist:
	
	if (can_any_sendable())
	{
		// Frame ueber CAN versenden ...
		uint32_t extid;
		extid = (frame->src & ADDR_MASK) |
			((uint32_t)(frame->dst & ADDR_MASK) << 10) |
			((uint32_t)(frame->proto & PROTO_MASK) << 20) |
			((uint32_t)(prio) << 24);
		can_send_message(extid, frame->data, frame->size);
	}

	// und jetzt noch lokal zustellen - auch wenn das Frame in die TX Queue
	// gestellt wurde:
	canix_deliver_frame(frame);
}

void canix_frame_send(const canix_frame *frame)
{
	canix_frame_send_with_prio(frame, HCAN_PRIO_LOW);
}

void canix_reg_frame_callback(canix_frame_callback callback, 
		int dst, uint8_t proto, uint8_t srv)
{
	uint8_t i;

	for (i = 0; i < MAX_FRAME_CALLBACKS; i++)
	{
		if (frame_callback_entry[i].callback == 0)
		{
			frame_callback_entry[i].callback = callback;
			frame_callback_entry[i].dst      = dst;
			frame_callback_entry[i].proto    = proto;
			frame_callback_entry[i].srv      = srv;
			return;
		}
	}

#ifdef CANIX_SYSLOG
	canix_syslog_P(SYSLOG_PRIO_CRITICAL, PSTR("reg_frame: no more handlers"));
#endif
}

void canix_reg_idle_callback(canix_idle_callback callback)
{
	uint8_t i;

	for (i = 0; i < MAX_IDLE_CALLBACKS; i++)
	{
		if (idle_callback[i] == 0)
		{
			idle_callback[i] = callback;
			return;
		}
	}

#ifdef CANIX_SYSLOG
	canix_syslog_P(SYSLOG_PRIO_CRITICAL, PSTR("idle: no more handlers"));
#endif
}

void canix_reg_rtc_callback(canix_rtc_callback callback)
{
	uint8_t i;

	for (i = 0; i < MAX_RTC_CALLBACKS; i++)
	{
		if (rtc_callback[i] == 0)
		{
			rtc_callback[i] = callback;
			return;
		}
	}

#ifdef CANIX_SYSLOG
	canix_syslog_P(SYSLOG_PRIO_CRITICAL, PSTR("rtc: no more handlers"));
#endif
}

void canix_process_messages(void)
{
	uint8_t i;

	for (i = 0; i < MAX_IDLE_CALLBACKS; i++)
	{
		if (can_message_available())
		{
			uint32_t extid;
			canix_frame frame;

			// Frame einlesen
			frame.size = can_read_message(&extid, (char *) &(frame.data[0]));
			frame.dst = (extid >> 10) & ADDR_MASK;
			frame.src = extid & ADDR_MASK;
			frame.proto = (extid >> 20) & PROTO_MASK;

			canix_deliver_frame(&frame);
		}
		else if (idle_callback[i])
				idle_callback[i]();
	}
}

void canix_mainloop(void)
{
	canix_loop_throughput_counter = 0;

	while (1)
	{
		wdt_reset();
		canix_process_messages();

		canix_loop_throughput_counter++;
	}
}

void canix_reset(void)
{
	wdt_enable(WDTO_15MS);
	while (1);
}

void canix_sleep_100th(uint8_t n)
{
	static uint8_t running = 0;

	// Diese Funktion ist NICHT reentrant; sonst gibts Stack-Ragout ;-)
	// Daher pruefen, und ggfl. beenden:

	if (running)
		return;

	running = 1;

	canix_sleep_ticker = n;
	while (1)
	{
		canix_process_messages();

		if (canix_sleep_ticker == 0)
		{
			running = 0;
			return;
		}
	}
}

