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
#include <shared_client_interface.h>

#include <websrv_help_functions.h>

#include <stdint.h> // fuer uint8_t...
#include <string.h> // fuer strcpy
#include <stdlib.h> // fuer sprintf

void getAjaxInput (char *str, t_query * query)
{
	char strBuf[IDS_LENGTH]; // sollte der Laenge von query->Ids entsprechen

	if (find_key_val (str,strBuf,20,"cmd"))
	{
		strcpy (query->cmd, strBuf);
	}
	if (find_key_val (str,strBuf,20,"newPage"))
	{
		if ('f' == strBuf[0]) query->newPage = 0;
		else query->newPage = 1;
	}
	if (find_key_val (str,strBuf,20,"page"))
	{
		strcpy (query->page, strBuf);
	}
	if (find_key_val (str,strBuf,20,"selectedId"))
	{
		query->selectedId = atoi (strBuf);
	}
	if (find_key_val (str,strBuf,IDS_LENGTH,"ids"))
	{
		urldecode (strBuf); // konvertiert von '%2C' zu ','
		uint8_t i = 0;
		char delimiter[] = ",";
		char * pId = strtok (strBuf, delimiter);
		while (pId != NULL) // ueber alle Ids
		{
			query->ids[i] = atoi (pId);
			pId = strtok (NULL, delimiter); // den naechsten Token holen
			i++;
		}
		query->ids[i] = 255; // Ende-Kennzeichen
	}
	if (find_key_val (str,strBuf,20,"queryStates"))
	{
		if ('f' == strBuf[0]) query->queryStates = 0;
		else query->queryStates = 1;
	}
}
