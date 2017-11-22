# check_hcan
#
.PHONY: libhcan++

TARGET = check_hcan
OBJ = main.o
SRC = main.cc

LD_FLAGS = -Wl,--export-dynamic -L ../libhcan++ \
		   -lhcan++ -lboost_program_options -lboost_regex \
		   `pkg-config libxml++-2.6 --libs` -fPIC

CXX = g++

CXXFLAGS = -Wall -O2
CXXFLAGS += -I. -I.. -I../libhcan++ -I../include
CXXFLAGS += -Werror

all: libhcan++ $(TARGET)

libhcan++:
	ln -sf libhcan++.so.2.0 libhcan++.so
	cd ../libhcan++ && $(MAKE)

$(TARGET): $(OBJ) libhcan++
	$(CXX) -o $@ $(OBJ) $(LD_FLAGS) 

allSrc:
	$(CXX) $(CXXFLAGS) -o $(SRC) -o $(TARGET) $(LD_FLAGS)

clean:
	rm -f *.o $(TARGET)
	cd ../libhcan++ && make clean


install:
	#strip ./$(TARGET)
	install $(TARGET) /usr/bin

