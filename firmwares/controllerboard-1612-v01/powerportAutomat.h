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
#ifndef POWERPORTAUTOMAT_H
#define POWERPORTAUTOMAT_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define N_ZEITZONEN 2

typedef struct
{
	uint8_t type;
	eds_powerportAutomat_block_t config;

	uint16_t brightness; // hier ist der aktuelle Helligkeitswert gespeichert
	uint8_t msgIstVersendet; // ROLLADEN_POSITION_SET ist einmalig versendet wurden
	uint8_t automatikEin; // Automatik aktiviert oder nicht
} device_data_powerportAutomat;

void powerportAutomat_init(device_data_powerportAutomat *p, eds_block_p it);
void powerportAutomat_can_callback(device_data_powerportAutomat *p, const canix_frame *frame);

#endif
