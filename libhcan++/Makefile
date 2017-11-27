# libhcan++
#
TARGET = libhcan++

SRC = $(patsubst %.cc, %.cc , $(wildcard *.cc))
OBJ = $(patsubst %.cc,%.o,$(wildcard *.cc))

CXX = g++ #-m32
#CXXFLAGS = -Wall
#CXXFLAGS += -O0   # als 64-Bit-Prozess fuehrt -O2 zum Segmentation fault
#CXXFLAGS += -g    # Debug aktivieren
CXXFLAGS += -I.. -I. -I../include `pkg-config libxml++-2.6 --cflags`
CXXFLAGS += -Werror
CXXFLAGS += -fPIC

all: $(TARGET).so

$(TARGET).so: $(OBJ)
	$(CXX) -shared -o $@ -fPIC -Wl,-soname,$(TARGET).so $(OBJ)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

allSrc: 
	$(CXX) $(CXXFLAGS) -o $(SRC) -shared -o $(TARGET).so -fPIC -Wl,-soname,$(TARGET).so -Wl,-Map=$(TARGET).map

install:
	/usr/bin/install ./$(TARGET).so /usr/lib
	ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2
	ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2.0

clean:
	rm -f $(TARGET).* *.o 

doc:
	doxygen    
      