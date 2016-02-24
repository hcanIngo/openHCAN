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
 *  (c) 2016 Ingo Lages, i dot Lages (at) gmx (dot) de
 */
#include "mqttclient.h"
// Dieser MQTT-Client baut eine Verbindung zum Broker (localhost:1883) auf:
// - abonniert (subscribes) das Thema (topic) "cb<". Daher Nachrichten an den CAN-Bus.
// - veroeffentlicht (publishes) alle HES-Nachrichten unter dem Thema (topic) "cb>". Daher Nachrichten vom CAN-Bus.

// Fehlerausgabe und beenden
void exitm(const char *msg)
{
	fprintf(stderr, "%s", msg);
	exit(1);
}

// Mosquitto-Client initialisieren
void initm(struct client_info *info)
{
	void *udata = (void *) info;

	// Mosquitto-Client mit dem Namen "cb_client" (= CAN-Bus-Client)
	struct mosquitto *m = mosquitto_new("cb_client", true, udata);
	TRACE("mosquitto_new -> new cb_client\n");

	int major, minor, revision;
	mosquitto_lib_version(&major, &minor, &revision);
	if(debug) printf("mosquitto-lib-version: %d.%d.%d\n", major, minor, revision);

	if (m == NULL)
	{
		exitm("init() failure\n");
	}
	info->m = m;
	if (!set_callbacksm(m))
	{
		exitm("set_callbacks() failure\n");
	}
	if (!connectm(m))
	{
		exitm("connectm() failure\n");
	}


}

// Callback, wenn die Verbindung zum Broker steht: Dann wird abonniert
static void on_connect(struct mosquitto *m, void *udata, int res)
{
	TRACE("on_connect\n");

	if (res == 0)
	{
		mosquitto_subscribe(m, NULL, "cb<", 0);   // Nachrichten zum CAN-Bus
		//mosquitto_subscribe(m, NULL, "cb</HES", 0); // HES-Nachrichten zum CAN-Bus
	}
	else
	{
		exitm("connection refused\n");
	}
}

// Eine Nachricht an den Broker wurde erfolgreich veroeffentlicht
static void on_publish(struct mosquitto *m, void *udata, int m_id)
{
	TRACE(":) published successfully\n");
}

static bool is(const char *topic, const char *key)
{
	return 0 == strncmp(topic, key, strlen(key));
}

static inline uint8_t get_proto(uint32_t extid)
{
	return (extid >> 20) & 0x07;
}

void publishm(struct client_info *info)
{
	char from_cb[64];
	char from_cb_tmp[128];
	size_t payloadlen = 0;

	if (info->cf.size > 8)
		RETURN("info->cf.size > 8");

	if (1 == get_proto(info->cf.id)) // proto = 1 (=SFP) ?
	{
		if (5 == info->cf.data[0]) // 5=HES  von SFP
		{
			// HES-Nachricht an den broker leiten:
			payloadlen = snprintf(from_cb, sizeof(from_cb), "HES");
			int i;
			for (i = 1; i < info->cf.size; i++)
			{
				snprintf(from_cb_tmp, sizeof(from_cb_tmp), "/%d",
						info->cf.data[i]);
				strncat(from_cb, from_cb_tmp, sizeof(from_cb));
			}
			payloadlen = strlen(from_cb);

			TRACE("cb>: %s size=%u\n", from_cb, payloadlen); // topic "cb>" (vom CAN-Bus)
			int res = mosquitto_publish(info->m, NULL, "cb>", payloadlen,
					from_cb, 0, false);
			if (res != MOSQ_ERR_SUCCESS)
			{
				TRACE("ERROR: res=%d\n", res);
				exitm("exit @ publish\n");
			}
		}
		else
			TRACE("not HES -> not published\n");
	}
	else
		TRACE("not SFP protocol -> not published\n");
}

// Empfangsbearbeitung einer der abonnierten Nachrichten
static void on_message(struct mosquitto *m, void *udata, const struct mosquitto_message *msg)
{
	if (msg == NULL)
	{
		return;
	}
	TRACE(":) got message @ %s: (%d, QoS %d, %s) '%s'\n", msg->topic,
			msg->payloadlen, msg->qos, msg->retain ? "R" : "!r", (char *)msg->payload);

	struct client_info *info = (struct client_info *) udata;

	if (is(msg->topic, "cb<"))
	{
		char **topics;
		int topic_count;
		mosquitto_sub_topic_tokenise(msg->payload, &topics, &topic_count);
		if (topic_count < 2 || topic_count > 9) // Mindestens ein Datenbyte und maximal 8 Datenbytes
			return;

		int i;
		for (i = 1; i < topic_count; i++)
		{
			frame_to_can4linux.data[i] = (uint8_t) atoi(topics[i]);
		}
		frame_to_can4linux.length = topic_count; // Anzahl der Daten-Bytes des Frames

		if (is(topics[0], "HES"))
		{
			frame_to_can4linux.data[0] = 5; // HES
			frame_to_can4linux.id = 0x2108E00; // src: 512, dst: 35=HCAN_MULTICAST_CONTROL, proto: z.B. 1=SFP, Prio: 2
			info->cf_to_read = 1;
		}
		mosquitto_sub_topic_tokens_free(&topics, topic_count);
	}
	else if (is(msg->topic, "HMS/"))
	{
		TRACE(":) HMS is not implementated yet\n");
	}
}

// Erfolgreich abonniert
static void on_subscribe(struct mosquitto *m, void *udata, int mid,
		int qos_count, const int *granted_qos)
{
	TRACE(":) subscribed successfully\n");
}

// Registrierung der callbacks die die mosquitto-Verbindung nutzen
bool set_callbacksm(struct mosquitto *m)
{
	mosquitto_connect_callback_set(m, on_connect);
	mosquitto_publish_callback_set(m, on_publish);
	mosquitto_subscribe_callback_set(m, on_subscribe);
	mosquitto_message_callback_set(m, on_message);
	return true;
}

// Netzwerkverbindung zum MQTT-Broker
bool connectm(struct mosquitto *m)
{
	int res = mosquitto_connect(m, BROKER_HOSTNAME, BROKER_PORT, KEEPALIVE_SECONDS);
	return res == MOSQ_ERR_SUCCESS;
}

void runm(struct client_info *info)
{
	TRACE("runm\n");
	mosquitto_loop(info->m, 1000, 1 /* unused */); // max. 1 s; "incl. select"
}

int endm(struct client_info *info)
{
	int res = mosquitto_loop(info->m, 1000, 1 /* unused */); // max. 1 s; "incl. select"
	mosquitto_destroy(info->m);
	(void) mosquitto_lib_cleanup();
	if (res == MOSQ_ERR_SUCCESS)
	{
		TRACE("endm\n");
		return 0;
	}
	else
		return 1;
}
