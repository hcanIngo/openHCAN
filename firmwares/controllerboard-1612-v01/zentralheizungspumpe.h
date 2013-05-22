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
#ifndef ZENTRALHEIZUNGSPUMPE_H
#define ZENTRALHEIZUNGSPUMPE_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define EINSCHALT_SCHWELLE 10 // Wertebereich 0..100
#define AUSSCHALT_SCHWELLE EINSCHALT_SCHWELLE - 5 //Hysterese von 5

#define ZENTRALHEIZUNGSPUMPE_UPDATE_INTERVAL 50
#define MAX_ANZ_HEIZUNGEN 15

typedef struct
{
	uint8_t type;
	eds_zentralheizungspumpe_block_t config;
	struct
	{
		uint8_t id; //ID der Heizung
		uint8_t waermebedarf;
	} heizung[MAX_ANZ_HEIZUNGEN];
	
	uint8_t gesamtWaermebedarf;
	
	/** hier wird der Zustand der Power-Gruppe gespeichert */
	uint8_t state;

	/** 
	 * In regelmaessigen Abstaenden wird der Zustand wiederholt an die
	 * Power Gruppe gesendet, so dass verlorene Frames oder rebootete 
	 * Geraete keine Probleme machen.
	 */
	uint16_t update_counter;
} device_data_zentralheizungspumpe;

void zentralheizungspumpe_init(device_data_zentralheizungspumpe *p, eds_block_p it);
void zentralheizungspumpe_can_callback(device_data_zentralheizungspumpe *p, const canix_frame *frame);

#endif
