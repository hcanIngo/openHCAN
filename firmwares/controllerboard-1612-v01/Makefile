# Makfile fuer C1612
# 
.PHONY:	release
include ../../ARCH.inc

SRC = $(patsubst %.c, %.c , $(wildcard *.c))
SRC += $(patsubst %.c, %.c , $(wildcard ./devices/*.c))
SRC += $(patsubst %.c, %.c , $(wildcard ../../canix/*.c))

TARGET = main
adr = 1022

OBJCOPY = avr-objcopy
CXX = avr-gcc

ifeq ($(MCU),atmega328)
	F_CPU = 16000000
else # MCU_atmega32 || MCU_atmega644
	F_CPU = 3686400
endif

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

clean_part:
	sudo rm -f *.map *.out

clean:
	make clean_part
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
