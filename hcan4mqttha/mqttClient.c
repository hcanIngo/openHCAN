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

#include "../hcan4mqttha/mqttClient.h"

#include <linux/can.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../hcan4mqttha/mqttHcan.h"
#include "../hcan4mqttha/MQTTPacket/src/MQTTPacket.h"
#include "../include/hcan-def.h"

/* Dieser MQTT-Client baut eine Verbindung zum Broker auf (z.B. localhost:1883):
 - abonniert (subscribes) das Thema (topic) "hc/<--". Daher Nachrichten an den CAN-Bus.
 - veroeffentlicht (publishes) bestimmte HES-Nachrichten unter dem Thema (topic) "homeassistant". Daher Nachrichten vom CAN-Bus zu homeassistant-System.
*/
unsigned char buf[dStrLen]; // fuer das Serialisieren / Deserialisieren
int buflen;
MQTTTransport mytransport;

// MQTTPacket_read erwartet die folgenden Callback-Signatur:
int transport_getdata(unsigned char* buf, int count)
{
	int rc = recv(sock_mqtt, buf, count, 0);
	//printf("received %d bytes count %d\n", rc, (int)count);
	return rc;
}

static int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock_mqtt = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc = recv(sock_mqtt, buf, count, 0);
	if (rc == -1)
		return 0; // check error conditions from your system here, and return -1

	return rc;
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

		if (NULL != result)
		{
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
		}

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
	TRACE("%s", msg);
	shutdown(sock_mqtt, SHUT_WR);
	recv(sock_mqtt, NULL, (size_t)0, 0);
	close(sock_mqtt);
	exit(1);
}

static void completeMsg4cb (int msgLen, struct can_frame * msg4cb)
{
	//mqttRxBuf[mqttBufWIdx].can_id = 0x108DFF; // src: 511, dst: 35=HCAN_MULTICAST_CONTROL, proto: 1=SFP, Prio:0=high
	msg4cb->can_id = 0x108DFD; // src: 509, dst: 35=HCAN_MULTICAST_CONTROL, proto: 1=SFP, Prio:0=high
	//msg4cb->can_id = 0x108CC1; // src: 193, dst: 35=HCAN_MULTICAST_CONTROL, proto: 1=SFP, Prio:0=high
	//msg4cb->can_id = 0x108CA6; // src: 166, dst: 35=HCAN_MULTICAST_CONTROL, proto: 1=SFP, Prio:0=high
	msg4cb->can_id &= CAN_EFF_MASK;
	msg4cb->can_id |= CAN_EFF_FLAG;

	msg4cb->data[0] = 5; // HCAN_SRV_HES
	msg4cb->can_dlc = msgLen; // Anzahl der Daten-Bytes des Frames

	mqttBufWIdx++; // mqttBufferWIdx > RIdx  ==> wir haben was eingelesen
	mqttBufWIdx &= BUFFERSIZE-1;
}

// Hinweis: return -1 ist bereits belegt
static int prepareReqMsg4cb (char * msg, struct can_frame * msg4cb)
{
	uint8_t rtnCanMsgLen = 0; // kein Topic (keine Message) fuer den CAN-Bus
	char* token = strtok(msg, "/"); if(NULL == token) return 0;

	TRACE("prepareReqMsg4cb token=%s\n", token);

	if (is(token, "RQC")) // Request Devices-Configs
	{
		msg4cb->data[1] = HCAN_HES_DEVICES_CONFIGS_REQUEST;
		rtnCanMsgLen = 2;
	}
	else if (is(token, "RQS")) // Request Device-States
	{
		msg4cb->data[1] = HCAN_HES_DEVICE_STATES_REQUEST;
		rtnCanMsgLen = 2;
	}
	else if (is(token, "RQH")) // Heizungs-Details Request
	{
		token = strtok(NULL, "/"); if(NULL == token) return -19;
		msg4cb->data[2] = atoi(token); // Gruppe

		msg4cb->data[1] = HCAN_HES_HEIZUNG_DETAILS_REQUEST; // --> Antwort vom CAN-Bus: HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS
		rtnCanMsgLen = 3; // Anzahl der Daten-Bytes des Frames
	}
	// else: s.o.

	return rtnCanMsgLen;
}

// message: cb< broker (Empfangsbearbeitung einer der abonnierten Nachrichten)
void createReqMsg4cb(char* msg)
{
	int msgLen = prepareReqMsg4cb (msg, &mqttRxBuf[mqttBufWIdx]);
	if (msgLen > 0) // eine bekannte Msg
	{
		completeMsg4cb (msgLen, &mqttRxBuf[mqttBufWIdx]);
	}
	else // msgLen enthaelt Error-Code:
	{
		TRACE("\nrx MQTT-Msg: Msg=%s  ->  prepareReqMsg4cb()-NG=%d\n", msg, msgLen); // rtn=msgLen hier 0
	}
}

// message: cb< broker (Empfangsbearbeitung einer der abonnierten Nachrichten)
static int createMsg4cb(char* topic_rx, char* payload_rx)
{
	int msgLen = prepareMsg4cb (topic_rx, payload_rx, &mqttRxBuf[mqttBufWIdx]);
	if (msgLen > 0) // eine bekannte Msg
	{
		completeMsg4cb (msgLen, &mqttRxBuf[mqttBufWIdx]);
	}
	else // msgLen enthaelt Error-Code:
	{
		TRACE("\ncb <--Broker  Topic=%s Payload=%s  ->  prepareMsg4cb()-NG=%d\n", topic_rx, payload_rx, msgLen); // rtn=msgLen hier 0
	}

	return msgLen;
}

