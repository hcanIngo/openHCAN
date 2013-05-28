include ../ARCH.inc
include ARCH_letzter_Compilerlauf.inc

ifneq ($(ARCH),$(last_ARCH))
	CONDITIONAL_CLEAN=clean	dep
endif

TARGET = libhcan++
OBJ = traceable_error.o frame.o frame_message_description.o \
      hcan_transport.o ihexfile.o \
	  board_driver.o atmega32_board_driver.o atmega8_board_driver.o \
	  driver_factory.o board_connection.o controller1612_driver.o \
	  transport_connection.o eds_desc.o userpanel_driver.o \
	  eds_connection.o eds_commands.o usv_driver.o colors.o \
	  dynaddress.o utils.o oap_driver.o oap_access.o oap_desc.o 

INC = -I.. -I. -I../include `pkg-config libxml++-2.6 --cflags`
CXXFLAGS = -Wall -Werror -O2 $(INC) -fPIC

ifeq ($(ARCH),i386)	
	CXX = g++
	ARXX = ar
else	
	CXX = arm-linux-gnu-g++
	ARXX = arm-linux-gnu-ar
endif



all: $(CONDITIONAL_CLEAN) $(TARGET).so.2.0

install:
	/usr/bin/install ./binary_$(ARCH)/$(TARGET).so.2.0 /usr/lib
	ln -fs /usr/lib/$(TARGET).so.2.0 /usr/lib/$(TARGET).so.2
	ln -fs /usr/lib/$(TARGET).so.2 /usr/lib/$(TARGET).so

$(TARGET).so.2.0: $(OBJ)
	$(CXX) -shared -o $@ -fPIC -Wl,-soname,$(TARGET).so.2 $(OBJ)
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
	sudo rm -f ./binary_$(ARCH)/$(TARGET).so.2.0 ./binary_$(ARCH)/$(TARGET).a

ctags:
	ctags *.cc *.h

dep: .depend
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

doc:
	doxygen

include .depend

