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
 *  (c) 2010 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 */
#ifndef ROLLADENAUTOMAT_H
#define ROLLADENAUTOMAT_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

// Bei Helligkeitswert Ueber- oder Unterschreitung wird ein Timer aufgezogen.
// Sobald dieser Ablaeuft wird sendMessage zur Rolladensteuerung aufgerufen.
// Erfolgen bei Timerlauf nach einer Ueberschreitung eine Unterschreitung bzw.
// nach einer Unterschreitung eine Ueberschreitung, wird der Timer zurueckgesetzt.
//
// SchwellwertErkannt ist:
// - HELLIGKEITS_UNTERSCHREITUNG_ERKANNT
// - HELLIGKEITS_UEBERSCHREITUNG_ERKANNT
// - HELLIGKEIT_IM_TOLERANZBEREICH
//
// SchwellwertErkanntSecsCnt

#define N_ZEITZONEN 2

#define HELLIGKEIT_IM_TOLERANZBEREICH 0
#define HELLIGKEITS_UNTERSCHREITUNG_ERKANNT 1
#define HELLIGKEITS_UEBERSCHREITUNG_ERKANNT 2

typedef struct
{
	uint8_t type;
	eds_rolladenAutomat_block_t config;

	uint16_t brightness; // hier ist der aktuelle Helligkeitswert gespeichert
	uint8_t msgIstVersendet; // ROLLADEN_POSITION_SET ist einmalig versendet wurden
	uint8_t automatikEin; // Automatik aktiviert oder nicht

	uint16_t SchwellwertErkanntSecsCnt; // 0 wenn dieser nicht laeuft
	uint8_t SchwellwertErkannt; // HELLIGKEITS_UNTERSCHREITUNG_ERKANNT, HELLIGKEITS_UEBERSCHREITUNG_ERKANNT
} device_data_rolladenAutomat;

void rolladenAutomat_init(device_data_rolladenAutomat *p, eds_block_p it);
void rolladenAutomat_timer_handler(device_data_rolladenAutomat *p);
void rolladenAutomat_can_callback(device_data_rolladenAutomat *p, const canix_frame *frame);

#endif
