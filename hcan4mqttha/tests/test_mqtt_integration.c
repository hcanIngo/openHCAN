/*
 *  Integrationstest: MQTT-Pfad gegen einen echten Broker im Internet
 *
 *  Der HCAN-Seite wird simuliert (kein CAN-Interface noetig):
 *   - Test 1 (MQTT -> CAN):  Der Test publishet  hc/cover/<id>/<--/set_pos  = "42"
 *     an den Broker.  rxFromBroker() (mqttClient.c) empfaengt die eigene
 *     Nachricht und muss daraus die CAN-Nachricht HCAN_HES_ROLLADEN_POSITION_SET
 *     mit Position 42 in mqttRxBuf[] erzeugen.
 *   - Test 2 (CAN -> MQTT):  Eine simulierte CAN-Nachricht (POSITION_REPLAY)
 *     wird ueber catHesTopic4Broker() + publishMqttMsg() an den Broker
 *     gesendet.  Ein zweiter Client (eigene Verbindung) abonniert das
 *     Position-Topic und prueft das ankommende Payload.
 *
 *  Aufruf:  ./test_mqtt_integration [brokerHost[:port]]
 *  Default: broker.hivemq.com:1883
 *  Exit-Code 0 = alle Tests bestanden
 */
#include "../mqttHcan.h"
#include "../mqttClient.h"
#include "../parseXml.h"
#include "../MQTTPacket/src/MQTTPacket.h"

#include <assert.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

// Globals, die normalerweise in main.c definiert sind:
int sock_mqtt = INVALID_SOCKET;
struct can_frame mqttRxBuf[BUFFERSIZE];
uint8_t mqttBufWIdx = 0;
uint8_t mqttBufRIdx = 0;
uint8_t debug = 0;
uint8_t debugWay = 0;
bool HaOnline = true; // Test 2 braucht HA-online
bool HaOnlineStateChanged = false;

// parseXml.c wird nicht gelinkt:  dessen Globals und getXmlDatenZurId() werden hier simuliert:
hcanInstallation dev[anzIds];
char stt[lenStt];
char filenameInstallationXml[lenFilename];

// Stub fuer getXmlDatenZurId():
bool getXmlDatenZurId(unsigned char id)
{
	memset(dev[id].Bereichsname, '\0', sizeof dev[id].Bereichsname);
	snprintf(dev[id].Bereichsname, sizeof dev[id].Bereichsname, "test");
	snprintf(stt, sizeof stt, "test %u", id);
	return true;
}

// tConfigTopic ist file-local in mqttHcan.c; strDateiEintrag[] ist aber global:
typedef struct configTopicTest
{
	char TildeOhneId[35];
	char stat_t[30];
	char cmd_t[20];
	char pos_t[30];
	char set_pos_t[30];
} tConfigTopicTest;
extern tConfigTopicTest strDateiEintrag[6];

typedef enum eDevTest { eCoverT = 5 } teDevTest;

// Test-Helfer:
static int testsGelaufen = 0;
static int testsFehlgeschlagen = 0;

#define CHECK(cond) do { \
	testsGelaufen++; \
	if (!(cond)) { \
		testsFehlgeschlagen++; \
		fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
	} \
} while (0)

