# libhcandata
#
TARGET = libhcandata

SRC = $(patsubst %.cc, %.cc , $(wildcard *.cc))
OBJ = $(patsubst %.cc,%.o,$(wildcard *.cc))

CXX = g++

INC = -I.. -I. -I../include
CXXFLAGS = -Wall -O2 $(INC) -fPIC #-Werror

all: $(TARGET).so

install:
	sudo /usr/bin/install ./$(TARGET).so /usr/lib
	sudo ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2
	sudo ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2.0

$(TARGET).so: $(OBJ)
	$(CXX) -shared -o $@ -Wl,-soname,$(TARGET).so $(OBJ)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<
	
allSrc: 
	$(CXX) $(CXXFLAGS) -o $(SRC) -shared -o $(TARGET).so -fPIC -Wl,-soname,$(TARGET).so -Wl,-Map=$(TARGET).map

clean:
	sudo rm -f *.o $(TARGET).*

doc:
	doxygen
