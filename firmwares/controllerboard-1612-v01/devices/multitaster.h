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
#ifndef MUTLITASTER_H
#define MUTLITASTER_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define MAX_MULTITASTER_GROUPS 10

#define FEATURE_MULTITASTER_ENTPRELL_1S  0 //Bit0 (the LSB)
#define FEATURE_MULTITASTER_ROLLADEN_AUF 1 //Bit1
#define FEATURE_MULTITASTER_ROLLADEN_ZU  2 //Bit2


typedef struct
{
	uint8_t type;
	eds_multitaster_block_t config;
	uint8_t pressed; // fuer die Taster Entprellung	
	uint8_t lastPotiPos; // Ist der Potiabschnitt, in dem das Poti zuletzt stand (letzter Gruppenindex)
	uint16_t oneScale; // Groesse eines Potiabschnitts (durch die Anzahl konfigurierter Gruppen festgelegt)
} device_data_multitaster;

void multitaster_init(device_data_multitaster *p, eds_block_p it);
inline void multitaster_timer_handler(device_data_multitaster *p, uint8_t zyklus);
void multitaster_can_callback(device_data_multitaster *p, const canix_frame *frame);

#endif
