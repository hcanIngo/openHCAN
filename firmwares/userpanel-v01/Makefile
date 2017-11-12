# Makfile fuer UI
# 
.PHONY:	release
include ../../ARCH.inc

TARGET = main

ifeq ($(MCU),atmega644)
	# atmega644 (64kbytes flash = 0 bis FFFF)
	# 0xF000 for firmware, 2048 words boot size
	AVRDUDE_FLASH = avrdude -c avrispmkII -p m644p -P usb -U flash:w:"UI_$(MCU)/$(TARGET).hex" -U hfuse:w:0xD0:m -U lfuse:w:0xc7:m -U efuse:w:0xfc:m
else
	# atmega32 (32kbytes flash = 0 bis 7FFF)
	# 0x7000 for firmware,  2048 words boot size
	AVRDUDE_FLASH = avrdude -c avrispmkII -p m32 -P usb -U flash:w:"UI_$(MCU)/$(TARGET).hex" -U hfuse:w:0xC0:m -U lfuse:w:0x0f:m
endif

SRC = $(patsubst %.c, %.c , $(wildcard *.c))
SRC += $(patsubst %.c, %.c , $(wildcard ../../canix/*.c))

OBJCOPY = avr-objcopy
CXX = avr-gcc

F_CPU  = 3686400
F_CAN   = 8000000
CAN_SPEED = 125000

DEFINES = -DMCU_$(MCU) -DF_CPU=$(F_CPU) -DF_CAN=$(F_CAN) -DCAN_SPEED=$(CAN_SPEED) \
          -DCANIX_SYSLOG -DCANIX_HMS -DCANIX_RTS -DCANIX_EDS
#DEFINES += -DBIGMEMUSAGE

CFLAGS =  -mmcu=$(MCU) -Wall -Werror -Wstrict-prototypes -Os -mcall-prologues
CFLAGS += -I. -I../../canix -I../.. -I../../include
CFLAGS += $(DEFINES)

all: $(TARGET).hex
	sudo test -d ./UI_$(MCU) || mkdir UI_$(MCU)
	sudo mv $(TARGET).hex UI_$(MCU)

$(TARGET).hex: $(TARGET).out
	$(OBJCOPY) -R .eeprom -O ihex $(TARGET).out $(TARGET).hex 
	avr-size $(TARGET).hex

$(TARGET).out:
	$(CXX) $(CFLAGS) -o $(TARGET).out $(SRC) -Wl,-Map,$(TARGET).map

clean:
	cd ../../canix; rm -f *.d *.lst
	sudo rm -f *.map *.out
	sudo rm -fR ./UI_$(MCU)

load:
	make all
	@echo -------------------------
	@echo Bsp.:  make load adr=71
	@echo -------------------------
	# --polite-time 60
	telican -a $(IP_HI) -c $(adr) -C --arch $(MCU) --ignore-type -e "flash UI_$(MCU)/$(TARGET).hex"

release:
	../../tools/mkrelease
	make clean
	make
	mv UI_$(MCU)/$(TARGET).hex UI_$(MCU)/userpanel-build-`cat buildver`-`date +%Y%m%d`.hex

flash:
	sudo $(AVRDUDE_FLASH)

