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
#ifndef POTI_H
#define POTI_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define POTI_VALUE_TOLERANCE 30 //min. Toleranz, damit ein der Potiwert als veraendert gilt

/* Anzahl Perioden fuer die der Potiwert konstant sein muss, 
 * damit eine neue Potinachricht versendet wird. Mindestprojektierung = 1 */
#define POTI_TELEGRAM_COUNTDOWN_INIT 4

typedef struct
{
	uint8_t type;
	eds_poti_block_t config;
	int16_t lastPotiValue; //kann durch MIN_POTI_VALUE_DIFF auch negativ werden
	uint8_t potiTelegramCountdown;
} device_data_poti;

void poti_init(device_data_poti *p, eds_block_p it);
void poti_timer_handler(device_data_poti *p);
int16_t getADCvalue(void);

#endif
