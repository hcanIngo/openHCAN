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
#include "../hcan4mqttha/mqttHcan.h"

#include <linux/can.h>
#include <string.h>
#include <json-c/json.h>
#include "../hcan4mqttha/mqttClient.h"
#include "../hcan4mqttha/parseXml.h"
#define PATH_TO_JSON_FILES "/etc/hcan/"

typedef enum eDev       {    eLight = 0,  eSwitch = 1,  eClimate = 2,  eTempsensor = 3,  eReedkontakt = 4,  eCover = 5,  eAnzDev = 6 } teDev;
char* strDevName[]    = {    "light",     "switch",     "climate",     "tempsensor",     "reedkontakt",     "cover"                  }; // Teil-Dateiname
typedef struct configTopic
{
	char TildeOhneId[35]; // "~" z.B. "hc/switch" aus switch_config.json   oder    "sensor/TEMPERATURE"  aus tempsensor_config.json
	char stat_t[30];      // "stat_t" = state  -Topic  z.B. "/-->/state"
	char cmd_t[20];       // "cmd_t"  = command-Topic  z.B. "/<--/set"
} tConfigTopic;
tConfigTopic strDateiEintrag[eAnzDev];

typedef struct configTopicClimate // die climate_config.json ist etwas speziell und erhaelt daher eine zusaetliche Struktur:
{
	char mode_cmd_t[30];   // "mode_cmd_t" = state  -Topic  z.B. "/<--/thermostatModeCmd"
	char temp_cmd_t[30];   // "temp_cmd_t"  = command-Topic  z.B. "/<--/targetTempCmd"
	// ggf. temp_stat_tpl  // z.B. "{{value_json.target_temp}}"
	// ggf. curr_temp_tpl  // z.B. "{{value_json.current_temp}}"
} tconfigTopicClimate;
tconfigTopicClimate strDateiEintragClimate;

void initMqttHcan(void)
{
	for(teDev eDev=eLight; eDev < eAnzDev; eDev++) // ueber alle Devices
	{
		memset(strDateiEintrag[eDev].TildeOhneId, '\0', sizeof strDateiEintrag[eDev].TildeOhneId);
		memset(strDateiEintrag[eDev].stat_t, '\0', sizeof strDateiEintrag[eDev].stat_t);
		memset(strDateiEintrag[eDev].cmd_t, '\0', sizeof strDateiEintrag[eDev].cmd_t);
	}
	memset(strDateiEintragClimate.mode_cmd_t, '\0', sizeof strDateiEintragClimate.mode_cmd_t);
	memset(strDateiEintragClimate.temp_cmd_t, '\0', sizeof strDateiEintragClimate.temp_cmd_t);

	for(uint8_t id=0; id < anzIds; id++) // ueber alle Devices
	{
		dev[id].tempSensor.HeizkoerperId = 255; // Zuordnung (TempSensor an Heizkoerper) noch unbekannt
		dev[id].tempSensor.UIelement = false;
		//snprintf(dev[id].mode, sizeof dev[id].mode, "off");
	}
}

bool is(const char *topic, const char *key)
{
	if (topic == NULL)
		return 0;

	// TRACE("# # %s==%s Len=%u\n", topic, key, (uint16_t)strlen(key));
	return 0 == strncmp(topic, key, strlen(key));
}

static bool isEq(const char *s1, const char *s2, size_t bisPos)
{
	return 0 == strncmp(s1, s2, bisPos) ? true:false;
}

