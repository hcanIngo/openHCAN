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
 *  (c) 2017 Ingo Lages, i dot Lages (at) gmx (dot) de
 */

#include "../hcan4mqttpc/mqttClient.h"

#include <linux/can.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../hcan4mqttpc/mqttHcan.h"
#include "../hcan4mqttpc/MQTTPacket/src/MQTTPacket.h"
#include "../include/hcan-def.h"

/* Dieser MQTT-Client baut eine Verbindung zum Broker auf (z.B. localhost:1883):
 - abonniert (subscribes) das Thema (topic) "cb<". Daher Nachrichten an den CAN-Bus.
 - veroeffentlicht (publishes) bestimmte HES-Nachrichten unter dem Thema (topic) "cb>". Daher Nachrichten vom CAN-Bus.
*/
unsigned char buf[200]; // fuer das Serialisieren / Deserialisieren


// MQTTPacket_read erwartet die folgenden Callback-Signatur:
static int recv_callback(unsigned char* buf, int count)
{
	return recv(sock_mqtt, buf, count, 0);
}

static int transport_open(char* addr, int port)
{
	int* sock = &sock_mqtt;
	int type = SOCK_STREAM;
	struct sockaddr_in address;
#if defined(AF_INET6)
	struct sockaddr_in6 address6;
#endif
	int rc = -1;

	sa_family_t family = AF_INET;

	struct addrinfo *result = NULL;
	struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};
	static struct timeval tv;

	*sock = -1;
	if (addr[0] == '[')
	  ++addr;

	if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0)
	{
		struct addrinfo* res = result;

		/* prefer ip4 addresses */
		while (res)
		{
			if (res->ai_family == AF_INET)
			{
				result = res;
				break;
			}
			res = res->ai_next;
		}

#if defined(AF_INET6)
		if (result->ai_family == AF_INET6)
		{
			address6.sin6_port = htons(port);
			address6.sin6_family = family = AF_INET6;
			address6.sin6_addr = ((struct sockaddr_in6*)(result->ai_addr))->sin6_addr;
		}
		else
#endif
		if (result->ai_family == AF_INET)
		{
			address.sin_port = htons(port);
			address.sin_family = family = AF_INET;
			address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
		}
		else
			rc = -1;

		freeaddrinfo(result);
	}

	if (rc == 0)
	{
		*sock =	socket(family, type, 0);
		if (*sock != -1)
		{
			if (family == AF_INET)
				rc = connect(*sock, (struct sockaddr*)&address, sizeof(address));
	#if defined(AF_INET6)
			else
				rc = connect(*sock, (struct sockaddr*)&address6, sizeof(address6));
	#endif
		}
	}
	if (sock_mqtt == INVALID_SOCKET)
		return rc;

	tv.tv_sec = 1;  /* 1 second Timeout */
	tv.tv_usec = 0;
	setsockopt(sock_mqtt, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
	return sock_mqtt;
}

// Fehlerausgabe und beenden
static void exitMqtt(const char* msg)
{
	TRACE(msg);
	shutdown(sock_mqtt, SHUT_WR);
	recv(sock_mqtt, NULL, (size_t)0, 0);
	close(sock_mqtt);
	exit(1);
}

// message: cb< broker (Empfangsbearbeitung einer der abonnierten Nachrichten)
void createMsg4cb(unsigned char* msg)
{
	int msgLen = getMessage4cb ((char *)msg, &mqttRxBuf[mqttBufWIdx]);
	if (msgLen) // eine bekannte Msg
	{
		//mqttRxBuf[mqttBufWIdx].can_id = 0x108DFF; // src: 511, dst: 35=HCAN_MULTICAST_CONTROL, proto: 1=SFP, Prio:0=high
		mqttRxBuf[mqttBufWIdx].can_id = 0x108DFD; // src: 509, dst: 35=HCAN_MULTICAST_CONTROL, proto: 1=SFP, Prio:0=high
		//mqttRxBuf[mqttBufWIdx].can_id = 0x108CC1; // src: 193, dst: 35=HCAN_MULTICAST_CONTROL, proto: 1=SFP, Prio:0=high
		//mqttRxBuf[mqttBufWIdx].can_id = 0x108CA6; // src: 166, dst: 35=HCAN_MULTICAST_CONTROL, proto: 1=SFP, Prio:0=high
		mqttRxBuf[mqttBufWIdx].can_id &= CAN_EFF_MASK;
		mqttRxBuf[mqttBufWIdx].can_id |= CAN_EFF_FLAG;

		mqttRxBuf[mqttBufWIdx].data[0] = 5; // HCAN_SRV_HES
		mqttRxBuf[mqttBufWIdx].can_dlc = msgLen; // Anzahl der Daten-Bytes des Frames

		mqttBufWIdx++; // mqttBufferWIdx > RIdx  ==> wir haben was eingelesen
		mqttBufWIdx &= BUFFERSIZE-1;
	}
	else
	{
		TRACE("\nrx MQTT-Msg: %s  ->  verwerfen, da nicht fuer den cb!\n", msg);
	}
}

