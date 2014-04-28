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
#ifndef HEIZUNG_INFO_H
#define HEIZUNG_INFO_H

#include <libhcan++/transport_connection.h>

#define HEIZUNG_MODE_OFF 0
#define HEIZUNG_MODE_MANUELL 1
#define HEIZUNG_MODE_THERMOSTAT 2
#define HEIZUNG_MODE_AUTO 3

typedef struct
{
	int mode; // 0 = aus, 1 = manuell, 2 = thermostat, 3 = auto
	int dauer; // in minuten
	int dauer_min, dauer_sec;
	std::string dauer_str;
	float soll_temp, ist_temp;
	int stellgrad;
	int zz_id; // Zeitzone ID
} heizung_info_t;

class heizung_info
{
	private:
		hcan::transport_connection &m_tcon;
		uint16_t m_src;

	public:
		heizung_info(hcan::transport_connection &tcon, uint16_t src);
		virtual ~heizung_info();
		
		heizung_info_t get_details(uint8_t id);
		float get_ist_temp(uint8_t id);
};

#endif
