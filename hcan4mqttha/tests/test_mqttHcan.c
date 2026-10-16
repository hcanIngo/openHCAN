/*
 *  Unit-Tests zu mqttHcan.c  (HCAN-Seite simuliert, kein Netzwerk)
 *
 *  Getestet werden die Cover-Anpassungen:
 *   - prepareMsg4cb():  set_pos_t  (Position 0..100 setzen)  +  Regression cmd_t/stat_t
 *   - catHesTopic4Broker():  pos_t  (Position-Topic/Payload)  +  Regression state-Payload
 *
 *  Die HCAN-Seite wird simuliert:
 *   - getXmlDatenZurId() wird durch einen Stub ersetzt (parseXml.c wird nicht gelinkt)
 *   - strDateiEintrag[] wird direkt befuellt (statt cover_config.json zu lesen)
 *
 *  Aufruf:  ./test_mqttHcan
 *  Exit-Code 0 = alle Tests bestanden
 */
#include "../mqttHcan.h"
#include "../mqttClient.h"
#include "../parseXml.h"

#include <assert.h>

// Globals, die normalerweise in main.c definiert sind:
int sock_mqtt = INVALID_SOCKET;
struct can_frame mqttRxBuf[BUFFERSIZE];
uint8_t mqttBufWIdx = 0;
uint8_t mqttBufRIdx = 0;
uint8_t debug = 0;
uint8_t debugWay = 0;
bool HaOnline = false;
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

// tConfigTopic ist file-local in mqttHcan.c; strDateiEintrag[] ist aber global.
// Layout-identische Struktur, um die Eintraege direkt zu befuellen:
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

