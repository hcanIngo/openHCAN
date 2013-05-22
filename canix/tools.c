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
 */

#include <avr/io.h>
#include <canix/rtc.h>

void delay(uint16_t ms)
{
	uint16_t zaehler;

	while (ms)
	{
		zaehler = F_CPU / 5000;
		while (zaehler)
		{
			asm volatile ("nop");
			zaehler--;
		}
		ms--;
	}
}

void _delayFourCycles(unsigned int count)
{
	if ( count == 0 )    
		asm volatile( "rjmp 1f\n 1:" );
	else
		asm volatile("1: sbiw %0,1" "\n\t" "brne 1b" : "=w"
				(count) : "0" (count));
}

uint8_t time_matches(uint8_t from_hour, uint8_t from_minute,
		uint8_t to_hour, uint8_t to_minute, uint8_t day_pattern)
{
	uint16_t t1,t2,tnow;

	t1 = from_hour * 60 + from_minute;
	t2 = to_hour * 60 + to_minute;
	tnow = canix_rtc_clock.hour * 60 + canix_rtc_clock.minute;

	if (t2 < t1)
	{
		// Falls t1 groesser t2 ist, dann liegt beispielsweise t2 nach
		// Mitternacht. Damit der Vergleich dann trotzdem funktioniert,
		// wird zu t2 und tnow 1 Tag hinzuaddiert:

		t2 += 1440;

		if (tnow < t1)
			tnow += 1440;
	}

	// das day_pattern und das Zeitinterval testen:
	if ((day_pattern & (1 << canix_rtc_clock.day_of_week)) &&
			(t1 <= tnow) && (tnow <= t2))
		return 1;

	return 0;
}

