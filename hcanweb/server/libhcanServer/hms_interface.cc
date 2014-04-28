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
#include <hms_interface.h>
#include <heizung_info.h>

#include <netinet/in.h>
#include <arpa/inet.h>

in_addr_t hcand_ip = inet_addr("192.168.1.24"); // HI-Server
//in_addr_t hcand_ip = inet_addr("127.0.0.1");

uint16_t queryState_specific (uint8_t query, uint8_t id)
{
	hcan::dynaddress dynsrc(hcand_ip); // HCAN Zugriff vorbereiten
	dynsrc.allocate();
	uint16_t src = dynsrc();
	hcan::transport_connection tcon(hcand_ip);

	if (HCAN_HES_POWER_GROUP_STATE_QUERY == query)
	{
		uint8_t status;
		uint8_t timer;
		tcon.send_POWER_GROUP_STATE_QUERY(src,HCAN_MULTICAST_CONTROL,id);
		//sleep (10); // ms
		tcon.recv_POWER_GROUP_STATE_REPLAY(0, src, &id, &status, &timer);
		return (uint16_t)status;
	}
	else if (HCAN_HES_HEIZUNG_DETAILS_REQUEST == query)
	{
		heizung_info cpHeizung(tcon,src);
		heizung_info_t info = cpHeizung.get_details(id);
		return (uint16_t)info.soll_temp;
	}
	else if (HCAN_HES_ROLLADEN_POSITION_REQUEST == query)
	{
		uint8_t pos;
		tcon.send_ROLLADEN_POSITION_REQUEST(src,HCAN_MULTICAST_CONTROL,id);
		tcon.recv_ROLLADEN_POSITION_REPLAY(0, src, &id, &pos);
		return (uint16_t)pos;
	}
	else if (HCAN_HES_REEDKONTAKT_STATE_QUERY == query)
	{
		uint8_t status;
		tcon.send_REEDKONTAKT_STATE_QUERY(src,HCAN_MULTICAST_CONTROL,id);
		tcon.recv_REEDKONTAKT_STATE_REPLAY(0, src, &id, &status);
		return (uint16_t)status;
	}
	else if (HCAN_HES_1WIRE_TEMPERATURE_QUERY == query)
	{
		uint8_t temp_hi, temp_lo;
		tcon.send_1WIRE_TEMPERATURE_QUERY(src,HCAN_MULTICAST_CONTROL,id);
		tcon.recv_1WIRE_TEMPERATURE_REPLAY(0, src, &id, &temp_hi, &temp_lo);
		return (uint16_t) ((temp_hi << 8)| temp_lo) / 16;
	}

	return 0; // Fehler
}

void executeCmd_specific (uint8_t id, uint8_t cmd, uint16_t x, uint16_t y)
{
	hcan::dynaddress dynsrc(hcand_ip); // HCAN Zugriff vorbereiten
	dynsrc.allocate();
	uint16_t src = dynsrc();
	hcan::transport_connection tcon(hcand_ip);

	if (HCAN_HES_POWER_GROUP_ON == cmd)
		tcon.send_POWER_GROUP_ON(src,HCAN_MULTICAST_CONTROL,id);

	else if (HCAN_HES_POWER_GROUP_OFF == cmd)
		tcon.send_POWER_GROUP_OFF(src,HCAN_MULTICAST_CONTROL,id);

	else if (HCAN_HES_HEIZUNG_SET_MODE_OFF == cmd)
		tcon.send_HEIZUNG_SET_MODE_OFF(src,HCAN_MULTICAST_CONTROL,id);

	else if (HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK == cmd)
		tcon.send_HEIZUNG_SET_MODE_AUTOMATIK(src,HCAN_MULTICAST_CONTROL,id);

	else if (HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS == cmd)
	{
		uint16_t dauer = x;
		uint16_t temp = y;
		tcon.send_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS(src,HCAN_MULTICAST_CONTROL,id,temp>>8,temp,dauer>>8,dauer);
	}
	else if (HCAN_HES_ROLLADEN_POSITION_SET == cmd)
	{
		uint16_t pos = x;
		tcon.send_ROLLADEN_POSITION_SET(src,HCAN_MULTICAST_CONTROL,id,pos);
	}
	else
	{
		// "executeEinzelCmd-Fehler: page=" + page
	}
}

float getSollTemp_specific (uint8_t id)
{
	hcan::dynaddress dynsrc(hcand_ip); // HCAN Zugriff vorbereiten
	dynsrc.allocate();
	uint16_t src = dynsrc();
	hcan::transport_connection tcon(hcand_ip);

	heizung_info cpHeizung(tcon,src);
	heizung_info_t info = cpHeizung.get_details(id);
	return info.soll_temp;
}