int main(void)
{
	initMqttHcan();
	setCoverConfig();

	// ---------- prepareMsg4cb(): cover set_pos (neue Funktion) ----------
	{
		struct can_frame msg4cb;
		memset(&msg4cb, 0, sizeof msg4cb);

		size_t len = prepareMsg4cb("hc/cover/5/<--/set_pos", "42", &msg4cb);
		CHECK(4 == len);
		CHECK(HCAN_HES_ROLLADEN_POSITION_SET == msg4cb.data[1]);
		CHECK(5 == msg4cb.data[2]);
		CHECK(42 == msg4cb.data[3]);
	}
	{
		struct can_frame msg4cb;
		memset(&msg4cb, 0, sizeof msg4cb);

		size_t len = prepareMsg4cb("hc/cover/5/<--/set_pos", "0", &msg4cb);
		CHECK(4 == len);
		CHECK(0 == msg4cb.data[3]);
	}
	{
		struct can_frame msg4cb;
		memset(&msg4cb, 0, sizeof msg4cb);

		// Hinweis:  data[3] enthaelt 0..100 (kein Invertieren, s.o.); 100 bleibt 100
		size_t len = prepareMsg4cb("hc/cover/5/<--/set_pos", "100", &msg4cb);
		CHECK(4 == len);
		CHECK(100 == msg4cb.data[3]);
	}
	{
		struct can_frame msg4cb;
		memset(&msg4cb, 0, sizeof msg4cb);

		// Regression: cmd_t OPEN/CLOSE/STOP bleibt unveraendert
		size_t len = prepareMsg4cb("hc/cover/5/<--/set", "OPEN", &msg4cb);
		CHECK(4 == len);
		CHECK(HCAN_HES_ROLLADEN_POSITION_SET == msg4cb.data[1]);
		CHECK(201 == msg4cb.data[3]);
	}
	{
		struct can_frame msg4cb;
		memset(&msg4cb, 0, sizeof msg4cb);

		size_t len = prepareMsg4cb("hc/cover/5/<--/set", "CLOSE", &msg4cb);
		CHECK(4 == len);
		CHECK(200 == msg4cb.data[3]);
	}
	{
		struct can_frame msg4cb;
		memset(&msg4cb, 0, sizeof msg4cb);

		size_t len = prepareMsg4cb("hc/cover/5/<--/set", "STOP", &msg4cb);
		CHECK(4 == len);
		CHECK(202 == msg4cb.data[3]);
	}
	{
		struct can_frame msg4cb;
		memset(&msg4cb, 0, sizeof msg4cb);

		// Regression: stat_t -> Position-Request
		size_t len = prepareMsg4cb("hc/cover/5/-->/state", "", &msg4cb);
		CHECK(3 == len);
		CHECK(HCAN_HES_ROLLADEN_POSITION_REQUEST == msg4cb.data[1]);
		CHECK(5 == msg4cb.data[2]);
	}
	{
		struct can_frame msg4cb;
		memset(&msg4cb, 0, sizeof msg4cb);

		// unbekanntes Topic -> keine CAN-Nachricht
		size_t len = prepareMsg4cb("hc/cover/5/<--/unbekannt", "42", &msg4cb);
		CHECK(0 == len);
	}

	// ---------- catHesTopic4Broker(): cover pos_t (neue Funktion) ----------
	HaOnline = true;
	{
		char strTopic[dStrLen];
		char strPayload[dStrLen];
		struct can_frame cf;
		memset(&cf, 0, sizeof cf);
		cf.data[1] = HCAN_HES_ROLLADEN_POSITION_REPLAY;
		cf.data[2] = 5;
		cf.data[3] = 42;

		size_t len = catHesTopic4Broker(strTopic, strPayload, &cf);
		CHECK(16 == len); // "{\"position\": 42}"
		CHECK_STR(strTopic, "hc/cover/5/-->/position");
		CHECK_STR(strPayload, "{\"position\": 42}");
	}
	{
		char strTopic[dStrLen];
		char strPayload[dStrLen];
		struct can_frame cf;
		memset(&cf, 0, sizeof cf);
		cf.data[1] = HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO;
		cf.data[2] = 5;
		cf.data[3] = 100;

		size_t len = catHesTopic4Broker(strTopic, strPayload, &cf);
		CHECK_STR(strTopic, "hc/cover/5/-->/position");
		CHECK_STR(strPayload, "{\"position\": 100}");
	}
	{
		// Regression: pos_t NICHT konfiguriert -> altes state-Verhalten
		tConfigTopicTest * cf = &strDateiEintrag[eCoverT];
		memset(cf->pos_t, '\0', sizeof cf->pos_t);

		char strTopic[dStrLen];
		char strPayload[dStrLen];
		struct can_frame can;
		memset(&can, 0, sizeof can);
		can.data[1] = HCAN_HES_ROLLADEN_POSITION_REPLAY;
		can.data[2] = 5;

		can.data[3] = 200;
		catHesTopic4Broker(strTopic, strPayload, &can);
		CHECK_STR(strTopic, "hc/cover/5/-->/state");
		CHECK_STR(strPayload, "closing");

		can.data[3] = 201;
		catHesTopic4Broker(strTopic, strPayload, &can);
		CHECK_STR(strPayload, "opening");

		can.data[3] = 202;
		catHesTopic4Broker(strTopic, strPayload, &can);
		CHECK_STR(strPayload, "stopped");

		can.data[3] = 100;
		catHesTopic4Broker(strTopic, strPayload, &can);
		CHECK_STR(strPayload, "open");

		can.data[3] = 0;
		catHesTopic4Broker(strTopic, strPayload, &can);
		CHECK_STR(strPayload, "closed");

		// pos_t wiederherstellen
		snprintf(cf->pos_t, sizeof cf->pos_t, "/-->/position");
	}
	{
		// HA offline -> nichts an HA senden
		HaOnline = false;
		char strTopic[dStrLen];
		char strPayload[dStrLen];
		struct can_frame cf;
		memset(&cf, 0, sizeof cf);
		cf.data[1] = HCAN_HES_ROLLADEN_POSITION_REPLAY;
		cf.data[2] = 5;
		cf.data[3] = 42;

		size_t len = catHesTopic4Broker(strTopic, strPayload, &cf);
		CHECK(0 == len);
		HaOnline = true;
	}

	// ---------- msgOfInterest4broker(): Position-Nachrichten sind fuer den Broker interessant ----------
	{
		struct can_frame cf;
		memset(&cf, 0, sizeof cf);
		cf.can_id = 0x108DFD; // proto: 1=SFP
		cf.can_dlc = 4;
		cf.data[0] = 5; // HES
		cf.data[1] = HCAN_HES_ROLLADEN_POSITION_REPLAY;
		CHECK(1 == msgOfInterest4broker(&cf));

		cf.data[1] = HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO;
		CHECK(1 == msgOfInterest4broker(&cf));

		cf.data[1] = HCAN_HES_ROLLADEN_POSITION_SET; // keine Broker-Nachricht
		CHECK(0 == msgOfInterest4broker(&cf));
	}

	if (testsFehlgeschlagen)
	{
		fprintf(stderr, "%d von %d Tests FEHLGESCHLAGEN\n", testsFehlgeschlagen, testsGelaufen);
		return 1;
	}
	printf("OK: alle %d Tests bestanden\n", testsGelaufen);
	return 0;
}
