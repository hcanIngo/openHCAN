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
#ifndef GESAMT_HEIZ_PAGE_H
#define GESAMT_HEIZ_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

#define MAX_HEIZ_IDS 12

// aus heizung.h:   HEIZUNG_MODE_OFF             0
// nicht verwendet: HEIZUNG_MODE_MANUAL          1
#define HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ      1
#define HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ 2
#define HEIZUNG_MODE_THERMOSTAT_X                3 // _X, da _KURZ eingefuegt ist
#define HEIZUNG_MODE_AUTOMATIK_X                 4


void gesamt_heiz_page_handle_key_down_event(eds_gesamt_heiz_page_block_t *p,
		uint8_t key);
void gesamt_heiz_page_print_page(eds_gesamt_heiz_page_block_t *p);

#endif
