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
#ifndef SHARED_CLIENT_INTERFACE_H
#define SHARED_CLIENT_INTERFACE_H

#include <stdint.h> // fuer uint8_t...

#define IDS_LENGTH 200

struct query {
	char JSONbuf[200];
	char cmd[10];
	uint8_t selectedId;
	uint8_t ids[IDS_LENGTH]; // wird im Server mit 255 abgeschlossen (Ende-Kennzeichen)

	uint8_t queryStates; // true=1, false=0

	uint8_t newPage; // true=1, false=0
	char page[12];
};
typedef struct query t_query;

void getAjaxInput (char *str, t_query * query);

#endif
