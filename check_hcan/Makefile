.PHONY: libhcan++

TARGET = check_hcan
OBJ = main.o

LD_FLAGS = -Wl,--export-dynamic -L ../libhcan++ \
		   -lhcan++ -lboost_program_options -lboost_regex \
		   `pkg-config libxml++-2.6 --libs` -fPIC

CXX = g++

INC = -I. -I.. -I../libhcan++ -I../include
CXXFLAGS = -Wall -O2 $(INC)
CXXFLAGS += -Werror

all: libhcan++ $(TARGET)

libhcan++:
	ln -sf libhcan++.so.2.0 libhcan++.so
	cd ../libhcan++ && $(MAKE)

$(TARGET): $(OBJ) libhcan++
	$(CXX) -o $@ $(OBJ) $(LD_FLAGS) 

clean:
	rm -f *.o
	cd ../libhcan++ && make clean

ctags:
	ctags *.cc *.h ../libhcan++/*.cc ../libhcan++/*.h

install:
	#strip ./$(TARGET)
	install $(TARGET) /usr/bin

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

include .depend

