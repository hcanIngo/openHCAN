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

#ifndef HCAN_RTS_H
#define HCAN_RTS_H

#ifndef CANIX_RTS
#error "CANIX_RTS muss definiert werden"
#endif

#include <canix/canix.h>

/**
 * @file	hcan_hms.h
 *
 * @author  Martin Haller
 * @date    13.3.2006
 *
 * @brief	Implementierung des RTS (Realtime (Clock) Service)
 *
 */

/**
 * Stellt den HCAN RTS Frame Callback dar. Es werden
 * Zeit-Informationen verarbeitet:
 *
 * - HCAN_RTS_TIME_INFO
 * - HCAN_RTS_DATE_INFO
 *
 * Diese werden vom Time-Master gesendet; jeder RTS Empfaenger 
 * synchronisiert sich dazu, sofern die Abweichung einen bestimmten
 * Betrag ueberschreitet. 
 *
 * \a Hinweis: Das Datum wird \a immer uebernommen.
 *
 * Es wird aber \a nicht die Time-Master Funktion uebernommen, wenn eine
 * Weile keine Time-Meldungen mehr kommen. Dieses Feature ist z.Z. nur
 * in der Controller-1612 Firmware implementiert.
 *
 * Auch werden Zeit-Anfrage beantwortet:
 * - HCAN_RTS_TIME_REQUEST
 * - HCAN_RTS_DATE_REQUEST
 *
 * Ueber diese Befehle koennen HCAN Clients die aktuelle Zeit 
 * / das Datum auslesen.
 *
 */
void canix_SFP_RTS_handler(const canix_frame *frame);

#endif
