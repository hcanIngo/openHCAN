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
#include "../hcan4mqttha/parseXml.h"
#include "../hcan4mqttha/mqttClient.h"

#include <libxml/parser.h>

int nodeAnz;
#define maxNodeAnz 1000
char Raumname[15];
char stt[lenStt];
bool sonstige = false;
bool devInXmlEnthalten = false;

hcanInstallation dev[anzIds];
char filenameInstallationXml[lenFilename];


// Bereichsnamen, Raumnamen sichern. Finden wir die gesuchte id dann stt, Bereichsnamen und Raumnamen zurueckgeben.
static void getDatenZuId(xmlNode *node, unsigned char gesuchteId, const xmlChar * strId)
{
	while (node && nodeAnz < maxNodeAnz)
	{
		if (node->type == XML_ELEMENT_NODE)
		{
			if (xmlStrEqual(node->name, (const xmlChar *)"bereich")) // bereich gefunden?
			{
				snprintf(dev[gesuchteId].Bereichsname, sizeof dev[gesuchteId].Bereichsname, "%s", xmlGetProp(node, (const xmlChar *)"name"));
			}
			else if (xmlStrEqual(node->name, (const xmlChar *)"raum")) // raum gefunden?
			{
				snprintf(Raumname, sizeof Raumname, "%s", xmlGetProp(node, (const xmlChar *)"name"));
			}


			if (xmlStrEqual(strId, xmlGetProp(node, (const xmlChar *)"gruppe"))) // id gefunden?
			{
				if (xmlStrEqual((const xmlChar *)"sonstige", xmlGetProp(node, (const xmlChar *)"typ")))
					dev[gesuchteId].sonstige = true;
				else dev[gesuchteId].sonstige = false;

				if (debug > 0) snprintf(stt, sizeof stt, "%s %s %u", dev[gesuchteId].Bereichsname, xmlGetProp(node, (const xmlChar *)"stt"), gesuchteId);
				else           snprintf(stt, sizeof stt, "%s %s", dev[gesuchteId].Bereichsname, xmlGetProp(node, (const xmlChar *)"stt"));

				//printf("\n\t%s (%s,%s)\n", stt, Raumname, dev[gesuchteId].Bereichsname);
				nodeAnz = maxNodeAnz; // gefunden -> Abbruch
				devInXmlEnthalten = true;
			}
		}

		getDatenZuId(node->children, gesuchteId, strId); // Rekursion!
	    node = node->next;
	    nodeAnz++;
	}
	//printf("\nnodeAnz=%d ...\n", nodeAnz);
}

bool getXmlDatenZurId(unsigned char id)
{
    xmlDoc *document = xmlReadFile(filenameInstallationXml, NULL, 0);
    xmlNode *root = xmlDocGetRootElement(document);
    xmlNode *first_child = root->children;

    memset(stt, '\0', sizeof stt);
    memset(dev[id].Bereichsname, '\0', sizeof dev[id].Bereichsname);
    memset(Raumname, '\0', sizeof Raumname);
    char strId[5];
	snprintf(strId, sizeof strId, "%u", id);
    nodeAnz = 0; // Suche von 0 starten
    devInXmlEnthalten = false;

    getDatenZuId(first_child, id, (const xmlChar *)strId);
    if (0 == strncmp(dev[id].Bereichsname, "", sizeof dev[id].Bereichsname)) // kein Bereichsname?
    	dev[id].Bereichsname[0] = '-'; // fehlenden Bereichsnamen durch '-' ersetzen

    //printf("\n### %s (%s %s) ###\n", stt, dev[id].Bereichsname, Raumname);
	return devInXmlEnthalten;
}

