

// ------- AUTOMATICLY GENERATED FROM mk_timer_c.xsl -----------------

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <canix/mem_check.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "timer.h"
#include "timeservice.h"
#include "dcf77_receiver.h"
#include "devices.h"

volatile uint8_t timer_counter_100th = 0;
volatile uint8_t timer_counter_10th = 0;
volatile uint8_t timer_counter_1 = 0;

volatile uint8_t timer_event_100th = 0;
volatile uint8_t timer_event_10th = 0;
volatile uint8_t timer_event_1 = 0;

// Achtung: RTC Timer Handler werden von einer ISR aufgerufen
// daher hier nichts machen, was wiederum Interrupts ausloesen koennte
void
timer_handler(void)
{
    timer_counter_100th++;
    timer_event_100th = 1;

    if (timer_counter_100th == 10) {
	// 10msec Takt
	timer_counter_10th++;
	timer_counter_100th = 0;
	timer_event_10th = 1;
    }

    if (timer_counter_10th == 10) {
	// 100msec Takt
	timer_counter_1++;
	timer_counter_10th = 0;
	timer_event_1 = 1;
    }
}

// Fuer die timer_handler gilt:
// 1: 1th, 10: 10th, 100: 100th -Sekundenaufruftakt
void
idle_handler(void)
{
    uint8_t         i;
    // uint16_t unusedSRAM; //in bytes

    // 10msec Takt:
    if (timer_event_100th) {
	// Event Flag zuruecksetzen
	timer_event_100th = 0;

	if (expanderActive)
	    handlerForExpanderUpdate();
	else if (portsDeviceCreated)
	    configurePorts();	// Ausnahme: EMV-Sicherung

	for (i = 0; i < MAX_PDEVICE_DATA; i++) {
	    uint8_t        *p = pdevice_data[i];
	    if (p) {
		switch (*p) {

		case EDS_analogComparator_BLOCK_ID:
		    analogComparator_timer_handler((device_data_analogComparator *) p, 100);
		    break;

		case EDS_heizung_BLOCK_ID:
		    heizung_timer_handler((device_data_heizung *) p, 100);
		    break;

		case EDS_helligkeitssensor_BLOCK_ID:
		    helligkeitssensor_timer_handler((device_data_helligkeitssensor *) p, 100);
		    break;

		case EDS_lichtzone_BLOCK_ID:
		    lichtzone_timer_handler((device_data_lichtzone *) p,
					    100);
		    break;

		case EDS_multitaster_BLOCK_ID:
		    multitaster_timer_handler((device_data_multitaster *)
					      p, 100);
		    break;

		case EDS_ports_BLOCK_ID:
		    ports_timer_handler((device_data_ports *) p, 100);
		    break;

		case EDS_poti_BLOCK_ID:
		    poti_timer_handler((device_data_poti *) p, 100);
		    break;

		case EDS_powerportAutomat_BLOCK_ID:
		    powerportAutomat_timer_handler((device_data_powerportAutomat *) p, 100);
		    break;

		case EDS_powerport_BLOCK_ID:
		    powerport_timer_handler((device_data_powerport *) p,
					    100);
		    break;

		case EDS_reedkontakt_BLOCK_ID:
		    reedkontakt_timer_handler((device_data_reedkontakt *)
					      p, 100);
		    break;

		case EDS_rolladenAutomat_BLOCK_ID:
		    rolladenAutomat_timer_handler((device_data_rolladenAutomat *) p, 100);
		    break;

		case EDS_rolladen_BLOCK_ID:
		    rolladen_timer_handler((device_data_rolladen *) p,
					   100);
		    break;

		case EDS_rolladenSchlitzpos_BLOCK_ID:
		    rolladenSchlitzpos_timer_handler((device_data_rolladenSchlitzpos *) p, 100);
		    break;

		case EDS_schalter_BLOCK_ID:
		    schalter_timer_handler((device_data_schalter *) p,
					   100);
		    break;

		case EDS_taster_BLOCK_ID:
		    taster_timer_handler((device_data_taster *) p, 100);
		    break;

		case EDS_tempsensor_BLOCK_ID:
		    tempsensor_timer_handler((device_data_tempsensor *) p,
					     100);
		    break;

		case EDS_zeitschaltuhr_BLOCK_ID:
		    zeitschaltuhr_timer_handler((device_data_zeitschaltuhr
						 *) p, 100);
		    break;

		case EDS_zeitzone_BLOCK_ID:
		    zeitzone_timer_handler((device_data_zeitzone *) p,
					   100);
		    break;

		case EDS_zentralheizungspumpe_BLOCK_ID:
		    zentralheizungspumpe_timer_handler((device_data_zentralheizungspumpe *) p, 100);
		    break;

		}
	    }
	}
    }
    // 100msec Takt
    if (timer_event_10th) {
	// Event Flag zuruecksetzen
	timer_event_10th = 0;

	for (i = 0; i < MAX_PDEVICE_DATA; i++) {
	    uint8_t        *p = pdevice_data[i];
	    if (p) {
		switch (*p) {

		case EDS_analogComparator_BLOCK_ID:
		    analogComparator_timer_handler((device_data_analogComparator *) p, 10);
		    break;

		case EDS_heizung_BLOCK_ID:
		    heizung_timer_handler((device_data_heizung *) p, 10);
		    break;

		case EDS_helligkeitssensor_BLOCK_ID:
		    helligkeitssensor_timer_handler((device_data_helligkeitssensor *) p, 10);
		    break;

		case EDS_lichtzone_BLOCK_ID:
		    lichtzone_timer_handler((device_data_lichtzone *) p,
					    10);
		    break;

		case EDS_multitaster_BLOCK_ID:
		    multitaster_timer_handler((device_data_multitaster *)
					      p, 10);
		    break;

		case EDS_ports_BLOCK_ID:
		    ports_timer_handler((device_data_ports *) p, 10);
		    break;

		case EDS_poti_BLOCK_ID:
		    poti_timer_handler((device_data_poti *) p, 10);
		    break;

		case EDS_powerportAutomat_BLOCK_ID:
		    powerportAutomat_timer_handler((device_data_powerportAutomat *) p, 10);
		    break;

		case EDS_powerport_BLOCK_ID:
		    powerport_timer_handler((device_data_powerport *) p,
					    10);
		    break;

		case EDS_reedkontakt_BLOCK_ID:
		    reedkontakt_timer_handler((device_data_reedkontakt *)
					      p, 10);
		    break;

		case EDS_rolladenAutomat_BLOCK_ID:
		    rolladenAutomat_timer_handler((device_data_rolladenAutomat *) p, 10);
		    break;

		case EDS_rolladen_BLOCK_ID:
		    rolladen_timer_handler((device_data_rolladen *) p, 10);
		    break;

		case EDS_rolladenSchlitzpos_BLOCK_ID:
		    rolladenSchlitzpos_timer_handler((device_data_rolladenSchlitzpos *) p, 10);
		    break;

		case EDS_schalter_BLOCK_ID:
		    schalter_timer_handler((device_data_schalter *) p, 10);
		    break;

		case EDS_taster_BLOCK_ID:
		    taster_timer_handler((device_data_taster *) p, 10);
		    break;

		case EDS_tempsensor_BLOCK_ID:
		    tempsensor_timer_handler((device_data_tempsensor *) p,
					     10);
		    break;

		case EDS_zeitschaltuhr_BLOCK_ID:
		    zeitschaltuhr_timer_handler((device_data_zeitschaltuhr
						 *) p, 10);
		    break;

		case EDS_zeitzone_BLOCK_ID:
		    zeitzone_timer_handler((device_data_zeitzone *) p, 10);
		    break;

		case EDS_zentralheizungspumpe_BLOCK_ID:
		    zentralheizungspumpe_timer_handler((device_data_zentralheizungspumpe *) p, 10);
		    break;

		}
	    }
	}
    }
    // 1sec Takt
    if (timer_event_1) {
	// Event Flag zuruecksetzen
	timer_event_1 = 0;

	for (i = 0; i < MAX_PDEVICE_DATA; i++) {
	    uint8_t        *p = pdevice_data[i];
	    if (p) {
		switch (*p) {

		case EDS_analogComparator_BLOCK_ID:
		    analogComparator_timer_handler((device_data_analogComparator *) p, 1);
		    break;

		case EDS_heizung_BLOCK_ID:
		    heizung_timer_handler((device_data_heizung *) p, 1);
		    break;

		case EDS_helligkeitssensor_BLOCK_ID:
		    helligkeitssensor_timer_handler((device_data_helligkeitssensor *) p, 1);
		    break;

		case EDS_lichtzone_BLOCK_ID:
		    lichtzone_timer_handler((device_data_lichtzone *) p,
					    1);
		    break;

		case EDS_multitaster_BLOCK_ID:
		    multitaster_timer_handler((device_data_multitaster *)
					      p, 1);
		    break;

		case EDS_ports_BLOCK_ID:
		    ports_timer_handler((device_data_ports *) p, 1);
		    break;

		case EDS_poti_BLOCK_ID:
		    poti_timer_handler((device_data_poti *) p, 1);
		    break;

		case EDS_powerportAutomat_BLOCK_ID:
		    powerportAutomat_timer_handler((device_data_powerportAutomat *) p, 1);
		    break;

		case EDS_powerport_BLOCK_ID:
		    powerport_timer_handler((device_data_powerport *) p,
					    1);
		    break;

		case EDS_reedkontakt_BLOCK_ID:
		    reedkontakt_timer_handler((device_data_reedkontakt *)
					      p, 1);
		    break;

		case EDS_rolladenAutomat_BLOCK_ID:
		    rolladenAutomat_timer_handler((device_data_rolladenAutomat *) p, 1);
		    break;

		case EDS_rolladen_BLOCK_ID:
		    rolladen_timer_handler((device_data_rolladen *) p, 1);
		    break;

		case EDS_rolladenSchlitzpos_BLOCK_ID:
		    rolladenSchlitzpos_timer_handler((device_data_rolladenSchlitzpos *) p, 1);
		    break;

		case EDS_schalter_BLOCK_ID:
		    schalter_timer_handler((device_data_schalter *) p, 1);
		    break;

		case EDS_taster_BLOCK_ID:
		    taster_timer_handler((device_data_taster *) p, 1);
		    break;

		case EDS_tempsensor_BLOCK_ID:
		    tempsensor_timer_handler((device_data_tempsensor *) p,
					     1);
		    break;

		case EDS_zeitschaltuhr_BLOCK_ID:
		    zeitschaltuhr_timer_handler((device_data_zeitschaltuhr
						 *) p, 1);
		    break;

		case EDS_zeitzone_BLOCK_ID:
		    zeitzone_timer_handler((device_data_zeitzone *) p, 1);
		    break;

		case EDS_zentralheizungspumpe_BLOCK_ID:
		    zentralheizungspumpe_timer_handler((device_data_zentralheizungspumpe *) p, 1);
		    break;

		case EDS_timeservice_BLOCK_ID:
		    timeservice_timer_handler((device_data_timeservice *)
					      p, 1);
		    break;
		case EDS_dcf77_receiver_BLOCK_ID:
		    dcf77_receiver_timer_handler((device_data_dcf77_receiver *) p, 1);
		    break;
		}
	    }
	}

	/*
	 * vermutlich nicht ganz korrekt und per "EDS config RAM overflow" 
	 * schon zum Teil abgedeckt: unusedSRAM = get_mem_unused(); if(
	 * unusedSRAM < MAX_MEM_CRITICAL_SIZE ) {
	 * canix_syslog_P(SYSLOG_PRIO_CRITICAL, PSTR("mem_free: %d
	 * bytes"), unusedSRAM); }
	 */
    }
}