static void holeConfigDateiEintragClimate(struct json_object * jPayload, const char * key)
{
	//const char * strQuelle = json_object_get_string(json_object_object_get(jPayload, key)); // deprecated
	struct json_object * jValue;
	json_object_object_get_ex(jPayload, key, &jValue);
	const char * strQuelle = json_object_get_string(jValue);
	if (is (key,"mode_cmd_t"))
	{
		size_t vonPos = sizeof "~" - 1; //  "~" auslassen (z.B. "~/<--/thermostatModeCmd")
		size_t bisPos = strnlen(strQuelle, sizeof strDateiEintragClimate.mode_cmd_t);
		memset(strDateiEintragClimate.mode_cmd_t, '\0', sizeof strDateiEintragClimate.mode_cmd_t); // WICHTIG!
		snprintf(strDateiEintragClimate.mode_cmd_t, bisPos+1, "%s", &strQuelle[vonPos]);
		TRACE("\n\t ### %s-Eintrag aus %s_config.json: %s -> %s|### ", key, strDevName[eClimate], strQuelle, strDateiEintragClimate.mode_cmd_t);
	}
	else if (is (key,"temp_cmd_t"))
	{
		size_t vonPos = sizeof "~" - 1; //  "~" auslassen (z.B. "~/<--/targetTempCmd")
		size_t bisPos = strnlen(strQuelle, sizeof strDateiEintragClimate.temp_cmd_t); // bis wohin kopiert wird
		memset(strDateiEintragClimate.temp_cmd_t, '\0', sizeof strDateiEintragClimate.temp_cmd_t); // WICHTIG!
		snprintf(strDateiEintragClimate.temp_cmd_t, bisPos+1, "%s", &strQuelle[vonPos]);
		TRACE("\n\t ### %s-Eintrag aus %s_config.json: %s -> %s|### ", key, strDevName[eClimate], strQuelle, strDateiEintragClimate.temp_cmd_t);
	}
}

// bei z.B. key="~" aus switch_config.json  laden   und   "/id" auslassen
static void holeConfigDateiEintrag(struct json_object * jPayload, teDev eDev, const char * key)
{
	struct json_object * jValue;
	json_object_object_get_ex(jPayload, key, &jValue);
	const char * strQuelle = json_object_get_string(jValue);
	if (is (key,"~"))
	{
		size_t vonPos = 0; // vom Anfang
		size_t bisPos = strnlen(strQuelle, sizeof strDateiEintrag[eDev].TildeOhneId) - (sizeof "/id" - 1); // bis wohin kopiert wird, daher "/id" entfernen
		memset(strDateiEintrag[eDev].TildeOhneId, '\0', sizeof strDateiEintrag[eDev].TildeOhneId); // WICHTIG!
		snprintf(strDateiEintrag[eDev].TildeOhneId, bisPos+1, "%s", &strQuelle[vonPos]);
		TRACE("\n\t ### %s-Eintrag aus %s_config.json: %s -> %s|### ", key, strDevName[eDev], strQuelle, strDateiEintrag[eDev].TildeOhneId);
	}
	else if (is (key,"stat_t") || ((eClimate == eDev) && is (key,"mode_stat_t")))
	{
		size_t vonPos = sizeof "~" - 1; //  "~" auslassen (z.B. "~/-->/state")
		size_t bisPos = strnlen(strQuelle, sizeof strDateiEintrag[eDev].stat_t); // bis wohin kopiert wird
		memset(strDateiEintrag[eDev].stat_t, '\0', sizeof strDateiEintrag[eDev].stat_t); // WICHTIG!
		snprintf(strDateiEintrag[eDev].stat_t, bisPos+1, "%s", &strQuelle[vonPos]);
		TRACE("\n\t ### %s-Eintrag aus %s_config.json: %s -> %s|### ", key, strDevName[eDev], strQuelle, strDateiEintrag[eDev].stat_t);
	}
	else if (is (key,"cmd_t"))
	{
		size_t vonPos = sizeof "~" - 1; //  "~" auslassen (z.B. "~/<--/set")
		size_t bisPos = strnlen(strQuelle, sizeof strDateiEintrag[eDev].cmd_t); // bis wohin kopiert wird
		memset(strDateiEintrag[eDev].cmd_t, '\0', sizeof strDateiEintrag[eDev].cmd_t); // WICHTIG!
		snprintf(strDateiEintrag[eDev].cmd_t, bisPos+1, "%s", &strQuelle[vonPos]);
		TRACE("\n\t ### %s-Eintrag aus %s_config.json: %s -> %s|### ", key, strDevName[eDev], strQuelle, strDateiEintrag[eDev].cmd_t);
	}
}

