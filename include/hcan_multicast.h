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

#ifndef HCAN_MULTICAST_H
#define HCAN_MULTICAST_H

/**
 * Spezielle ping Multicast Adresse; wird nur noch aus Legacy-Gruenden
 * von den Bootloadern verwendet. CANIX verwendet die HCAN_MULTICAST_INFO
 * Adresse
 */

#define HCAN_MULTICAST_PING                         31

/**
 * An diese Multicast Adresse wird alles gesendet, was zur Steuerung
 * im Haus noetig ist. Die Adresse hat hoehere Prioritaet als die
 * Info-Muliticast Adresse.
 */
#define HCAN_MULTICAST_CONTROL                      35

/**
 * An diese Multicast Adresse wird alles gesendet, was _nicht_ zur 
 * Steuerung im Haus noetig ist. Die Adresse hat eine niedrigere Prioritaet
 * als die Control-Muliticast Adresse.
 */
#define HCAN_MULTICAST_INFO                         36

/**
 * An diese Multicast Adresse werden Syslog Meldungen gesendet. Da sie nur
 * zu Debugging Zwecken noetig sind, stammt diese Adresse aus dem hohen
 * Multicast Adressbereich, welcher eine niedrige Adress-Prioritaet hat
 */
#define HCAN_MULTICAST_SYSLOG                      970

#endif
