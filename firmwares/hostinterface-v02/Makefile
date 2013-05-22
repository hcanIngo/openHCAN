# Makfile fuer versuchsboard1
# 
#

.PHONY:	release

MCU     = atmega32
CC      = avr-gcc
OBJCOPY = avr-objcopy
OBJ     = main.o usb.o \
		  ../../canix/canix.o ../../canix/mcp2515.o \
		  ../../canix/tools.o ../../canix/hcan_hms.o 

F_CPU   = 4000000
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

clean:
	rm -f *.o *.map *.out *.hex ../../canix/*.o

ctags:
	ctags *.c *.h ../../canix/*.c ../../canix/*.h ../../include/*.h

all: main.hex

load: main.hex
	uisp -dlpt=/dev/parport0 --erase  -dprog=dapa
	uisp -dlpt=/dev/parport0 --upload if=main.hex -dprog=dapa  -v=3 --hash=32

flash: main.hex
	sudo avrdude -c avrispmkII -p m32 -P usb -e -U flash:w:"main.hex" -U hfuse:w:0xC0:m -U lfuse:w:0x03:m -U lock:w:0x2F:m

release:
	../../tools/mkrelease
	make clean
	make
	mv main.hex hostinterface-v02-build-`cat buildver`-`date +%Y%m%d`.hex

