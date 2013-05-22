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
#ifndef SCHALTER_H
#define SCHALTER_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define FALLING	0 //Eingangs-Port 1->0
#define RISING 	1 //Eingangs-Port 0->1

#define FEATURE_SCHALTER_INVERTIEREN  0 // Bit0 (the LSB)
#define FEATURE_SCHALTER_PULLUP_AUS   1 // Bit1

typedef struct
{
	uint8_t type;
	eds_schalter_block_t config;
	uint8_t newState; // fuer Entprellung
	uint8_t lastEdge; // letzte erkannte Flanke (steigend oder fallend)
} device_data_schalter;

void schalter_can_callback(device_data_schalter *p, const canix_frame *frame);
void schalter_timer_handler(device_data_schalter *p);

#endif

