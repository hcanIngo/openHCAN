# hcandq
#
.PHONY: libhcan++ libhcandata

TARGET = hcandq
OBJ = main.o
SRC = main.cc

LD_FLAGS = -Wl,--export-dynamic \
			-L ../libhcandata -lhcandata -L ../libhcan++ -lhcan++ \
			-lboost_program_options -lboost_regex \
			`pkg-config libxml++-2.6 --libs`

CXX = g++

CXXFLAGS  = -Wall -Werror -O2 
CXXFLAGS += -I. -I.. -I../libhcan++ -I../libhcandata -I../include
CXXFLAGS += -g

all: libhcan++ libhcandata $(TARGET)

libhcan++:
	ln -sf libhcan++.so.2.0 libhcan++.so
	cd ../libhcan++ && $(MAKE)

libhcandata:
	cd ../libhcandata && $(MAKE)

$(TARGET): $(OBJ) libhcan++ libhcandata
	$(CXX) -o $@ $(OBJ) $(LD_FLAGS)

allSrc:
	$(CXX) $(CXXFLAGS) -o $(SRC) -o $(TARGET) $(LD_FLAGS)

clean:
	rm -f *.o
	sudo rm -f $(TARGET)

install:
	strip $(TARGET)
	sudo install $(TARGET) /usr/bin

