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
#ifndef HELLIGKEITSSENSOR_H
#define HELLIGKEITSSENSOR_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>
#include "../../controllerboard/floatingAverage.h"

typedef struct
{
	uint8_t type;
	eds_helligkeitssensor_block_t config;
	uint16_t secsUntilNextSend; // zyklisch Helligkeitsnachrichten versenden
	tFloatAvgFilter filterBrightness; // Speicher des gleitenden Mittelwerts zur Glättung
} device_data_helligkeitssensor;

void helligkeitssensor_init(device_data_helligkeitssensor *p, eds_block_p it);
extern void helligkeitssensor_timer_handler(device_data_helligkeitssensor *p, uint8_t zyklus);
void helligkeitssensor_can_callback(device_data_helligkeitssensor *p, const canix_frame *frame);

#endif
