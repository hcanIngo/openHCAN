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

#include "usart.h" // fuer DEBUG

void getAjaxInput (char *str, t_query * query)
{
	uint8_t ok = 1; // notwendige Interface-Parameter sind vorhanden

	char strBuf[IDS_LENGTH]; // sollte der Laenge von query->Ids entsprechen

	if (find_key_val (str, strBuf, 20, "cmd"))
		strcpy (query->cmd, strBuf);
	else ok = 0;

	if (find_key_val (str, strBuf, 20, "d")) // device (= hcanweb-Page)
		strcpy (query->page, strBuf);
	else ok = 0;

	if (find_key_val (str, strBuf, 20, "setid")) // selectedId
		query->selectedId = atoi (strBuf);
	else ok = 0;

	if (find_key_val (str, strBuf, IDS_LENGTH, "qid"))
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

		/* DEBUG("\r\nqid:");
		for (i=0; i < IDS_LENGTH; i++)
		{
			DEBUG("%i,", query->ids[i]);
			if (255 == query->ids[i]) break;
		}
		DEBUG("\r\n"); */

		if (query->ids[0] != 255)
			query->queryStates = 1;
		else query->queryStates = 0; // qid = "" -> keine Zustandsabfrage
	}
	else ok = 0;

	if (0 == ok)
	{
		// Der Client hat das Interface nicht eingehalten:
		strcpy (query->cmd, ""); // kein cmd ausfuehren
		query->queryStates = 0; // keine states abfragen
		query->ids[0] = 255; // Ende-Kennzeichen
	}
}
