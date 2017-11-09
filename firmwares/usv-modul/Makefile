# Makfile fuer versuchsboard1
# 
#

.PHONY:	release

MCU     = atmega32
CC      = avr-gcc
OBJCOPY = avr-objcopy
OBJ     = main.o usvstatemachine.o darlingtonoutput.o \
		  ../../canix/canix.o ../../canix/mcp2515.o \
		  ../../canix/tools.o ../../canix/hcan_hms.o \
		  led.o

F_CPU   = 3686400
F_CAN   = 8000000
CAN_SPEED = 125000

CFLAGS= -mmcu=$(MCU) -Wall -Werror -Wstrict-prototypes -Os -mcall-prologues \
		-DF_CPU=$(F_CPU) -DF_CAN=$(F_CAN) -DCAN_SPEED=$(CAN_SPEED) \
		-I. -I../../canix -I../.. -I../../include -DMCU_$(MCU) \
		-DCANIX_HMS

main.hex : main.out 
	$(OBJCOPY) -R .eeprom -O ihex main.out main.hex 
	avr-size main.hex

main.out : $(OBJ)
	$(CC) $(CFLAGS) -o main.out -Wl,-Map,main.map $(OBJ) $(LDFLAGS)

i_clean_canix:
	cd ../../canix; rm -f *.o *.d *.lst

clean:
	make i_clean_canix
	rm -f *.o *.map *.out *.hex ../../canix/*.o

ctags:
	ctags *.c *.h ../../canix/*.c ../../canix/*.h ../../include/*.h

all: main.hex

load: main.hex
	telican -s 853 -c 400 -e "flash main.hex"

release:
	../../tools/mkrelease
	make clean
	make
	mv main.hex usv-build-`cat buildver`-`date +%Y%m%d`.hex

