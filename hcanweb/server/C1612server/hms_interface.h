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
 *  (c) 2013 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 *  based on hcan:
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 */
#ifndef HMS_INTERFACE_H
#define HMS_INTERFACE_H

#include "stdint.h"
#include <canix/canix.h>

#include <shared_client_interface.h> // fuer t_query

struct selectedDevice {
	uint8_t deviceQuery; // z.B. HCAN_HES_POWER_GROUP_STATE_REPLAY
	uint8_t id; // gruppe
	uint16_t state;
	uint8_t updated;
};

struct global {
	t_query query;
	struct selectedDevice response;
};
typedef struct global t_global;

extern t_global global;

uint16_t queryState_specific (uint8_t query, uint8_t id);
void executeCmd_specific (uint8_t id, uint8_t cmd, uint16_t x, uint16_t y);

void hauselektrik_cmd_query (void);
void hauselektrik_callback (const canix_frame *frame);

#endif