static size_t setClimateCanMsg(teDev eDev, char* restTopic, size_t bisPos, char* payload_rx, struct can_frame * msg4cb)
{
	uint8_t rtnCanMsgLen = 0; // kein Topic (keine Message) fuer den CAN-Bus

	if (isEq(strDateiEintrag[eDev].stat_t, restTopic, bisPos)) // z.B. "/-->/state" mit topic_rx-Ende vergleichen
	{
		msg4cb->data[1] = HCAN_HES_HEIZUNG_TIST_REQUEST;
		rtnCanMsgLen = 3; // Anzahl der Daten-Bytes des Frames
	}
	else if (isEq(strDateiEintragClimate.mode_cmd_t, restTopic, bisPos)) //  z.B. "/<--/thermostatModeCmd"
	{
		if (is(payload_rx, "off"))
		{
			msg4cb->data[1] = HCAN_HES_HEIZUNG_SET_MODE_OFF;
			rtnCanMsgLen = 3; // Anzahl der Daten-Bytes des Frames
		}
		else if (is(payload_rx, "auto"))
		{
			msg4cb->data[1] = HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK; // autom  da nur mit heat die Zieltemp-Angabe fehlt:
			rtnCanMsgLen = 3;
		}
		else // "cool" oder "heat":
		{
			msg4cb->data[1] = HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;

			dev[msg4cb->data[2]].tSoll = 10.; // "cool" Frostschutz
			uint16_t dauer = 0;               // "cool" 0 = ohne Zeitangabe unbegrenzt (falls der naechste Token fehlt)
			if (is(payload_rx, "heat"))
			{
				dev[msg4cb->data[2]].tSoll = 19.5; // "heat"
				dauer = (uint16_t)(3600.0f * 3);   // 3 hours -> secs
			}
			// else: "cool"

			uint16_t temp = (uint16_t)(16. * dev[msg4cb->data[2]].tSoll);
			msg4cb->data[3] = temp>>8; // temp_hi : Soll-Temperatur (MSB)
			msg4cb->data[4] = temp; // temp_li : Soll-Temperatur (LSB)

			msg4cb->data[5] = dauer>>8; // duration_hi : Restdauer (MSB), 0 = unbegrenzt
			msg4cb->data[6] = dauer; // duration_lo : Restdauer (LSB)
			rtnCanMsgLen = 7;
		}
	}
	else if (isEq(strDateiEintragClimate.temp_cmd_t, restTopic, bisPos)) //  z.B. "/<--/targetTempCmd"
	{
		msg4cb->data[1] = HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS;
		dev[msg4cb->data[2]].tSoll = atof(payload_rx);

		uint16_t temp = (uint16_t)(16. * dev[msg4cb->data[2]].tSoll);
		msg4cb->data[3] = temp>>8; // temp_hi : Soll-Temperatur (MSB)
		msg4cb->data[4] = temp; // temp_li : Soll-Temperatur (LSB)

		uint16_t dauer = 0; // 0 = ohne Zeitangabe unbegrenzt (falls der naechste Token fehlt)
		dauer = (uint16_t) (3600.0f * 3.); // 3 hours -> secs
		msg4cb->data[5] = dauer>>8; // duration_hi : Restdauer (MSB), 0 = unbegrenzt
		msg4cb->data[6] = dauer; // duration_lo : Restdauer (LSB)
		rtnCanMsgLen = 7;
		//TRACE("targetTempCmd=%.1f\n", (float)temp/16.);
	}

	return rtnCanMsgLen;
}

