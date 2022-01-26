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
 *  (c) 2022 Ingo Lages, i dot Lages (at) gmx (dot) de
 */
#ifndef MQTTXML_H_
#define MQTTXML_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <json-c/json.h>

typedef struct Tempsensor
{
	bool UIelement;
	uint8_t HeizkoerperId; // Heizkoerper der diesem Tempsensor zugeordnet ist (255 = Temperatursensor ist keinem Heizkoerper zugeordnet)
} tTempsensor;

typedef struct hcanInstallation
{
	bool sonstige;  // Lampe=false
	char Bereichsname[4];
	//char Raumname[30];
	//unsigned int typ; // light, climate, ...

	tTempsensor tempSensor;
	float tIst; // Ist-Temperatur
	float tSoll; // Soll-Temperatur (fuer Heizkoerper-Stellteil)
	char mode[5]; // auto, heat, cool, off (fuer Heizkoerper-Stellteil)
} hcanInstallation;

#define anzIds 255 // Anzahl Gruppen
extern hcanInstallation dev[anzIds];

#define lenStt 25
extern char stt[lenStt];

#define lenFilename 256
extern char filenameInstallationXml[lenFilename];
#define INSTALLATION_XML "/etc/hcan/installation.xml" // "./installation.xml"

extern bool getXmlDatenZurId(unsigned char id);

#endif
