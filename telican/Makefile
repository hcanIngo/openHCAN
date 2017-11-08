.PHONY: libhcan++

TARGET = telican
OBJ = main.o readlinepp.o control.o readline_completer.o \
	  readline_completion_generator.o \
	  readline_completion_by_list.o installation_data.o \
	  command.o command_processor.o lampe_command.o sonstige_command.o \
	  reedkontakt_command.o rolladen_command.o \
      heizung_command.o tempsensor_command.o

#DEBUG = aktiv

LD_FLAGS = -Wl,--export-dynamic -L ../libhcan++ \
		   -lhcan++ -lboost_program_options -lreadline \
		   -lboost_regex \
		   `pkg-config libxml++-2.6 --libs` -fPIC

CXX = g++ #-m32

INC = -I. -I.. -I../libhcan++ -I../include `pkg-config libxml++-2.6 --cflags`

CXXFLAGS = -Wall -O0
ifeq ($(DEBUG),aktiv)	
	CXXFLAGS += -g 
else	
	# als 64-Bit-Prozess fuehrt -O2 zum Segmentation fault:   CXXFLAGS = -Wall -O2 $(INC) -fPIC
endif
CXXFLAGS += $(INC) -fPIC
# wegen deprecated auto_ptr:  CXXFLAGS += -Werror


all: libhcan++ $(TARGET)

libhcan++:
	ln -sf libhcan++.so.2.0 libhcan++.so
	cd ../libhcan++ && $(MAKE)

$(TARGET):$(OBJ) libhcan++
	$(CXX) -o $@ $(OBJ) $(LD_FLAGS)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f *.o $(TARGET)

ctags:
	ctags *.cc *.h ../libhcan++/*.cc ../libhcan++/*.h

install:
	strip ./$(TARGET)
	install ./$(TARGET) /usr/bin

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

include .depend

