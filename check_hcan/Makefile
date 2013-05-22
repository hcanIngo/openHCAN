.PHONY: libhcan++

include ../ARCH.inc

ifneq ($(ARCH),$(last_ARCH))
	CONDITIONAL_CLEAN=clean
endif

TARGET = check_hcan
OBJ = main.o

LD_FLAGS = -Wl,--export-dynamic -L ../libhcan++ \
		   -lhcan++ -lboost_program_options -lboost_regex \
		   `pkg-config libxml++-2.6 --libs` -fPIC

ifeq ($(ARCH),i386)	
	CXX = g++
else	
	CXX = arm-linux-gnu-g++	
endif

INC = -I. -I.. -I../libhcan++ -I../include
CXXFLAGS = -Wall -Werror -O2 $(INC)

all: libhcan++ $(CONDITIONAL_CLEAN) $(TARGET)

libhcan++:
	cd ../libhcan++ && $(MAKE)

$(TARGET): $(OBJ) libhcan++
	$(CXX) -o $@ $(OBJ) $(LD_FLAGS) 
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv $@ binary_$(ARCH)

clean:
	rm -f *.o
	cd ../libhcan++ && make clean
	cd binary_$(ARCH); sudo rm -f $(TARGET)

ctags:
	ctags *.cc *.h ../libhcan++/*.cc ../libhcan++/*.h

install:
	cd binary_$(ARCH); install $(TARGET) /usr/bin

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

include .depend