size_t prepareMsg4cb (char* topic_rx, char* payload_rx, struct can_frame * msg4cb)
{
	uint8_t rtnCanMsgLen = 0; // kein Topic (keine Message) fuer den CAN-Bus

	for(teDev eDev=eLight; (eDev < eAnzDev) && (rtnCanMsgLen == 0); eDev++) // ueber alle Devices  bis  zur ersten Uebereinstimmung
	{
		size_t bisPos = strnlen(strDateiEintrag[eDev].TildeOhneId, sizeof strDateiEintrag[eDev].TildeOhneId);
		if (isEq(strDateiEintrag[eDev].TildeOhneId, topic_rx, bisPos)) // z.B. "hc/light" mit topic_rx-Anfang vergleichen
		{
			char rest = '\0';
			char * restTopic;
			msg4cb->data[2] = strtol(&topic_rx[bisPos+1], &restTopic, 10); // Gruppe/ID
			if (restTopic == NULL) restTopic = &rest;
			size_t bisPos = strnlen(restTopic, 30) - strnlen(payload_rx, 20) - 1; // restTopic enthaelt zusatzlich das Payload (-1, da von 0 gezaehlt)
			TRACE("\n\t ### id=%d restTopic=%s bisPos=%u (%s)\n", msg4cb->data[2], restTopic, bisPos, strDevName[eDev]);

			switch (eDev)
			{
				case eLight:
				case eSwitch:
					if (isEq(strDateiEintrag[eDev].stat_t, restTopic, bisPos)) // z.B. "/-->/state" mit restTopic=topic_rx-Ende vergleichen
					{
						msg4cb->data[1] = HCAN_HES_POWER_GROUP_STATE_QUERY;
						rtnCanMsgLen = 3; // Anzahl der Daten-Bytes des Frames
					}
					else if (isEq(strDateiEintrag[eDev].cmd_t, restTopic, bisPos)) // z.B. "/-->/set" mit restTopic=topic_rx-Ende vergleichen
					{
						msg4cb->data[1] = is(payload_rx, "ON") ? HCAN_HES_POWER_GROUP_ON:HCAN_HES_POWER_GROUP_OFF;
						rtnCanMsgLen = 3; // Anzahl der Daten-Bytes des Frames
					}
					break;

				case eClimate:
					rtnCanMsgLen = setClimateCanMsg(eDev, restTopic, bisPos, payload_rx, msg4cb);
					break;

				case eTempsensor:
				case eReedkontakt:
					if (isEq(strDateiEintrag[eDev].stat_t, restTopic, bisPos)) // z.B. "/-->/state" mit restTopic-Ende vergleichen
					{
						msg4cb->data[1] = eDev == eTempsensor ? HCAN_HES_1WIRE_TEMPERATURE_QUERY : HCAN_HES_REEDKONTAKT_STATE_QUERY;
						rtnCanMsgLen = 3; // Anzahl der Daten-Bytes des Frames
					}
					break;

				case eCover:
					if (isEq(strDateiEintrag[eDev].stat_t, restTopic, bisPos)) // z.B. "/-->/state" mit restTopic=topic_rx-Ende vergleichen
					{
						msg4cb->data[1] = HCAN_HES_ROLLADEN_POSITION_REQUEST;
						rtnCanMsgLen = 3; // Anzahl der Daten-Bytes des Frames
					}
					else if (isEq(strDateiEintrag[eDev].cmd_t, restTopic, bisPos)) // z.B. "/-->/set" mit restTopic=topic_rx-Ende vergleichen
					{
						msg4cb->data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
						if      (is(payload_rx, "CLOSE")) msg4cb->data[3] = 200;   // gewuenschte Rollladenposition: Pos=200=0%=zu
						else if (is(payload_rx, "OPEN"))  msg4cb->data[3] = 201;   // gewuenschte Rollladenposition: Pos=201=100%=auf
						else if (is(payload_rx, "STOP"))  msg4cb->data[3] = 202;   // stop
						// moeglich waere auch            msg4cb->data[3] = 0..100 // gewuenschte Rollladenposition: von 0%..100% vorgeben
						rtnCanMsgLen = 4; // Anzahl der Daten-Bytes des Frames
					}
					break;

					/*  if (is(token, "C")) // Color (RGB-Farbvorgabe)
						{
							msg4cb->data[1] = HCAN_HES_WS2812B_SEND;

							token = strtok(NULL, "/"); if(NULL == token) return 0;
							msg4cb->data[2] = atoi(token); // Gruppe
							token = strtok(NULL, "/"); if(NULL == token) return 0; // token=<--

							token = strtok(NULL, "/"); if(NULL == token) return 0;
							msg4cb->data[3] = atoi(token); // Pattern: 1 = Nicht gewaehlte LEDs gehen aus; 2 = Nicht gewaehlte LEDs bleiben an

							token = strtok(NULL, "/"); if(NULL == token) return 0;
							msg4cb->data[7] = atoi(token); // BenutzeLEDsPattern: 1 = Jede LED (Pattern ohne Wirkung); 2 = jede zweite LED; 3 = jede dritte LED, usw.


							token = strtok(NULL, "/"); if(NULL == token) return 0; // naechster token = R,G,B
							char* rgbToken = strtok(token, ","); if(NULL == rgbToken) return 0;
							msg4cb->data[4] = atoi(rgbToken); // R

							rgbToken = strtok(NULL, ","); if(NULL == rgbToken) return 0;
							msg4cb->data[5] = atoi(rgbToken); // G

							rgbToken = strtok(NULL, ","); if(NULL == rgbToken) return 0;
							msg4cb->data[6] = atoi(rgbToken); // B

							return 8; // Anzahl der Daten-Bytes des Frames
						} */
				default:
					{}
			}
		}
	}

	return rtnCanMsgLen;
}

