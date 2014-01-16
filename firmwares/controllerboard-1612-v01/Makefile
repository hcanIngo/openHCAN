# Makfile fuer C1612
# 
#
include ../../ARCH.inc

OBJ_LOCAL := $(patsubst %.c,%.o,$(wildcard *.c))

.PHONY:	release

#Der atmega644 hat die doppelte EEPROM- und SRAM-Groesse gegenueber dem atmega32.
#export MCU = atmega32
#export MCU = atmega644
TARGET = main
adr = 1022

export CC = avr-gcc
export OBJCOPY = avr-objcopy
OBJ     = ../../canix/canix.o ../../canix/mcp2515.o \
		  ../../canix/tools.o ../../canix/hcan_hms.o \
		  ../../canix/hcan_rts.o ../../canix/syslog.o \
		  ../../canix/eds.o \
		  ../../canix/mem_check.o \
		  $(OBJ_LOCAL)

export F_CPU  = 3686400
export F_CAN   = 8000000
export CAN_SPEED = 125000

export DEFINES = -DMCU_$(MCU) \
        -DF_CPU=$(F_CPU) -DF_CAN=$(F_CAN) -DCAN_SPEED=$(CAN_SPEED) \
        -DCANIX_SYSLOG -DCANIX_HMS -DCANIX_RTS \
        -DCANIX_EDS $(FEATURES)

export GCCFLAGS = -mmcu=$(MCU) -Wall -Werror -Wstrict-prototypes -Os \
        -mcall-prologues

INC = -I. -I../../canix -I../.. -I../../include
CFLAGS= $(GCCFLAGS) $(INC) $(DEFINES)

all: $(TARGET).hex
	test -d ./C1612_$(MCU) || mkdir C1612_$(MCU)
	sudo mv $(TARGET).hex C1612_$(MCU)
	ls ./C1612_$(MCU)/*.*

$(TARGET).hex : $(TARGET).out
	$(OBJCOPY) -R .eeprom -O ihex $(TARGET).out $(TARGET).hex 
	avr-size $(TARGET).hex

$(TARGET).out : $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET).out -Wl,-Map,$(TARGET).map $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o *.map *.out ../../canix/*.o
	sudo rm -f ./C1612_$(MCU)/*.hex

ctags:
	ctags *.c *.h ../../canix/*.c ../../canix/*.h ../../include/*.h

load:
	make all
	@echo -------------------------
	@echo Bsp.:  make load adr=161
	@echo -------------------------
	#telican -a $(IP_HI) -c $(adr) --arch $(MCU) -e "flash C1612_$(MCU)/$(TARGET).hex"
	telican -c $(adr) --arch $(MCU) -e "flash C1612_$(MCU)/$(TARGET).hex"

release:
	../../tools/mkrelease
	make clean
	make
	sudo mv C1612_$(MCU)/$(TARGET).hex C1612_$(MCU)/controller1612-build-`cat buildver`-`date +%Y%m%d`.hex

rolladen-statemachine.png : rolladen-statemachine.dot
	dot -Tpng rolladen-statemachine.dot  > rolladen-statemachine.png


