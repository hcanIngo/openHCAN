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
#ifndef ROLLADENSCHLITZPOS_H
#define ROLLADENSCHLITZPOS_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define SCHLITZ_POS					17 //17% (ist die Postion, in der die Rolllaeden auf Schlitz stehen)
#define SCHLITZ_JOB_ABGEMELDET		255 //wenn ungleich 255 wird der job nach x * 100ms ausgefuehrt
#define SCHLITZ_JOB_TIMER_LOAD		20 //20 * 100ms = 2 s bis der job ausgefuehrt wird


typedef struct
{
	uint8_t type;
	eds_rolladenSchlitzpos_block_t config;
	uint8_t rolladenSchlitzJob;
	uint8_t rolladenIstposition;
	uint8_t state1stReedkontakt;
} device_data_rolladenSchlitzpos;

void rolladenSchlitzpos_init(device_data_rolladenSchlitzpos *p, eds_block_p it);
extern void rolladenSchlitzpos_timer_handler(device_data_rolladenSchlitzpos *p, uint8_t zyklus);
void rolladenSchlitzpos_can_callback(device_data_rolladenSchlitzpos *p,
		const canix_frame *frame);

#endif

