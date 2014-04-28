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
#include "heizung_info.h"
#include <iostream>
#include <hcan_multicast.h>
#include <hcan-def.h>
#include <sstream>

heizung_info::heizung_info(hcan::transport_connection &tcon, uint16_t src) :
	m_tcon(tcon),
	m_src(src)
{
}

heizung_info::~heizung_info()
{
}

float heizung_info::get_ist_temp(uint8_t id)
{
	uint8_t dummy,hi,lo;
	m_tcon.send_HEIZUNG_TIST_REQUEST(m_src, HCAN_MULTICAST_CONTROL,id);
	m_tcon.recv_HEIZUNG_TIST_REPLAY(0,m_src,&dummy,&hi,&lo);
	return ((hi << 8) | lo) / 16.0;
}

heizung_info_t heizung_info::get_details(uint8_t id)
{
	heizung_info_t info;
	m_tcon.send_HEIZUNG_DETAILS_REQUEST(m_src, HCAN_MULTICAST_CONTROL,id);

	hcan::frame f = m_tcon.recv_frame(m_src);

	switch (f.data(1))
	{
		case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS :
			info.mode = 0;
			info.soll_temp = 0; // IL
			break;
		case HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS :
			info.mode = 1;
			info.soll_temp = 0; // IL
/*			info.stellgrad = f.data(3);
			info.dauer = (f.data(4)<<8 | f.data(5));
			info.dauer_min = info.dauer / 60;
			info.dauer_sec = info.dauer % 60;*/
			break;
		case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS :
			info.mode = 2;
			info.soll_temp = ((f.data(3)<<8)|f.data(4))/16.0;
/*			info.dauer = (f.data(5)<<8) | f.data(6);
			info.dauer_min = info.dauer / 60;
			info.dauer_sec = info.dauer % 60;*/
			break;
		case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS :
			info.mode = 3;
			info.soll_temp = (f.data(3)<<8|f.data(4))/16.0;
//			info.zz_id = f.data(5);
			break;
		default:
			throw traceable_error("unknown heizung mode");
	}

//	info.ist_temp = get_ist_temp(id);

/*	stringstream ss;
	ss << setw(3) << info.dauer_min << ":" << setw(2) << setfill('0') 
		<< info.dauer_sec << "min";
	info.dauer_str = ss.str();*/

	return info;
}
