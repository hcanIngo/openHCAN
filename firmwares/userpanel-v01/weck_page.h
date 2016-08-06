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
#ifndef WECK_PAGE_H
#define WECK_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

// Fuer die Powerport-Feature-Konstanten
#include "../controllerboard-1612-v01/devices/powerport.h"
/* POWERPORT_FEATURE_WECK_VERZOEGERT_EIN	4 //Bit4
 * POWERPORT_FEATURE_WECK_AUTO_AUS			5 //Bit5 */

void weck_page_handle_key_down_event(eds_weck_page_block_t *p, uint8_t key);
void weck_page_print_page(eds_weck_page_block_t *p);
void weck_page_can_callback(eds_weck_page_block_t *p, const canix_frame *frame);

#endif
