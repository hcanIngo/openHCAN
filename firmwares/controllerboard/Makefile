# Makfile fuer Controllerboard
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

ifeq ($(MCU),atmega328p)
	F_CPU = 16000000
	MCU_DIR = C1416_$(MCU)
else # MCU_atmega32 || MCU_atmega644p
	F_CPU = 3686400
	MCU_DIR = C1612_$(MCU)
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
ifeq ($(MCU),atmega328p)
	@SIZE="$(shell avr-size $(TARGET).hex | grep '$(TARGET).hex' | cut -f4 | xargs)" && if [ "$$SIZE" -gt "$(MAXSIZE_ATMEGA328P)" ]; then tput setaf 1; echo "Hex to large for ATmega328p!"; tput sgr0; exit 1;  fi;
else ifeq ($(MCU),atmega32)
	@SIZE="$(shell avr-size $(TARGET).hex | grep '$(TARGET).hex' | cut -f4 | xargs)" && if [ "$$SIZE" -gt $(MAXSIZE_ATMEGA32) ]; then tput setaf 1; echo "Hex to largefor ATmega32!"; tput sgr0; exit 1;  fi;
else ifeq ($(MCU),atmega644p)
	@SIZE="$(shell avr-size $(TARGET).hex | grep '$(TARGET).hex' | cut -f4 | xargs)" && if [ "$$SIZE" -gt $(MAXSIZE_ATMEGA644P) ]; then tput setaf 1; echo "Hex to large!for ATmega644p"; tput sgr0; exit 1;  fi;
endif
	mkdir -p $(MCU_DIR)
	sudo mv $(TARGET).hex ./$(MCU_DIR)

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -R .eeprom -O ihex $(TARGET).elf $(TARGET).hex 
	avr-size $(TARGET).hex

$(TARGET).elf: $(SRC)
	$(CXX) $(CFLAGS) -o $(TARGET).elf $(SRC) -Wl,-Map,$(TARGET).map

clean_part:
	sudo rm -f *.map *.elf

clean:
	make clean_part
	sudo rm -fR ./$(MCU_DIR)
	sudo rm -f devices.c devices.h timer.c
	cd devices; rm -f devices.xml

load:
	make all
	@echo -------------------------
	@echo Bsp.:  make load adr=161
	@echo -------------------------
	telican -C -c $(adr) --arch $(MCU) -e "flash $(MCU_DIR)/$(TARGET).hex"
	#telican --polite-time 2 -a $(IP_HI) -c $(adr) --arch $(MCU) -e "flash $(MCU_DIR)/$(TARGET).hex"

enableDebugWIRE_atmega328p:
	avrdude -c avrispmkII -p $(MCU) -B 12000 -P usb -U hfuse:w:0x88:m
	
disableDebugWIRE_atmega328p:
	@# moeglich:  atmelice, atmelice_isp, atmelice_dw
	avrdude -c atmelice_dw -p $(MCU) -P usb -U hfuse:w:0xC8:m
	
	

