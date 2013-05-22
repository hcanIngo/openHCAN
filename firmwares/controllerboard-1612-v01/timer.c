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
#include "devices.h"
#include "shiftio.h"

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
	uint8_t i;
	uint16_t unusedSRAM; //in bytes

	// 10msec Takt:
	if (timer_event_100th)
	{
		// Event Flag zuruecksetzen
		timer_event_100th = 0;

		for (i = 0; i < MAX_PDEVICE_DATA; i++)
		{
			uint8_t *p = pdevice_data[i];
			if (p)
			{
				switch (*p)
				{
					case EDS_dcf77_receiver_BLOCK_ID:
						dcf77_receiver_timer_handler ( (device_data_dcf77_receiver*) p); 
						break;
					case EDS_multitaster_BLOCK_ID:
						multitaster_timer_handler ( (device_data_multitaster*) p);
						break;
					case EDS_taster_BLOCK_ID: 
						taster_timer_handler ( (device_data_taster*) p); 
						break;
				}
			}
		}
	}

	// 100msec Takt
	if (timer_event_10th)
	{
		// Event Flag zuruecksetzen
		timer_event_10th = 0;

		for (i = 0; i < MAX_PDEVICE_DATA; i++)
		{
			uint8_t *p = pdevice_data[i];
			if (p)
			{
				switch (*p)
				{
					case EDS_multitaster_BLOCK_ID:
						multitaster_timer_handler ( (device_data_multitaster*) p);
						break;
					case EDS_poti_BLOCK_ID:
						poti_timer_handler ( (device_data_poti*) p);
						break;
					case EDS_rolladen_BLOCK_ID: 
						rolladen_timer_handler ( (device_data_rolladen*) p); 
						break;
					case EDS_rolladenSchlitzpos_BLOCK_ID:
						rolladenSchlitzpos_timer_handler ( (device_data_rolladenSchlitzpos*) p);
						break;
					case EDS_schalter_BLOCK_ID:
						schalter_timer_handler ( (device_data_schalter*) p);
						break;
				}
			}
		}
	}

	// 1sec Takt
	if (timer_event_1)
	{
		// Event Flag zuruecksetzen
		timer_event_1 = 0;

		for (i = 0; i < MAX_PDEVICE_DATA; i++)
		{
			uint8_t *p = pdevice_data[i];
			if (p)
			{
				switch (*p)
				{
					case EDS_dunstabzugport_BLOCK_ID: 
						dunstabzugport_timer_handler ( (device_data_dunstabzugport*) p); 
						break;
					case EDS_helligkeitssensor_BLOCK_ID:
						helligkeitssensor_timer_handler ( (device_data_helligkeitssensor*) p);
						break;
					case EDS_heizung_BLOCK_ID: 
						heizung_timer_handler ( (device_data_heizung*) p); 
						break;
					case EDS_lichtzone_BLOCK_ID: 
						lichtzone_timer_handler ( (device_data_lichtzone*) p); 
						break;
					case EDS_reedkontakt_BLOCK_ID: 
						reedkontakt_timer_handler ( (device_data_reedkontakt*) p); 
						break;
					case EDS_rolladenAutomat_BLOCK_ID:
						rolladenAutomat_timer_handler ( (device_data_rolladenAutomat*) p);
						break;
					case EDS_tempsensor_BLOCK_ID: 
						tempsensor_timer_handler ( (device_data_tempsensor*) p); 
						break;
					case EDS_timeservice_BLOCK_ID: 
						timeservice_timer_handler ( (device_data_timeservice*) p); 
						break;
					case EDS_zeitschaltuhr_BLOCK_ID: 
						zeitschaltuhr_timer_handler ( (device_data_zeitschaltuhr*) p); 
						break;
					case EDS_powerport_BLOCK_ID :
						powerport_timer_handler( (device_data_powerport*) p);
						break;
				}
			}
		}

		unusedSRAM = get_mem_unused();
		if( unusedSRAM < MAX_MEM_CRITICAL_SIZE )
		{
			canix_syslog_P(SYSLOG_PRIO_CRITICAL,
					PSTR("mem_free: %d bytes"), unusedSRAM);
		}
	}

	shiftOut_timer_handler(); //die Ausgaben nachtriggern (Störungsunterdrückung)
}

