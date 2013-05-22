#ifndef CANIX_RTC_H
#define CANIX_RTC_H

#include <avr/io.h>

/**
 * Diese Struktur haelt alle Felder, die zur RTC gehoeren.
 */

typedef struct 
{
	/** Uptime Sekunden seit Boot */
	uint32_t uptime_sec;
	/** 100tel Sekunden seit letztder uptime_sec Inkrementierung  */
	uint8_t uptime_sec_100th;

	/** Wochentag; 1 = Montag, ... 7 = Sonntag */
	uint8_t day_of_week;
	/** Stunde: 0..23 */
	uint8_t hour;
	/** Minute: 0..59 */
	uint8_t minute;
	/** Sekunde: 0..59 */
	uint8_t second;

	/** vergangene Sekunden seit dem letzten empfangenen Zeitframe */
	uint8_t last_date_update;

	/** Tag des Monats: [1..31] */
	uint8_t day_of_month;
	/** Monat: [1..12] */
	uint8_t month_of_year;
	/** Jahr: Das Yahr wird errechnet durch: 2000 + year;
	 * 1 = 2001, 2 = 2002 
	 * */
	uint8_t year;

} canix_rtc_clock_t;

extern volatile canix_rtc_clock_t canix_rtc_clock;

/**
 * Liefert zurueck, ob das Datum valide ist. Das Datum ist valide,
 * wenn in den letzten 254 Minuten ein Datumsframe empfangen wurde.
 *
 * @return 0 wenn invalide, 1 wenn valide
 */
uint8_t canix_is_date_valid(void);

#endif
