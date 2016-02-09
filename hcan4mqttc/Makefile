include ../hcanhab/MakefileDefines.inc
include ../ARCH.inc
include ARCH_letzter_Compilerlauf.inc

ifneq ($(ARCH),$(last_ARCH))
	CONDITIONAL_CLEAN=clean
endif

TARGET = hcan4mqttc
OBJ = main.o mqttclient.o

ifeq ($(ARCH),i386)
	CXX = gcc
	LDFLAGS += -lmosquitto #-lpopt
else
	CXX = arm-linux-gnu-gcc
	LDFLAGS += -lmosquitto
endif

CFLAGS = -O2 -I. -Wall -g #-DDEBUG0
CFLAGS += -DDEBUG

all: $(CONDITIONAL_CLEAN) $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv $@ binary_$(ARCH)
	strip binary_$(ARCH)/$@

$(OBJ): %.o:%.c
	$(CXX) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f *.o $(TARGET)
	@echo last_ARCH=$(ARCH) > ARCH_letzter_Compilerlauf.inc
	sudo rm -f ./binary_$(ARCH)/$(TARGET)

ctags:
	ctags *.c *.h

install:
	sudo install binary_$(ARCH)/$(TARGET) /usr/sbin
	
bInstall:
	$(_SCP) ./*.c $(SCP_ZIEL_DIR)/../openHCAN/$(TARGET)/
	$(_SCP) ./*.h $(SCP_ZIEL_DIR)/../openHCAN/$(TARGET)/

run: 
	./binary_i386/$(TARGET) -D