#define CHECK_STR(got, want) do { \
	testsGelaufen++; \
	if (0 != strcmp((got), (want))) { \
		testsFehlgeschlagen++; \
		fprintf(stderr, "FAIL %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, (got), (want)); \
	} \
} while (0)

// cover-Eintraege wie in cover_config.json setzen:
static void setCoverConfig(void)
{
	tConfigTopicTest * cf = &strDateiEintrag[eCoverT];
	memset(cf, '\0', sizeof *cf);
	snprintf(cf->TildeOhneId, sizeof cf->TildeOhneId, "hc/cover");
	snprintf(cf->stat_t,     sizeof cf->stat_t,     "/-->/state");
	snprintf(cf->cmd_t,      sizeof cf->cmd_t,      "/<--/set");
	snprintf(cf->pos_t,      sizeof cf->pos_t,      "/-->/position");
	snprintf(cf->set_pos_t,  sizeof cf->set_pos_t,  "/<--/set_pos");
}

// ---------- zweiter MQTT-Client (eigene Verbindung) fuer Test 2 ----------
static int sock_mqtt2 = -1;
static unsigned char buf2[4000];
static int buflen2 = sizeof buf2;
static char g_posTopic[64];
static int g_qos0 = 0;
static MQTTTransport mytransport2;

// non-blocking, wie transport_getdatanb() in mqttClient.c:
static int transport_getdatanb2(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck);
	int rc = recv(sock, buf, count, 0);
	if (rc == -1)
		return 0;
	return rc;
}

// blockierend genau ein MQTT-Paket lesen (Header + Remaining-Length + Rest)
static int wait4Packet(int sock, unsigned char * buf, int buflen)
{
	// Header-Byte
	int rc = recv(sock, buf, 1, 0);
	if (rc != 1) return -1;

	// Remaining-Length (varint)
	int rem_len = 0, multiplier = 1;
	unsigned char c;
	do
	{
		rc = recv(sock, &c, 1, 0);
		if (rc != 1) return -1;
		buf[1] = c; // für die byte-dump-Debug-Ausgabe (nur erstes varint-Byte)
		rem_len += (c & 127) * multiplier;
		multiplier *= 128;
	} while ((c & 128) != 0);

	// Payload
	int n = 2; // Header + 1 Remaining-Length-Byte (für CONNACK/SUBACK)
	while (n < 1 + rem_len)
	{
		rc = recv(sock, &buf[n], 1, 0);
		if (rc <= 0) return -1;
		n += rc;
	}
	MQTTHeader header;
	header.byte = buf[0];
	return header.bits.type;
}

// Verbindung aufbauen und subscribe-Paket senden; liefert 0 bei Erfolg.
// Der oeffentliche Broker (broker.hivemq.com) begrenzt die Anzahl
// gleichzeitiger Verbindungen pro IP - bei Timeout wird die Verbindung
// daher bis zu 3x aufgebaut (einzigartige clientID pro Versuch).
static int client2_connect(const char * host, int port)
{
	for (int attempt = 0; attempt < 3; attempt++)
	{
		struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};
		struct addrinfo *result = NULL;
		if (getaddrinfo(host, NULL, &hints, &result) != 0) return -1;
		struct addrinfo *res = result;
		while (res && res->ai_family != AF_INET) res = res->ai_next;
		if (!res) { freeaddrinfo(result); return -1; }

		sock_mqtt2 = socket(AF_INET, SOCK_STREAM, 0);
		if (sock_mqtt2 < 0) { freeaddrinfo(result); return -1; }

		// eigener Socket, damit die addrinfo-Struktur nach dem connect wiederverwendet werden kann
		struct sockaddr_in address;
		address.sin_port = htons(port);
		address.sin_family = AF_INET;
		address.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
		if (connect(sock_mqtt2, (struct sockaddr *)&address, sizeof address) < 0)
		{
			close(sock_mqtt2);
			freeaddrinfo(result);
			return -1;
		}
		freeaddrinfo(result);

		// 5s Timeout, damit ein nicht antwortender Broker nicht haengt
		struct timeval tv = {5, 0};
		setsockopt(sock_mqtt2, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);

		// CONNECT (einzigartige clientID, user/pass wie in initMqtt())
		MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
		char clientID[32];
		snprintf(clientID, sizeof clientID, "hcan_test_%d_%d", getpid(), attempt);
		data.clientID.cstring = clientID;
		data.username.cstring = "hcan";
		data.password.cstring = "n_A_c";
		data.keepAliveInterval = 40;
		data.cleansession = 1;
		data.MQTTVersion = 4;
		int len = MQTTSerialize_connect(buf2, buflen2, &data);
		if (write(sock_mqtt2, buf2, len) != len)
		{
			close(sock_mqtt2); sock_mqtt2 = -1;
			continue;
		}

		// CONNACK erwarten (4 Byte: Header + Remaining-Length 1 + 2 Payload-Bytes)
		unsigned char connack[4] = {0, 0, 0, 0};
		if (wait4Packet(sock_mqtt2, connack, 4) != CONNACK)
		{
			close(sock_mqtt2); sock_mqtt2 = -1;
			continue;
		}
		unsigned char sessionPresent, connack_rc;
		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, connack, 4) != 1 || connack_rc != 0)
		{
			close(sock_mqtt2); sock_mqtt2 = -1;
			continue;
		}

		// SUBSCRIBE: 1 Topic, QoS 0
		MQTTString topic = MQTTString_initializer;
		topic.cstring = (char *)g_posTopic;
		len = MQTTSerialize_subscribe(buf2, buflen2, 0, 1, 1, &topic, &g_qos0);
		if (write(sock_mqtt2, buf2, len) != len)
		{
			close(sock_mqtt2); sock_mqtt2 = -1;
			continue;
		}

		// SUBACK erwarten (4 Byte: Header + Remaining-Length 2 + msgid)
		// Der Broker kann zwischendurch PUBLISH-Nachrichten senden (z.B. von
		// Test 1, falls das Topic ueberschneidet) - diese abwarten und verwerfen.
		unsigned char suback[4] = {0, 0, 0, 0};
		int suback_rc = -1;
		for (int i = 0; i < 10; i++)
		{
			suback_rc = wait4Packet(sock_mqtt2, suback, 4);
			if (suback_rc == SUBACK) break;
			if (suback_rc < 0) break;
		}
		if (suback_rc != SUBACK)
		{
			close(sock_mqtt2); sock_mqtt2 = -1;
			continue;
		}

		// non-blocking Transport fuer den Empfang einrichten (wie mytransport in mqttClient.c)
		mytransport2.sck = &sock_mqtt2;
		mytransport2.getfn = transport_getdatanb2;
		mytransport2.state = 0;
		mytransport2.len = 0;
		mytransport2.multiplier = 1;
		mytransport2.rem_len = 0;
		return 0;
	}
	return -1;
}

