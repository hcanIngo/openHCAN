.PHONY: libhcan++
include ../../../ARCH.inc
include ARCH_letzter_Compilerlauf.inc

# Besonderheit: Die Erzeugung von *.oo -Objekten, zur Unterscheidung von o.*-c-Objekten
OBJ_LOCAL := $(patsubst %.cc,%.oo,$(wildcard *.cc))

ifneq ($(ARCH),$(last_ARCH))
	CONDITIONAL_CLEAN=clean	dep
endif

TARGET = hcanweb
OBJ_SHARED = ../shared_client_interface.o \
			 ../shared_hms_interface.o

LD_FLAGS = -Wl,--export-dynamic -L ../../../libhcan++/binary_$(ARCH) \
		   -lhcan++ -lboost_program_options -lreadline \
		   -lboost_regex -lcgicc \
		   `pkg-config libxml++-2.6 --libs` -fPIC

ifeq ($(ARCH),i386)	
	CXX = g++
else	
	CXX = arm-linux-gnu-g++	
endif

INC = -I. -I.. -I../../.. -I../../../libhcan++ -I../../../include `pkg-config libxml++-2.6 --cflags`
CXXFLAGS = -Wall -Werror -O2 $(INC) -fPIC


all: $(CONDITIONAL_CLEAN) libhcan++ $(TARGET)

libhcan++:
	ln -sf libhcan++.so.2.0 libhcan++.so
	cd ../../../libhcan++ && $(MAKE)

$(TARGET):$(OBJ_SHARED) $(OBJ_LOCAL) libhcan++
	$(CXX) $(LD_FLAGS) -o $@ $(OBJ_SHARED) $(OBJ_LOCAL)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv $@ binary_$(ARCH)

$(OBJ_LOCAL): %.oo:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<
	
$(OBJ_SHARED): %.o:%.c
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f *.oo $(TARGET)
	rm -f ../*.o
	sudo rm -f ./binary_$(ARCH)/$(TARGET)
	#touch .depend # leere Datei erzeugen

ctags:
	ctags *.cc *.h ../../../libhcan++/*.cc ../../../libhcan++/*.h
	
dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

include .depend