static int recvMQTTsubcribedAck(void)
{
	int rc = MQTTPacket_read(buf, sizeof(buf), recv_callback);

	if (rc == SUBACK) // wait for suback
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, sizeof(buf));
		if (granted_qos != 0)
		{
			exitMqtt("granted qos != 0\n"); // sub-qos=0  ggf. granted_qos mit ausgeben
		}

		// wir haben ein Subscribe-ACK erhalten:
		TRACE(":) got SUBACK: (QoS=%d)\n", granted_qos);
	}
	else
	{
		TRACE("Error: MQTTPacket_read liefert rc=%d\n", rc);
		exitMqtt("Error: MQTTPacket_read() @ on_broker_connected()\n");
	}

	return rc;
}

// Wenn die Verbindung zum Broker steht: Dann wird abonniert
static void onMQTTBrokerConnected(void)
{
	TRACE("broker_connected -> subscribe (Nachrichten zum CAN-Bus)\n");

	int req_qos = 0;
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = "cb<"; // Nachrichten zum CAN-Bus
	int len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, msgid, 1, &topicString, &req_qos);
	if(0 == write(sock_mqtt, buf, len)) TRACE("Fehler: Write hat 0 Byte geschrieben\n");

	while(SUBACK != recvMQTTsubcribedAck()); // ist blockierend, aber nur @ boker connected!
}

// Client initialisieren
void initMqtt(char *brokerHost_ip)
{
	int port = 1883;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

	sock_mqtt = transport_open(brokerHost_ip, port);
	if(sock_mqtt < 0)
		exitMqtt("sock_mqtt < 0\n");

	TRACE("initMQTT: connect to broker=%s, port=%d\n", brokerHost_ip, port);

	data.clientID.cstring = "cb_client";
	data.keepAliveInterval = 20;
	data.cleansession = 1;
	data.username.cstring = "hcan";
	data.password.cstring = "n_A_c";
	data.MQTTVersion = 4;

	int len = MQTTSerialize_connect(buf, sizeof(buf), &data);
	if(0 == write(sock_mqtt, buf, len)) TRACE("Fehler: Write hat 0 Byte geschrieben\n");

	// wait for connack:
	int rc = MQTTPacket_read(buf, sizeof(buf), recv_callback);
	if (rc == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;
		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, sizeof(buf)) != 1 || connack_rc != 0)
		{
			exitMqtt("Unable to connect to MQTT-broker\n");
		}
	}
	else
	{
		TRACE("MQTTPacket_read liefert rc=%d\n", rc);
		exitMqtt("initMqtt() failure\n");
	}

	onMQTTBrokerConnected(); // Verbindung steht -> Broker-Nachrichten abonnieren
}

// publish: cb -> broker
void recvMqttMsg(void)
{
	int payloadlen_rx = 0; // keine Nachricht

	memset(buf, '\0', sizeof(buf)); // WICHTIG!
	int rc = MQTTPacket_read(buf, sizeof(buf), recv_callback);

	if (rc == PUBLISH) // message?
	{
		unsigned char dup;
		int qos;
		unsigned char retained;
		unsigned short msgid;
		unsigned char* payload_rx;
		MQTTString receivedTopic;
		MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
				&payload_rx, &payloadlen_rx, buf, sizeof(buf));

		if (0 < payloadlen_rx)
		{
			// got message:
			TRACE("cb<--msg form mqtt-broker: %.*s\n", payloadlen_rx, payload_rx);
			createMsg4cb(payload_rx); // mqttBufWIdx++, falls msg 4 cb
		}
	}
}

int publishMqttMsg(char* pubTopic, const unsigned char* payload, const int payloadlen)
{
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = pubTopic; // topic "cb>" (vom CAN-Bus) // "pubtopic"
	int len = MQTTSerialize_publish(buf, sizeof(buf), 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	return write(sock_mqtt, buf, len);
}

void endMqtt(void)
{
	int len = MQTTSerialize_disconnect(buf, sizeof(buf));
	if(0 == write(sock_mqtt, buf, len)) TRACE("Fehler: Write hat 0 Byte geschrieben\n");
	exitMqtt("endMqtt\n");
}