int main(int argc, char ** argv)
{
	char host[128] = "broker.hivemq.com";
	int port = 1883;
	if (argc > 1)
	{
		char * colon = strchr(argv[1], ':');
		if (colon)
		{
			*colon = '\0';
			port = atoi(colon + 1);
		}
		snprintf(host, sizeof host, "%s", argv[1]);
	}

	initMqttHcan();
	setCoverConfig();

	unsigned char testId = (unsigned char)(getpid() % 200 + 1); // 1..200, vermeidet Kollisionen
	snprintf(g_posTopic, sizeof g_posTopic, "hc/cover/%u/-->/position", testId);
	printf("Broker=%s:%d  TestId=%u\n", host, port, testId);

	// ---------- Test 1:  MQTT -> CAN  (set_pos) ----------
	initMqtt(host); // Verbindung + subscribe "hc/+/+/<--/+" und "homeassistant/status"
	{
		char topic[64];
		snprintf(topic, sizeof topic, "hc/cover/%u/<--/set_pos", testId);
		const char * payload = "42";
		int written = publishMqttMsg(topic, (const unsigned char *)payload, (int)strlen(payload));
		CHECK(written > 0);

		// eigene Nachricht vom Broker empfangen und CAN-Nachricht pruefen
		int gotFrame = 0;
		for (int i = 0; i < 100 && !gotFrame; i++)
		{
			if (PUBLISH == rxFromBroker())
			{
				if (mqttBufWIdx != mqttBufRIdx)
				{
					const struct can_frame * cf = &mqttRxBuf[mqttBufRIdx];
					CHECK(HCAN_HES_ROLLADEN_POSITION_SET == cf->data[1]);
					CHECK(testId == cf->data[2]);
					CHECK(42 == cf->data[3]);
					CHECK(4 == cf->can_dlc);
					CHECK(0x108DFD == (cf->can_id & 0x1FFFFFFFu));
					gotFrame = 1;
				}
			}
			usleep(100 * 1000);
		}
		CHECK(gotFrame);
	}

	// ---------- Test 2:  CAN -> MQTT  (pos_t) ----------
	{
		// zweiter Client (eigene Broker-Verbindung) - bei Verweis nur warnen,
		// nicht abbrechen:  der lokale Teil (catHesTopic4Broker) laeuft trotzdem
		int c2rc = client2_connect(host, port);
		CHECK(0 == c2rc);
		if (c2rc != 0)
			fprintf(stderr, "client2: CONNECT-NG (Broker begrenzt evtl. die Verbindungen pro IP)\n");
		int client2ok = (sock_mqtt2 >= 0);

		// simulierte CAN-Nachricht:  Position 42
		struct can_frame cf;
		memset(&cf, 0, sizeof cf);
		cf.data[1] = HCAN_HES_ROLLADEN_POSITION_REPLAY;
		cf.data[2] = testId;
		cf.data[3] = 42;

		char strTopic[dStrLen];
		char strPayload[dStrLen];
		size_t len = catHesTopic4Broker(strTopic, strPayload, &cf);
		CHECK(len > 0);
		CHECK_STR(strTopic, g_posTopic);
		CHECK_STR(strPayload, "{\"position\": 42}");

		// erst nach dem Subscribe publishen, damit client2 die Nachricht bekommt
		int written = publishMqttMsg(strTopic, (const unsigned char *)strPayload, (int)len);
		CHECK(written > 0);

		// zweiter Client empfaengt die Nachricht (non-blocking, wie rxFromBroker())
		int gotMsg = 0;
		if (client2ok)
		{
			unsigned char * payload_rx = NULL;
			int payloadlen_rx = 0;
			for (int i = 0; i < 100 && !gotMsg; i++)
			{
				memset(buf2, '\0', buflen2);
				MQTTString receivedTopic;
				unsigned char dup, qos, retained;
				unsigned short msgid;
				int rc = MQTTPacket_readnb(buf2, buflen2, &mytransport2);
				if (rc == PUBLISH)
				{
					int rcd = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, &payload_rx, &payloadlen_rx, buf2, buflen2);
					if (1 == rcd && 0 < payloadlen_rx)
					{
						CHECK_STR(receivedTopic.lenstring.data, g_posTopic);
						CHECK_STR((char *)payload_rx, "{\"position\": 42}");
						gotMsg = 1;
					}
				}
				usleep(100 * 1000);
			}
			CHECK(gotMsg);
			close(sock_mqtt2);
			sock_mqtt2 = -1;
		}
		else
		{
			printf("  [SKIP] Test 2: zweiter Broker-Client nicht erreichbar\n");
		}
	}

	if (testsFehlgeschlagen)
	{
		fprintf(stderr, "%d von %d Tests FEHLGESCHLAGEN\n", testsFehlgeschlagen, testsGelaufen);
		return 1;
	}
	printf("OK: alle %d Tests bestanden\n", testsGelaufen);
	return 0;
}
