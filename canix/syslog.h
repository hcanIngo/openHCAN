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

#ifndef HCAN_SYSLOG_H
#define HCAN_SYSLOG_H

#ifndef CANIX_SYSLOG
#error "CANIX_SYSLOG muss definiert werden"
#endif

#define SYSLOG_PRIO_CRITICAL     1
#define SYSLOG_PRIO_ERROR        2
#define SYSLOG_PRIO_WARNING      3
#define SYSLOG_PRIO_DEBUG        4

#include <avr/pgmspace.h>

/**
 * Loggt eine Meldung ueber das HCAN-Syslog Protokoll.
 * @param prio die Syslog Prioritaet (SYSLOG_PRIO_CRITICAL 
 * | SYSLOG_PRIO_ERROR | SYSLOG_PRIO_WARNING | SYSLOG_PRIO_DEBUG)
 * @param fmt der Format-String (muss mit PSTR(..) uebergeben werden !!)
 */
void canix_syslog_P(uint8_t prio, const char *fmt, ...);

/**
 * Setzt den Syslog Level; alle Meldungen, deren Prioritaet die kleiner 
 * oder gleich (zahlenmaesig) diesem Wert sind, werden geloggt, alle anderen
 * werden verworfen. 
 *
 * Beispiel: 
 *
 * canix_set_syslog_level (SYSLOG_PRIO_ERROR);
 *
 * // wird geloggt, da SYSLOG_PRIO_CRITICAL <= SYSLOG_PRIO_ERROR
 * canix_syslog_P(SYSLOG_PRIO_CRITICAL, PSTR("Test1"));
 * 
 * // wird nicht geloggt, da SYSLOG_PRIO_DEBUG > SYSLOG_PRIO_ERROR
 * canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("Test2"));
 *
 */
void canix_set_syslog_level(uint8_t level);

#endif
