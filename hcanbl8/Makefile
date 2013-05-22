
MCU     = atmega8
CC      = avr-gcc
OBJCOPY = avr-objcopy
OBJ     = main.o
		 
F_CPU   = 3686400
F_CAN	= 8000000
CAN_SPEED = 125000

# 0x0C00*2=0x1800 for ATmega8  1024 words Boot Size
MT_BOOTLOADER_ADDRESS = 1800 

CFLAGS= -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues \
		-std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct \
		-fshort-enums -DF_CPU=$(F_CPU) -DF_CAN=$(F_CAN) -DCAN_SPEED=$(CAN_SPEED)

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=main.map,--cref

################## BOOTLOADER ######################
# MT_BOOTLOADER_ADDRESS (=Start of Boot Loader section
# in bytes - not words) is defined above.
LDFLAGS += -Wl,--section-start=.text=$(MT_BOOTLOADER_ADDRESS)

all: main.hex
	avr-size main.hex

main.hex : main.out 
	$(OBJCOPY) -R .eeprom -O ihex main.out main.hex 

main.out : $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o main.out -Wl,-Map,main.map $(OBJ)

load: main.hex
	uisp -dlpt=/dev/parport0 --erase  -dprog=dapa
	uisp -dlpt=/dev/parport0 --upload if=main.hex -dprog=dapa  -v=3 --hash=32

clean:
	rm -f *.o *.map *.out *.hex

ctags:
	ctags *.c *.h

