.PHONY: libhcan++
include ../ARCH.inc
include ARCH_letzter_Compilerlauf.inc

ifneq ($(ARCH),$(last_ARCH))
	CONDITIONAL_CLEAN=clean	dep
endif

#DEBUG = aktiv
TARGET = telican
OBJ = main.o readlinepp.o control.o readline_completer.o \
	  readline_completion_generator.o \
	  readline_completion_by_list.o installation_data.o \
	  command.o command_processor.o lampe_command.o sonstige_command.o \
	  reedkontakt_command.o rolladen_command.o \
      heizung_command.o tempsensor_command.o

LD_FLAGS = -Wl,--export-dynamic -L ../libhcan++/binary_$(ARCH) \
		   -lhcan++ -lboost_program_options -lreadline \
		   -lboost_regex \
		   `pkg-config libxml++-2.6 --libs` -fPIC

ifeq ($(ARCH),i386)	
	CXX = g++ #-m32
else	
	CXX = arm-linux-gnu-g++	
endif

INC = -I. -I.. -I../libhcan++ -I../include `pkg-config libxml++-2.6 --cflags`

ifeq ($(DEBUG),aktiv)	
	CXXFLAGS = -Wall -g -O0 $(INC) -fPIC
else	
	CXXFLAGS = -Wall -O0 $(INC) -fPIC
	# als 64-Bit-Prozess fuehrt -O2 zum Segmentation fault:   CXXFLAGS = -Wall -O2 $(INC) -fPIC
	# wegen deprecated auto_ptr:  CXXFLAGS += -Werror
endif


all: $(CONDITIONAL_CLEAN) libhcan++ $(TARGET)

libhcan++:
	ln -sf libhcan++.so.2.0 libhcan++.so
	cd ../libhcan++ && $(MAKE)

$(TARGET):$(OBJ) libhcan++
	$(CXX) -o $@ $(OBJ) $(LD_FLAGS)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	sudo mv $@ binary_$(ARCH)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	>.depend
	make dep
	rm -f *.o $(TARGET)
	#cd ../libhcan++ && make clean
	sudo rm -f ./binary_$(ARCH)/$(TARGET)

ctags:
	ctags *.cc *.h ../libhcan++/*.cc ../libhcan++/*.h

install:
	strip ./binary_$(ARCH)/$(TARGET)
	install ./binary_$(ARCH)/$(TARGET) /usr/bin

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

include .depend