static inline uint8_t get_proto(uint32_t extid)
{
	return (extid >> 20) & 0x07;
}

static inline uint8_t isItTopic4Broker(uint8_t msg4broker)
{
	switch (msg4broker)
	{
		case HCAN_HES_POWER_GROUP_CONFIG_REPLAY:
		case HCAN_HES_POWER_GROUP_STATE_REPLAY:
		case HCAN_HES_POWER_GROUP_STATE_INFO:

		case HCAN_HES_ROLLADEN_CONFIG_REPLAY:
		case HCAN_HES_ROLLADEN_POSITION_REPLAY:
		case HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO:

		case HCAN_HES_1WIRE_TEMPERATURE_CONFIG_REPLAY:
		case HCAN_HES_1WIRE_TEMPERATURE:
		case HCAN_HES_1WIRE_TEMPERATURE_REPLAY:

		//case HCAN_HES_REEDKONTAKT_OFFEN:
		case HCAN_HES_REEDKONTAKT_CONFIG_REPLAY:
		case HCAN_HES_REEDKONTAKT_STATE_REPLAY:
		case HCAN_HES_REEDKONTAKT_STATE_CHANGE:

		case HCAN_HES_HEIZUNG_CONFIG_REPLAY:
		case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS:
		case HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS:
		case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS:
		case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS:

		case HCAN_HES_HEIZUNG_TIST_REPLAY:
			return 1; // fuer den Broker von Interesse

		default:
			return 0; // fuer den Broker uninteressant
	}
}

