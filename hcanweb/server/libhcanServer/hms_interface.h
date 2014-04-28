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

#include <libhcan++/traceable_error.h>
#include <libhcan++/transport_connection.h>
#include <libhcan++/board_connection.h>
#include <libhcan++/board_driver.h>
#include <libhcan++/driver_factory.h>
#include <libhcan++/dynaddress.h>
#include <libhcan++/colors.h>
#include <hcan_multicast.h>

#include <netinet/in.h>
#include <arpa/inet.h>

uint16_t queryState_specific (uint8_t query, uint8_t id);
void executeCmd_specific (uint8_t id, uint8_t cmd, uint16_t x, uint16_t y);
float getSollTemp_specific (uint8_t id);

#endif