static int recvMQTTsubcribedAck(void)
{
	int rc = MQTTPacket_readnb(buf, buflen, &mytransport);
	if (rc == SUBACK) // wait for suback
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos != 0)
		{
			TRACE("Error: granted qos %d != 0\n", granted_qos); // Subscriber verlangt sub-qos=0
			exitMqtt("Error: Subscriber verlangt sub-qos=0\n");
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
static void onMQTTBrokerConnected(MQTTString topicFilters[])
{
	int req_qos = 0;
	int msgid = 1;
	int len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, topicFilters, &req_qos);
	if(0 == write(sock_mqtt, buf, len)) TRACE("Fehler: Write hat 0 Byte geschrieben\n");

	TRACE("broker_connected -> subscribe-Topic=%s (Nachrichten zum CAN-Bus)\n", topicFilters->cstring);
	while (SUBACK != recvMQTTsubcribedAck()); // ist blockierend, aber nur @ boker connected!
}

// Client initialisieren
void initMqtt(char *brokerHost_ip)
{
	buflen = sizeof(buf);
	int port = 1883;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

	sock_mqtt = transport_open(brokerHost_ip, port);
	if(sock_mqtt < 0)
		exitMqtt("sock_mqtt < 0\n");

	TRACE("initMQTT: connect to broker=%s, port=%d\n", brokerHost_ip, port);

	mytransport.sck = &sock_mqtt;
	mytransport.getfn = transport_getdatanb;
	mytransport.state = 0;
	data.clientID.cstring = "ha_client";
	data.keepAliveInterval = 40;
	data.cleansession = 1;
	data.username.cstring = "hcan";
	data.password.cstring = "n_A_c";
	data.MQTTVersion = 4;

	int len = MQTTSerialize_connect(buf, buflen, &data);
	if(0 == write(sock_mqtt, buf, len)) TRACE("Fehler: Write hat 0 Byte geschrieben\n");

	// wait for connack:
	int rc = MQTTPacket_read(buf, buflen, transport_getdata);
	if (rc == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;
		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
			TRACE("connack_rc=%d @ MQTTDeserialize_connack\n", connack_rc);
			exitMqtt("Unable to connect to MQTT-broker\n");
		}
	}
	else
	{
		TRACE("MQTTPacket_read liefert rc=%d\n", rc);
		exitMqtt("initMqtt() failure\n");
	}


	MQTTString topic1 = MQTTString_initializer;
	//topic1.cstring = "hc/<--/#";
	//topic1.cstring = "hc/<--/light/1/set"; // Dennoch kein Topic-Name @ MQTTDeserialize_publish
	//topic1.cstring = "hc/<--/+/+/set"; // z. B. UI-Schalter bewegt -> Nachricht zum CAN-Bus (hc/<--)
	//topic1.cstring = "hc/<--/+/+/+"; // z. B. UI-Schalter bewegt -> Nachricht zum CAN-Bus (hc/<--)
	topic1.cstring = "hc/+/+/<--/+"; // z. B. UI-Schalter bewegt -> Nachricht zum CAN-Bus (hc)
	onMQTTBrokerConnected(&topic1); // Verbindung steht -> Broker-Nachrichten abonnieren

/*  Alternative topicFilters[]:
	MQTTString topic2 = MQTTString_initializer;
	topic2.cstring = "hc/<--/+/+/state"; // Nachrichten zum CAN-Bus (hc/<--)
	onMQTTBrokerConnected(&topic2); // Verbindung steht -> Broker-Nachrichten abonnieren
*/
}

// write to mqttBuf
int rxFromBroker(void)
{
	int payloadlen_rx = 0; // keine Nachricht
	memset(buf, '\0', buflen); // WICHTIG!
	int rc = MQTTPacket_readnb(buf, buflen, &mytransport);
	if (rc == PUBLISH) // message?
	{
		unsigned char dup;
		int qos;
		unsigned char retained;
		unsigned short msgid;
		unsigned char* payload_rx;

		MQTTString receivedTopic;
		int rcd = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, &payload_rx, &payloadlen_rx, buf, buflen);
		// Mehrere subscriptions, dann:    "we have to find the right message handler - indexed by topic"
		if ((1 == rcd) && (0 < payloadlen_rx))
		{
			// got message:
			TRACE("cb <--Broker  Topic=%.*s Payload=%.*s (Qos=%d)\n", receivedTopic.lenstring.len, receivedTopic.lenstring.data, payloadlen_rx, payload_rx, qos);

			int rtn = createMsg4cb(receivedTopic.lenstring.data/*receivedTopic.cstring*/, (char*)payload_rx); // mqttBufWIdx++, falls msg 4 cb
			if (rtn > 0)
			{
				// alles ok
				if (debug > 1) TRACE("Broker <--cb  %u Gr=%u\n", mqttRxBuf[mqttBufWIdx].data[1], mqttRxBuf[mqttBufWIdx].data[2]);
			}
			else
			{
				TRACE("\ncreateMsg4cb()-NG liefert %d\n", rtn); // rtn=msgLen hier 0 oder negativ
				rc = rtn; // createMsg4cb abgebrochen
			}
		}
		else TRACE("mqttPUBLISH-Err: payloadlen_rx=%d, rcd=%d\n", payloadlen_rx, rcd);
	}
	else
		TRACE("mqttRead: rc=%d\n", rc); // z.B. DISCONNECT

	return rc;
}

int publishMqttMsg(char* pubTopic, const unsigned char* payload, const int payloadlen)
{
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = pubTopic;
	int len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	return write(sock_mqtt, buf, len);
}

void endMqtt(void)
{
	int len = MQTTSerialize_disconnect(buf, buflen);
	if(0 == write(sock_mqtt, buf, len)) TRACE("Fehler: Write hat 0 Byte geschrieben\n");
	exitMqtt("endMqtt\n");
}
