TARGET = hcan4mqttpc


SRC = $(patsubst %.c, %.c , $(wildcard *.c))
SRC += $(patsubst %.c, %.c , $(wildcard ./MQTTPacket/src/*.c))

CXX = clang

CFLAGS  = -I. -Wall -g -Os
CFLAGS += -DDEBUG

all:
	$(CXX) $(CFLAGS) -o $(TARGET) $(SRC)
	
clean:
	rm -f $(TARGET)
	rm -fR ./scanBuild

install:
	strip $(TARGET)
	sudo install $(TARGET) /usr/sbin
	
run: 
	$(TARGET) -D
