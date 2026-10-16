# Unit- und Integrationstests zu hcan4mqttha
#
#   make test             -> Unit-Tests (kein Netzwerk, HCAN-Seite simuliert)
#   make test-integration -> Integrationstest gegen echten MQTT-Broker
#                            (Default: broker.hivemq.com:1883, uebergabbar:
#                             make test-integration BHOST=meinbroker:1883)
#   make clean

CXX = gcc
CFLAGS = -I.. -Wall -g -O0 -DDEBUG
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS = $(shell pkg-config --libs json-c)

BHOST ?= broker.hivemq.com:1883

MQTTPACKET_SRC = $(wildcard ../MQTTPacket/src/*.c)

test: test_mqttHcan
	./test_mqttHcan

test_mqttHcan: test_mqttHcan.c ../mqttHcan.c
	$(CXX) $(CFLAGS) -o $@ test_mqttHcan.c ../mqttHcan.c $(LDFLAGS)

test-integration: test_mqtt_integration
	./test_mqtt_integration $(BHOST)

test_mqtt_integration: test_mqtt_integration.c ../mqttHcan.c ../mqttClient.c $(MQTTPACKET_SRC)
	$(CXX) $(CFLAGS) -o $@ test_mqtt_integration.c ../mqttHcan.c ../mqttClient.c $(MQTTPACKET_SRC) $(LDFLAGS)

clean:
	rm -f test_mqttHcan test_mqtt_integration
