.PHONY: libhcan++
include ../ARCH.inc
include ARCH_letzter_Compilerlauf.inc

ifneq ($(ARCH),$(last_ARCH))
	CONDITIONAL_CLEAN=clean	dep
endif

TARGET = telican
OBJ = main.o readlinepp.o control.o readline_completer.o \
	  readline_completion_generator.o \
	  readline_completion_by_list.o installation_data.o \
	  command.o command_processor.o lampe_command.o sonstige_command.o \
	  reedkontakt_command.o rolladen_command.o \
      heizung_command.o tempsensor_command.o

LD_FLAGS = -Wl,--export-dynamic -L ../libhcan++ \
		   -lhcan++ -lboost_program_options -lreadline \
		   -lboost_regex \
		   `pkg-config libxml++-2.6 --libs` -fPIC

ifeq ($(ARCH),i386)	
	CXX = g++
else	
	CXX = arm-linux-gnu-g++	
endif

INC = -I. -I.. -I../libhcan++ -I../include `pkg-config libxml++-2.6 --cflags`
CXXFLAGS = -Wall -Werror -O2 $(INC) -fPIC


all: $(CONDITIONAL_CLEAN) libhcan++ $(TARGET)

libhcan++:
	cd ../libhcan++ && $(MAKE)

$(TARGET):$(OBJ) libhcan++
	$(CXX) $(LD_FLAGS) -o $@ $(OBJ)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv $@ binary_$(ARCH)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f *.o $(TARGET)
	#cd ../libhcan++ && make clean
	test -d ./binary_$(ARCH) || cd binary_$(ARCH); sudo rm -f $(TARGET)

ctags:
	ctags *.cc *.h ../libhcan++/*.cc ../libhcan++/*.h

install:
	strip ./binary_$(ARCH)/$(TARGET)
	install ./binary_$(ARCH)/$(TARGET) /usr/bin

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

include .depend