static size_t erzeugeConfigTopicUndPayload(teDev eDev, unsigned char id, char * strTopic, char * strPayload)
{
	bool devInXmlEnthalten = getXmlDatenZurId(id);
	if (!devInXmlEnthalten) return 0; // fuer UI nicht vorgesehen

	if ((eLight == eDev) && dev[id].sonstige) eDev = eSwitch; // eSwitch verwenden (dev[id].sonstige wurde in getXmlDatenZurId() ermittelt)


	char strDateiname[125];
	snprintf(strDateiname, sizeof strDateiname, "%s%s_config.json", PATH_TO_JSON_FILES, strDevName[eDev]);
	FILE *fp = fopen(strDateiname, "r");
	if (!fp) // json-Datei dort nicht vorhanden?
	{
		syslog(LOG_ERR, "\nFehler: %s NICHT gefunden! \n", strDateiname);
		TRACE("\nFehler: %s NICHT gefunden! \n", strDateiname);
		exit (1);
	}
	fread(strPayload, dStrLen, 1, fp);
	fclose(fp);
	struct json_object *jPayload = json_tokener_parse(strPayload);
	json_object_object_add(jPayload, "name", json_object_new_string(stt)); // name ersetzen (stt wird in getXmlDatenZurId() ermittelt)
	char unique_id[10];
	snprintf(unique_id, sizeof unique_id, "hcanId%u", id);
	json_object_object_add(jPayload, "unique_id", json_object_new_string(unique_id));

	struct json_object *jDevice = json_object_new_object();
	json_object_object_add(jDevice, "configuration_url", json_object_new_string("https://github.com/hcanIngo/openHCAN/wiki"));

	struct json_object *jArray = json_object_new_array();
	json_object_array_add(jArray, json_object_new_string(unique_id)); // oder hcan2mqtt_idXY
	json_object_object_add(jDevice, "identifiers", jArray);

	json_object_object_add(jDevice, "manufacturer", json_object_new_string("openHCAN"));
	json_object_object_add(jDevice, "suggested_area", json_object_new_string(dev[id].Bereichsname));
	json_object_object_add(jDevice, "name", json_object_new_string("")); // doppelte Benamung in HA verhindern
	json_object_object_add(jDevice, "sw_version", json_object_new_string("1.0"));
	json_object_object_add(jPayload, "device", jDevice);


	switch (eDev)
	{
		case eTempsensor:
		case eReedkontakt:
			holeConfigDateiEintrag(jPayload, eDev, "~");
			holeConfigDateiEintrag(jPayload, eDev, "stat_t");
			break;

		case eLight:
		case eSwitch:
			holeConfigDateiEintrag(jPayload, eDev, "~");
			holeConfigDateiEintrag(jPayload, eDev, "stat_t");
			holeConfigDateiEintrag(jPayload, eDev, "cmd_t");
			break;

		case eCover:
			holeConfigDateiEintrag(jPayload, eDev, "~");
			holeConfigDateiEintrag(jPayload, eDev, "stat_t");
			holeConfigDateiEintrag(jPayload, eDev, "cmd_t");
			break;

		case eClimate:
			holeConfigDateiEintrag(jPayload, eDev, "~");
			holeConfigDateiEintrag(jPayload, eDev, "mode_stat_t"); // in climate.json muessen mode_stat_t = temp_stat_t = curr_temp_t gleich konfiguiert werden (mode_stat_t wird in stat_t gespeichert)
			holeConfigDateiEintragClimate(jPayload, "mode_cmd_t");
			holeConfigDateiEintragClimate(jPayload, "temp_cmd_t");
			break;

		default:
			{}
	}

	int vonPos = (sizeof "hc/" - 1);
	snprintf(strTopic, dStrLen, "homeassistant/%s/%s/%u/config", &(strDateiEintrag[eDev].TildeOhneId)[vonPos], dev[id].Bereichsname, id); // &strDateiEintrag[eDev].TildeOhneId[vonPos]  ist z.B.  binary_sensor

	char strTopicUndId[35];
	snprintf(strTopicUndId, sizeof strTopicUndId, "%s/%u", strDateiEintrag[eDev].TildeOhneId, id); // konkrete id einfuegen
	json_object_object_add(jPayload, "~",  json_object_new_string(strTopicUndId));
	return snprintf(strPayload, dStrLen, "%s", json_object_to_json_string(jPayload));
}

