# telican
#
.PHONY: libhcan++

TARGET = telican

SRC = $(patsubst %.cc, %.cc , $(wildcard *.cc))
OBJ = $(patsubst %.cc,%.o,$(wildcard *.cc))

LD_FLAGS = -Wl,--export-dynamic -L ../libhcan++ \
		   -lhcan++ -lboost_program_options -lreadline \
		   -lboost_regex \
		   `pkg-config libxml++-2.6 --libs` -fPIC

CXX = g++ #-m32

CXXFLAGS = -Wall -O0 # als 64-Bit-Prozess fuehrt -O2 zum Segmentation fault
# Debug aktivieren:	CXXFLAGS += -g
CXXFLAGS += -I. -I.. -I../libhcan++ -I../include `pkg-config libxml++-2.6 --cflags`
CXXFLAGS += -fPIC
CXXFLAGS += -Werror

all: $(TARGET)

$(TARGET):$(OBJ) libhcan++
	$(CXX) -o $@ $(OBJ) $(LD_FLAGS)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

allSrc:
	$(CXX) $(CXXFLAGS) -o $(SRC) -o $(TARGET) $(LD_FLAGS)
	
clean:
	rm -f *.o $(TARGET)

install:
	strip ./$(TARGET)
	install ./$(TARGET) /usr/bin
	