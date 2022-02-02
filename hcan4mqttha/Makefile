TARGET = hcan4mqttha


SRC = $(patsubst %.c, %.c , $(wildcard *.c))
SRC += $(patsubst %.c, %.c , $(wildcard ./MQTTPacket/src/*.c))

CXX = gcc
#CXX = clang # scan-build -o ./scanBuild make all -j4     ## zur statischen Analyse

# Vorrausetzung:  sudo apt-get install libxml2 libjson-c-dev

# CFLAGS =  -fsanitize=address -ggdb # Speicherzugriffsfehler ermitteln  und  gnu-Debug-Infos  
CFLAGS = -I. -Wall -g -Os

CFLAGS += -I/usr/local/include/  # fuer json-c
CFLAGS += -L/usr/local/lib/      # fuer json-c
# alternativ: CFLAGS += $(shell pkg-config --cflags json-c) $(shell pkg-config --libs json-c)

CFLAGS += -I/usr/include/libxml2 # fuer libxml2
CFLAGS += -L/usr/include/        # fuer libxml2
# alternativ: CFLAGS += $(shell xml2-config --cflags) $(shell xml2-config --libs)

# CFLAGS += -DDEBUG # ermoeglicht das Aktivieren der Debug-Ausgaben (per hcan4mqttha -D oder -T) 


# LDFLAGS = -lasan # Speicherzugriffsfehler ermitteln 
LDFLAGS = -ljson-c -lxml2

all:
	$(CXX) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)
	
clean:
	rm -f $(TARGET)
	rm -fR ./scanBuild

install:
	strip $(TARGET)
	sudo install $(TARGET) /usr/bin
	sudo cp ./*_config.json /etc/hcan
	
run: 
	$(TARGET) -D
