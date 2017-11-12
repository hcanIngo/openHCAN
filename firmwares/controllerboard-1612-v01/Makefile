# Makfile fuer C1612
# 
.PHONY:	release
include ../../ARCH.inc

SRC = $(patsubst %.c, %.c , $(wildcard *.c))
SRC += $(patsubst %.c, %.c , $(wildcard ./devices/*.c))
SRC += $(patsubst %.c, %.c , $(wildcard ../../canix/*.c))

#Der atmega644 hat die doppelte EEPROM- und SRAM-Groesse gegenueber dem atmega32.
#export MCU = atmega32
#export MCU = atmega644
TARGET = main
adr = 1022

OBJCOPY = avr-objcopy
CXX = avr-gcc

F_CPU  = 3686400
F_CAN   = 8000000
CAN_SPEED = 125000

DEFINES = -DMCU_$(MCU) -DF_CPU=$(F_CPU) -DF_CAN=$(F_CAN) -DCAN_SPEED=$(CAN_SPEED) \
          -DCANIX_SYSLOG -DCANIX_HMS -DCANIX_RTS -DCANIX_EDS

CFLAGS =  -mmcu=$(MCU) -Wall -Werror -Wstrict-prototypes -Os -mcall-prologues
CFLAGS += -I. -I../../canix -I../.. -I../../include
CFLAGS += $(DEFINES)

all:
	cd devices; make all
	make allX
	
allX: $(TARGET).hex
	sudo test -d ./C1612_$(MCU) || mkdir C1612_$(MCU)
	sudo mv $(TARGET).hex ./C1612_$(MCU)

$(TARGET).hex: $(TARGET).out
	$(OBJCOPY) -R .eeprom -O ihex $(TARGET).out $(TARGET).hex 
	avr-size $(TARGET).hex

$(TARGET).out:
	$(CXX) $(CFLAGS) -o $(TARGET).out $(SRC) -Wl,-Map,$(TARGET).map

clean:
	cd ../../canix; rm -f *.d *.lst
	sudo rm -f *.map *.out
	sudo rm -fR ./C1612_$(MCU)
	sudo rm -f devices.c devices.h timer.c
	cd devices; rm -f devices.xml

load:
	make all
	@echo -------------------------
	@echo Bsp.:  make load adr=161
	@echo -------------------------
	telican -a $(IP_HI) -C -c $(adr) --arch $(MCU) -e "flash C1612_$(MCU)/$(TARGET).hex"
	#telican --polite-time 2 -a $(IP_HI) -c $(adr) --arch $(MCU) -e "flash C1612_$(MCU)/$(TARGET).hex"

release:
	../../tools/mkrelease
	make clean
	make
	sudo mv C1612_$(MCU)/$(TARGET).hex C1612_$(MCU)/controller1612-build-`cat buildver`-`date +%Y%m%d`.hex
