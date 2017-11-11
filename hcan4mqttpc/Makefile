TARGET = hcan4mqttpc

SRC  = main.c mqttClient.c mqttHcan.c
SRC += ./MQTTPacket/src/MQTTConnectClient.c 
SRC += ./MQTTPacket/src/MQTTSerializePublish.c
SRC += ./MQTTPacket/src/MQTTPacket.c
SRC += ./MQTTPacket/src/MQTTSubscribeClient.c
SRC += ./MQTTPacket/src/MQTTDeserializePublish.c

CXX = clang

CFLAGS  = -I. -Wall -g -Os
#CFLAGS += -DDEBUG
#CFLAGS += -I ./MQTTPacket/src

all:
	$(CXX) $(CFLAGS) -o $(TARGET) $(SRC)
	
clean:
	rm -f $(TARGET)
	test -d ./scanBuild || sudo rm -R ./scanBuild

install:
	strip $(TARGET)
	sudo install $(TARGET) /usr/sbin
	
run: 
	$(TARGET) -D
