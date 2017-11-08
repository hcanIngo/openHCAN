TARGET = libhcan++
OBJ = traceable_error.o frame.o frame_message_description.o \
      hcan_transport.o ihexfile.o \
	  board_driver.o atmega32_board_driver.o atmega8_board_driver.o \
	  driver_factory.o board_connection.o controller1612_driver.o \
	  transport_connection.o eds_desc.o userpanel_driver.o \
	  eds_connection.o eds_commands.o usv_driver.o colors.o \
	  dynaddress.o utils.o oap_driver.o oap_access.o oap_desc.o 

#DEBUG = aktiv

INC = -I.. -I. -I../include `pkg-config libxml++-2.6 --cflags`

CXX = g++ #-m32

CXXFLAGS = -Wall -O0 
ifeq ($(DEBUG),aktiv)	
	CXXFLAGS += -g $(INC) 
else	
	CXXFLAGS += $(INC)
	# als 64-Bit-Prozess fuehrt -O2 zum Segmentation fault:   CXXFLAGS = -Wall -O2 $(INC) -fPIC
endif
CXXFLAGS += -fPIC
# wegen deprecated auto_ptr:  CXXFLAGS += -Werror

all: $(TARGET).so

install:
	/usr/bin/install ./$(TARGET).so /usr/lib
	ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2
	ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2.0

$(TARGET).so: $(OBJ)
	$(CXX) -shared -o $@ -fPIC -Wl,-soname,$(TARGET).so $(OBJ)

$(TARGET).a: $(OBJ)
	$(ARXX) rcs $@ $(OBJ)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f *.o $(TARGET)

ctags:
	ctags *.cc *.h

dep: .depend
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

doc:
	doxygen

include .depend

