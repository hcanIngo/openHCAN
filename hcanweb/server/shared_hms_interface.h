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
#ifndef SHARED_HMS_INTERFACE_H
#define SHARED_HMS_INTERFACE_H

#include <shared_client_interface.h> // fuer t_query
#include <stdint.h> // fuer uint8_t...

uint8_t is (const char * a, const char * b);
uint8_t isnt (const char * a, const char * b);
void processCmd (t_query * query);
void processQuery (t_query * query);
void executeCmd (const char * page, uint8_t id, const char * cmd);
void executeCmds (const char * page, uint8_t * ids, const char * cmd);
void queryStates (char * JSONbuf, const char * page, uint8_t * ids);

#endif