size_t catHesTopic4Broker(char * strTopic, char * strPayload, const struct can_frame * cf)
{
	if (!HaOnline) // HA noch nicht online, daher wurde RQC noch nicht versendet?
		return 0; // noch nichts an HA-UI senden

	const size_t maxSize = dStrLen;

	switch (cf->data[1])
	{
		case HCAN_HES_POWER_GROUP_CONFIG_REPLAY:
			return erzeugeConfigTopicUndPayload(eLight,  cf->data[2], strTopic, strPayload);

		case HCAN_HES_POWER_GROUP_STATE_REPLAY:
		case HCAN_HES_POWER_GROUP_STATE_INFO: // ggf. \"dev_cla\": \"motion\"  fuer Bewegungsmelder-switch     siehe https://www.home-assistant.io/integrations/cover#device-class
		{
			teDev eDev = dev[cf->data[2]].sonstige ? eSwitch:eLight;
			snprintf(strTopic, maxSize, "%s/%u%s", strDateiEintrag[eDev].TildeOhneId, cf->data[2], strDateiEintrag[eDev].stat_t); // switch damit es in der Oberflaeche "schaltet"
			return snprintf(strPayload, maxSize, "%s", cf->data[3] ? "ON":"OFF");
		}

		case HCAN_HES_1WIRE_TEMPERATURE_CONFIG_REPLAY :
				// Temperatursensoren erst nach den Heizungen erstellen: Die Temperatursensoren, die einer Heizung zugeordet sind erhalten keinen eigenen UI-Eintrag!
				return 0;

		case HCAN_HES_1WIRE_TEMPERATURE:
		case HCAN_HES_1WIRE_TEMPERATURE_REPLAY:
			if ((dev[cf->data[2]].tempSensor.HeizkoerperId == 255) && !dev[cf->data[2]].tempSensor.UIelement) // Temperatursensor (noch) keinem Heizkoerper zugeordnet  und  noch nicht in UI?
			{
				dev[cf->data[2]].tempSensor.UIelement = true;
				return erzeugeConfigTopicUndPayload(eTempsensor,  cf->data[2], strTopic, strPayload); // erst nach den Heizungen erstellen!
			}


			dev[cf->data[2]].tIst = (float)((cf->data[3]<<8)|cf->data[4]) / 16.0; // Ist-Temperaturwert
			if (dev[cf->data[2]].tempSensor.HeizkoerperId == 255) // Temperatursensor ist keinem Heizkoerper zugeordnet?
			{
				snprintf(strTopic, maxSize, "%s/%u%s", strDateiEintrag[eTempsensor].TildeOhneId, cf->data[2], strDateiEintrag[eTempsensor].stat_t);
				return snprintf(strPayload, maxSize, "{\"temperature\": %.1f}", dev[cf->data[2]].tIst); // mit einer Nachkommastelle
			}
			else // Temperatursensor bei Heizkoerper -> Anzeige nur in UI-Heizkarte:
			{
				uint8_t HeizkoerperId = dev[cf->data[2]].tempSensor.HeizkoerperId;
				dev[HeizkoerperId].tIst = dev[cf->data[2]].tIst; // auch fuer den Heizkoerper die tIst aktuell halten

				// Dann kann tIst in der UI-Heizkarte aktualisiert werden:
				snprintf(strTopic, maxSize, "%s/%u%s", strDateiEintrag[eClimate].TildeOhneId, HeizkoerperId, strDateiEintrag[eClimate].stat_t);
				return snprintf(strPayload, maxSize, "{\"mode\":\"%s\", \"target_temp\":\"%.2f\", \"current_temp\":\"%.1f\"}", dev[HeizkoerperId].mode, dev[HeizkoerperId].tSoll, dev[HeizkoerperId].tIst);
			}

		case HCAN_HES_REEDKONTAKT_CONFIG_REPLAY :
			return erzeugeConfigTopicUndPayload(eReedkontakt,  cf->data[2], strTopic, strPayload);

		case HCAN_HES_REEDKONTAKT_STATE_REPLAY:
		case HCAN_HES_REEDKONTAKT_STATE_CHANGE:
			snprintf(strTopic, maxSize, "%s/%u%s", strDateiEintrag[eReedkontakt].TildeOhneId, cf->data[2], strDateiEintrag[eReedkontakt].stat_t);
			return snprintf(strPayload, maxSize, "%s", cf->data[3] ? "OPEN":"CLOSE");


		case HCAN_HES_HEIZUNG_CONFIG_REPLAY:
			dev[cf->data[3]].tempSensor.HeizkoerperId = cf->data[2]; // fuer Tempsensor speichern: cf->data[3] enthaelt die Temperatursensor-Id
			return erzeugeConfigTopicUndPayload(eClimate,  cf->data[2], strTopic, strPayload); // https://community.home-assistant.io/t/help-me-with-hvac-mqtt-climate-component/97899/8

		case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS:
			snprintf(strTopic, maxSize, "%s/%u%s", strDateiEintrag[eClimate].TildeOhneId, cf->data[2], strDateiEintrag[eClimate].stat_t);
			snprintf(dev[cf->data[2]].mode, sizeof dev[cf->data[2]].mode, "off");
			return snprintf(strPayload, maxSize, "{\"mode\":\"%s\"}", dev[cf->data[2]].mode);
		case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS:
			dev[cf->data[2]].tSoll = (float)((cf->data[3]<<8)|cf->data[4]) / 16.0; // Soll-Temperaturwert
			// int dauer =	(int)((cf->data[5]<<8)|cf->data[6]) / 3600.0; // Dauer
			//TRACE("\n\t ### dauer=%dh\n", dauer);
			snprintf(strTopic, maxSize, "%s/%u%s", strDateiEintrag[eClimate].TildeOhneId, cf->data[2], strDateiEintrag[eClimate].stat_t);
			snprintf(dev[cf->data[2]].mode, sizeof dev[cf->data[2]].mode, dev[cf->data[2]].tSoll > 10. ? "heat":"cool");
			return snprintf(strPayload, maxSize, "{\"mode\":\"%s\", \"target_temp\":\"%.2f\", \"current_temp\":\"%.1f\"}", dev[cf->data[2]].mode, dev[cf->data[2]].tSoll, dev[cf->data[2]].tIst);

		case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS:
			dev[cf->data[2]].tSoll = (float)((cf->data[3]<<8)|cf->data[4]) / 16.0; // Temperaturwert
			dev[cf->data[2]].tIst  = (float)((cf->data[6]<<8)|cf->data[7]) / 16.0; // Ist-Temperaturwert
			snprintf(strTopic, maxSize, "%s/%u%s", strDateiEintrag[eClimate].TildeOhneId, cf->data[2], strDateiEintrag[eClimate].stat_t);
			snprintf(dev[cf->data[2]].mode, sizeof dev[cf->data[2]].mode, "auto");
			return snprintf(strPayload, maxSize, "{\"mode\":\"%s\", \"target_temp\":\"%.1f\", \"current_temp\":\"%.1f\"}", dev[cf->data[2]].mode, dev[cf->data[2]].tSoll, dev[cf->data[2]].tIst);

		case HCAN_HES_HEIZUNG_TIST_REPLAY:
			dev[cf->data[2]].tIst =	(float)((cf->data[3]<<8)|cf->data[4]) / 16.0; // Ist-Temperaturwert
			snprintf(strTopic, maxSize, "%s/%u%s", strDateiEintrag[eClimate].TildeOhneId, cf->data[2], strDateiEintrag[eClimate].stat_t);
			return snprintf(strPayload, maxSize, "{\"mode\":\"%s\", \"target_temp\":\"%.2f\", \"current_temp\":\"%.1f\"}", dev[cf->data[2]].mode, dev[cf->data[2]].tSoll, dev[cf->data[2]].tIst);


		case HCAN_HES_ROLLADEN_CONFIG_REPLAY:
			return erzeugeConfigTopicUndPayload(eCover,  cf->data[2], strTopic, strPayload);

		case HCAN_HES_ROLLADEN_POSITION_REPLAY:
		case HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO:
			snprintf(strTopic, maxSize, "%s/%u%s", strDateiEintrag[eCover].TildeOhneId, cf->data[2], strDateiEintrag[eCover].stat_t);
			switch (cf->data[3])
			{
				// state-Payload: closed, closing, open, opening, stopped
				case 200:
					return snprintf(strPayload, maxSize, "%s", "closing");
				case 201:
					return snprintf(strPayload, maxSize, "%s", "opening");
				case 202:
					return snprintf(strPayload, maxSize, "%s", "stopped");
				default:
				{
					uint8_t pos = cf->data[3]; // kein Invertieren notwendig (HA:  0 indicates closed position;  hcan: 0%=zu)
					return snprintf(strPayload, maxSize, "%s", pos == 100 ? "open":"closed");
				}
			}
			break;



		default:
			TRACE("ERROR: canData[1]=%d\n", cf->data[1]);
			return 0;
	}
}

uint8_t msgOfInterest4broker(const struct can_frame * cf)
{
	if (2 > cf->can_dlc)
	{
		TRACE("can_dlc=%d NG\n", cf->can_dlc);
		return 0;
	}

	uint8_t msgOfInterest =    (1 == get_proto(cf->can_id)) // proto = 1 (=SFP) ?
							&& (5 == cf->data[0]) 		    // 5=HES  von SFP
							&& isItTopic4Broker(cf->data[1]);
	if (!msgOfInterest && (debug > 1) )
		TRACE("not for broker cf[0]=%d\n", cf->data[0]);

	return msgOfInterest;
}
