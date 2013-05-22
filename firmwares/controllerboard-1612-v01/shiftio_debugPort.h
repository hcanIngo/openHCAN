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
#ifndef SHIFTIO_DEBUG_PORT_H_
#define SHIFTIO_DEBUG_PORT_H_

#include <canix/canix.h> //for SYSLOG_PRIO_CRITICAL
#include <canix/syslog.h> //for canix_syslog_P

#define debugPort(portXY) canix_syslog_P(SYSLOG_PRIO_CRITICAL, PSTR("%x%x%x%x%x"),\
                             (portXY & 0x10)!=0, (portXY & 0x08)!=0, (portXY & 0x04)!=0,\
                             (portXY & 0x02)!=0, (portXY & 0x01)!=0); //Zustand von debugPort binaer (5 Bits) ausgeben

#endif /*DEBUG_PORT_H_*/

