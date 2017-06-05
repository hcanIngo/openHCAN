TARGET = hcan4mqttpc
OBJ  = main.o mqttClient.o mqttHcan.o
OBJ += 	./MQTTPacket/src/MQTTConnectClient.o ./MQTTPacket/src/MQTTSerializePublish.o \
		./MQTTPacket/src/MQTTPacket.o ./MQTTPacket/src/MQTTSubscribeClient.o \
		./MQTTPacket/src/MQTTDeserializePublish.o
		 

CXX = gcc
LDFLAGS = 

CFLAGS  = -I. -Wall -g -Os
CFLAGS += -DDEBUG
CFLAGS += -I ./MQTTPacket/src

all: $(CONDITIONAL_CLEAN) $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ)
	strip $@

$(OBJ): %.o:%.c
	$(CXX) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f *.o $(TARGET)
	cd ./MQTTPacket/src; rm -f *.o

ctags:
	ctags *.c *.h

install:
	sudo install $(TARGET) /usr/sbin
	
run: 
	./$(TARGET) -D
