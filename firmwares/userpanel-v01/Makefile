# Makfile fuer UI
# 
#
include ../../ARCH.inc
.PHONY:	release

#Der atmega644 hat die doppelte EEPROM- und SRAM-Groesse gegenueber dem atmega32.
#export MCU = atmega32
#export MCU = atmega644
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


export CC = avr-gcc
export OBJCOPY = avr-objcopy
OBJ     = main.o ../../canix/canix.o ../../canix/mcp2515.o \
		  ../../canix/tools.o ../../canix/hcan_hms.o \
		  ../../canix/hcan_rts.o ../../canix/syslog.o \
		  ../../canix/eds.o \
		  ../../canix/mem_check.o \
		  timer.o lcd.o lcdctrl.o keys.o lcdstatemachine.o \
		  heiz_page.o gesamt_heiz_page.o rolladen_page.o taster_page.o powerport_page.o \
		  fenster_monitor_page.o temp_anzeige_page.o weck_page.o \
		  reedkontakt_monitor.o lampen_monitor.o sonstige_monitor.o \
		  heiz_monitor.o mute_monitor.o leave_come_page.o

export F_CPU  = 3686400
export F_CAN   = 8000000
export CAN_SPEED = 125000

#export FEATURES = -DBIGMEMUSAGE

export DEFINES = -DMCU_$(MCU) \
        -DF_CPU=$(F_CPU) -DF_CAN=$(F_CAN) -DCAN_SPEED=$(CAN_SPEED) \
        -DCANIX_SYSLOG -DCANIX_HMS -DCANIX_RTS \
        -DCANIX_EDS $(FEATURES)

export GCCFLAGS = -mmcu=$(MCU) -Wall -Werror -Wstrict-prototypes -Os \
        -mcall-prologues

INC = -I. -I../../canix -I../.. -I../../include
CFLAGS= $(GCCFLAGS) $(INC) $(DEFINES)

all: $(TARGET).hex
	sudo test -d ./UI_$(MCU) || mkdir UI_$(MCU)
	sudo mv $(TARGET).hex UI_$(MCU)

$(TARGET).hex : $(TARGET).out
	$(OBJCOPY) -R .eeprom -O ihex $(TARGET).out $(TARGET).hex 
	avr-size $(TARGET).hex

$(TARGET).out : $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET).out -Wl,-Map,$(TARGET).map $(OBJ) $(LDFLAGS)

clean:
	sudo rm -f *.o *.map *.out ../../canix/*.o
	sudo rm -f ./UI_$(MCU)/*.hex

ctags:
	ctags *.c *.h ../../canix/*.c ../../canix/*.h ../../include/*.h

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

