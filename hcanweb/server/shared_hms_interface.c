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
#include <shared_hms_interface.h>

#include <hms_interface.h>
#include <shared_client_interface.h>
#include <stdlib.h> // fuer atoi
#include <stdio.h> // fuer sprintf
#include <hcan-def.h> // fuer HCAN_HES_POWER_GROUP_STATE_QUERY usw.

#include <string.h> // fuer strcmp

static save_strcpy (char * destination,  const char * source, size_t num_max)
{
	strncpy (destination, source, num_max); // Teilkopie (nur num_max Zeichen)
	destination[num_max] = '\0'; // Ende bzw. NULL-Zeichen manuell eingefuegt
}

uint8_t is (const char * a, const char * b)
{
	return strcmp (a, b) == 0;
}

uint8_t isnt (const char * a, const char * b)
{
	return strcmp (a, b) != 0;
}

void processCmd (t_query * query)
{
	if (isnt (query->cmd,"")) // neues Kommando?
	{
		if (query->selectedId == 255) // entspricht "alle gelisteten Devices"
		{
			executeCmds (query->page, query->ids, query->cmd);
		}
		else
		{
			executeCmd (query->page, query->selectedId, query->cmd);
		}
	}
}

void processQuery (t_query * query)
{
	save_strcpy (query->JSONbuf, "", 1); // initialisieren

/*	if (query->selectedId == 255 && isnt (query->cmd,""))
		return; // nach executeCmds liefert queryStates nur ein JSON-Element
*/
	if (query->queryStates)
		queryStates (query->JSONbuf, query->page, query->ids);
	else
		save_strcpy (query->JSONbuf, "noStateQuery", 13);
}

static void catJSONobj (char * JSONbuf, uint8_t id, const char * state)
{
	size_t num = 11;
	char JSONobj[num]; // z.B.: '"123":100'

	if (is (JSONbuf, "")) snprintf (JSONobj, num, "\"%d\":%s", id, state); // nur beim ersten Element kein Komma
	else snprintf (JSONobj, num, ",\"%d\":%s", id, state);
	strcat (JSONbuf, JSONobj);
}

void queryStates (char * JSONbuf, const char * page, uint8_t * ids)
{
	uint16_t state = 0;
	uint8_t i = 0;
	while (255 != ids[i]) // ueber alle ids
	{
		uint8_t id = ids[i];

		if (id == 0)
		{
			// id 0 ueberspringen (ungueltig id)
		}

		else if (is (page, "lampe") || is (page, "sonstige"))
		{
			state = queryState_specific (HCAN_HES_POWER_GROUP_STATE_QUERY, id);
			catJSONobj (JSONbuf, id, state != 0 ? "1" : "0"); // state: 1=ein, 0=aus
		}

		else if (is (page, "heizung"))
		{
			state = queryState_specific (HCAN_HES_HEIZUNG_DETAILS_REQUEST, id);
			char temp[8];
			sprintf (temp, "%d", state);
			catJSONobj (JSONbuf, id, temp); // temp: soll-Temp
		}

		else if (is (page, "rolladen"))
		{
			state = queryState_specific (HCAN_HES_ROLLADEN_POSITION_REQUEST, id);
			char pos[8];
			sprintf (pos, "%d", state);
			catJSONobj (JSONbuf, id, pos); // pos: ist-Pos
		}

		else if (is (page, "reedkontakt"))
		{
			state = queryState_specific (HCAN_HES_REEDKONTAKT_STATE_QUERY, id);
			catJSONobj (JSONbuf, id, state != 0 ? "1" : "0"); // state: 1=offen, 0=geschlossen
		}

		else if (is (page, "tempsensor"))
		{
			state = queryState_specific (HCAN_HES_1WIRE_TEMPERATURE_QUERY, id);
			char temp[8];
			sprintf (temp, "%d", state);
			catJSONobj (JSONbuf, id, temp); // temp: ist-Temperatur
		}

		else
		{
			save_strcpy (JSONbuf, "", 1); // initialisieren
			catJSONobj (JSONbuf, id, "UD"); // unbekanntes Device
			return; // queryStates beenden
		}

		if (0xffff == state)
		{
			save_strcpy (JSONbuf, "", 1); // initialisieren
			catJSONobj (JSONbuf, id, "TO"); // TO = timeout while waiting for packet
			return; // queryStates beenden
		}

		i++;
	}
}

void executeCmd (const char * page, uint8_t id, const char * cmd)
{
	uint16_t d = 0; // dummy

	if (is (page, "lampe") || is (page, "sonstige"))
		executeCmd_specific (id, is (cmd, "ein") ? HCAN_HES_POWER_GROUP_ON : HCAN_HES_POWER_GROUP_OFF, d, d);

	else if (is (page, "heizung"))
	{
		if (is (cmd, "aus"))
			executeCmd_specific (id, HCAN_HES_HEIZUNG_SET_MODE_OFF, d, d);

		else if (is (cmd, "auto"))
			executeCmd_specific (id, HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK, d, d);

		else // therm:
		{
			uint16_t dauer = 3 * 3600; // Sekunden! (immer 3 h)
			uint8_t temp;
			if      (is (cmd, "therm"))  	temp = 19;
			else if (is (cmd, "frost"))  	temp = 10;
			else if (is (cmd, "frostLang")) {temp = 10; dauer = 65535;}
			else
			{
				temp = atoi (cmd); // sonst steht eine Temp. im cmd
				if (temp < 0 || temp > 30) return; // Fehler!
			}

			uint16_t tempX16 = temp * 16;
			executeCmd_specific (id, HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS, dauer, tempX16);
		}
	}
	else if (is (page, "rolladen"))
	{
		uint8_t pos = atoi (cmd);
		if (pos < 0 || pos > 100) return; // Fehler!

		executeCmd_specific (id, HCAN_HES_ROLLADEN_POSITION_SET, pos, d);
	}
	else if (is (page, "reedkontakt") || is (page, "tempsensor"))
	{}	//("sprich");

	else
	{}	// "executeEinzelCmd-Fehler: page=" + page
}

void executeCmds (const char * page, uint8_t * ids, const char * cmd)
{
	uint8_t i = 0;
	while (255 != ids[i]) // ueber alle ids
	{
		executeCmd (page, ids[i], cmd);
		i++;
		// ggf. sleep (1); // ms
	}
}

