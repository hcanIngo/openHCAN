include ../ARCH.inc
include ARCH_letzter_Compilerlauf.inc

ifneq ($(ARCH),$(last_ARCH))
	CONDITIONAL_CLEAN=clean	dep
endif

#DEBUG = aktiv
TARGET = libhcan++
OBJ = traceable_error.o frame.o frame_message_description.o \
      hcan_transport.o ihexfile.o \
	  board_driver.o atmega32_board_driver.o atmega8_board_driver.o \
	  driver_factory.o board_connection.o controller1612_driver.o \
	  transport_connection.o eds_desc.o userpanel_driver.o \
	  eds_connection.o eds_commands.o usv_driver.o colors.o \
	  dynaddress.o utils.o oap_driver.o oap_access.o oap_desc.o 

INC = -I.. -I. -I../include `pkg-config libxml++-2.6 --cflags`

ifeq ($(ARCH),i386)	
	CXX = g++ #-m32
	ARXX = ar
else	
	CXX = arm-linux-gnu-g++
	ARXX = arm-linux-gnu-ar
endif

ifeq ($(DEBUG),aktiv)	
	CXXFLAGS = -Wall -O0 -g $(INC) -fPIC
else	
	CXXFLAGS = -Wall -O0 $(INC) -fPIC
	# als 64-Bit-Prozess fuehrt -O2 zum Segmentation fault:   CXXFLAGS = -Wall -O2 $(INC) -fPIC

	# wegen deprecated auto_ptr:  CXXFLAGS += -Werror
endif

all: $(CONDITIONAL_CLEAN) $(TARGET).so

install:
	/usr/bin/install ./binary_$(ARCH)/$(TARGET).so /usr/lib
	ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2
	ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2.0

$(TARGET).so: $(OBJ)
	$(CXX) -shared -o $@ -fPIC -Wl,-soname,$(TARGET).so $(OBJ)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	sudo mv $@ binary_$(ARCH)

$(TARGET).a: $(OBJ)
	$(ARXX) rcs $@ $(OBJ)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	sudo mv $@ binary_$(ARCH)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f *.o
	@echo last_ARCH=$(ARCH) > ARCH_letzter_Compilerlauf.inc
	sudo rm -f ./binary_$(ARCH)/$(TARGET).*

ctags:
	ctags *.cc *.h

dep: .depend
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

doc:
	doxygen

include .depend

