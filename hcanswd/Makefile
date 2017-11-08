PHONY: libhcan++ libhcandata

TARGET = hcanswd
OBJ = main.o

LD_FLAGS = -Wl,--export-dynamic \
			-L ../libhcandata -lhcandata \
			-L ../libhcan++ -lhcan++ \
			-lboost_program_options \
			-lboost_regex \
			`pkg-config libxml++-2.6 --libs`

CXX = g++

INC = -I. -I.. -I../libhcan++ -I../libhcandata -I../include
CXXFLAGS = -Wall -Werror -O2 $(INC) -g

all: libhcan++ libhcandata $(TARGET)

libhcan++:
	ln -sf libhcan++.so.2.0 libhcan++.so
	cd ../libhcan++ && $(MAKE)

libhcandata:
	cd ../libhcandata && $(MAKE)

$(TARGET): $(OBJ) libhcan++ libhcandata
	$(CXX) -o $@ $(OBJ) $(LD_FLAGS) 

clean:
	rm -f *.o $(TARGET)

ctags:
	ctags *.cc *.h ../libhcan++/*.cc ../libhcan++/*.h ../libhcandata/*.cc ../libhcandata/*.h

install:
	# strip ./$(TARGET)
	sudo install ./$(TARGET) /usr/sbin

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

include .depend

