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

#ifndef TOOLS_H
#define TOOLS_H

/**
 * Wartet die angegebene Anzahl von msec. Achtung: Der WDT koennte den
 * Controller resetten
 */
void delay(uint16_t ms);

/**
 * Wartet die angegebene Anzahl von usec (!). Achtung: Falls die F_CPU
 * einen anderen Wert bekomt, kann es noetig sein, diese Formel (wegen
 * Rundungsfehler) anzupassen.
 */
#define udelay(us)  _delayFourCycles( ( ( 1*(F_CPU/4000) )*us)/1000 )

/**
 * nur zu internen Verwendung: Wartet 4 Takt-Zyklen
 */
void _delayFourCycles(unsigned int count);

/**
 * Prueft, ob die aktuelle Uhrzeit im gegebenen Interval (Zeitzone)
 * liegt. Liefert 1 bzw. 0.
 */
uint8_t time_matches(uint8_t from_hour, uint8_t from_minute,
		uint8_t to_hour, uint8_t to_minute, uint8_t day_pattern);

#endif
