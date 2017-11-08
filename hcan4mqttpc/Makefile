TARGET = hcan4mqttpc
OBJ  = main.o mqttClient.o mqttHcan.o
OBJ += ./MQTTPacket/src/MQTTConnectClient.o 
OBJ += ./MQTTPacket/src/MQTTSerializePublish.o
OBJ += ./MQTTPacket/src/MQTTPacket.o
OBJ += ./MQTTPacket/src/MQTTSubscribeClient.o
OBJ += ./MQTTPacket/src/MQTTDeserializePublish.o

CXX = clang

CFLAGS  = -I. -Wall -g -Os
CFLAGS += -DDEBUG
CFLAGS += -I ./MQTTPacket/src

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ)

$(OBJ): %.o:%.c
	$(CXX) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f *.o $(TARGET)
	cd ./MQTTPacket/src; rm -f *.o

ctags:
	ctags *.c *.h

install:
	strip ./$(TARGET)
	sudo install $(TARGET) /usr/sbin

analyse: # clang Static Analyzer -> Report
	# scan-build ./configure” (für autoconf-Projekte)
	scan-build make
	
run: 
	./$(TARGET) -D
