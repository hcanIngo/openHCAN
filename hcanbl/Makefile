# firmware bootloader
# 
include ../ARCH.inc

CXX     = avr-gcc
OBJCOPY = avr-objcopy

TARGET = main
SRC    = $(TARGET).c
		 
ifeq ($(MCU),atmega328p)
	F_CPU = 16000000
else # MCU_atmega32 || MCU_atmega644p
	F_CPU = 3686400
endif

F_CAN	= 8000000
CAN_SPEED = 125000

ifeq ($(MCU),atmega644p)
	# atmega644 (64kbytes flash = 0 bis FFFF)
	# 0xF000 for firmware, 2048 words boot size
	MT_BOOTLOADER_ADDRESS = F000
	# JTAG-Enable:                   hfuse:w:0x90:m
	FUSES =     -U efuse:w:0xFC:m -U hfuse:w:0xD0:m -U lfuse:w:0xC7:m
	AVRDUDE_FLASH = avrdude -c avrispmkII -p m644p -P usb -U flash:w:"hcanBL_$(MCU)/$(TARGET).hex" $(FUSES)
	AVRDUDE_READ_FLASH = avrdude -c avrispmkII -p m644p -P usb -U flash:r:flash.hex:r
	# Da beim EESAVE FUSEBIT der EEPROM auch bei einem Chip Erase nicht geloescht wird, aendern wir vor dem loeschen die fueses
	AVRDUDE_ERASE_CHIP1 = avrdude -c avrispmkII -p m644p -P usb -U efuse:w:0xFC:m -U hfuse:w:0xD8:m -U lfuse:w:0xC7:m
	AVRDUDE_ERASE_CHIP2 = avrdude -c avrispmkII -p m644p -P usb -e
	AVRDUDE_ERASE_CHIP3 = avrdude -c avrispmkII -p m644p -P usb $(FUSES)
else ifeq ($(MCU),atmega328p)
 	# atmega328p (32kbytes flash = 0 bis 7FFF)
	# 0x7000 for firmware (bytes boot size),  2048 words boot size
	MT_BOOTLOADER_ADDRESS = 7000
	AVRDUDE_READ_FUSES = avrdude -c avrispmkII -p $(MCU) -P usb
	# JTAG-Enable (debugWIRE Enable): hfuse:w:0x80:m
	FUSES =      -U efuse:w:0xFD:m -U hfuse:w:0xC0:m -U lfuse:w:0xFF:m
	AVRDUDE_FLASH = avrdude -c avrispmkII -p $(MCU) -P usb -U flash:w:"hcanBL_$(MCU)/$(TARGET).hex" $(FUSES)
	AVRDUDE_READ_FLASH = avrdude -c avrispmkII -p $(MCU) -P usb -U flash:r:flash.hex:r
	# Da beim EESAVE FUSEBIT der EEPROM auch bei einem Chip Erase nicht geloescht wird, aendern wir vor dem loeschen die fueses
	AVRDUDE_ERASE_CHIP1 = avrdude -c avrispmkII -p $(MCU) -P usb -U efuse:w:0xfd:m -U hfuse:w:0xC8:m -U lfuse:w:0xff:m
	AVRDUDE_ERASE_CHIP2 = avrdude -c avrispmkII -p $(MCU) -P usb -e
	AVRDUDE_ERASE_CHIP3 = avrdude -c avrispmkII -p $(MCU) -P usb $(FUSES)
else
	# atmega32 (32kbytes flash = 0 bis 7FFF)
	# 0x7000 for firmware,  2048 words boot size
	MT_BOOTLOADER_ADDRESS = 7000
	# JTAG-Enable: hfuse:w:0x80:m
	FUSES =     -U hfuse:w:0xC0:m -U lfuse:w:0x0f:m
	AVRDUDE_FLASH = avrdude -c avrispmkII -p m32 -P usb -U flash:w:"hcanBL_$(MCU)/$(TARGET).hex" $(FUSES)
	AVRDUDE_READ_FLASH = avrdude -c avrispmkII -p m32 -P usb -U flash:r:flash.hex:r
	# Da beim EESAVE FUSEBIT der EEPROM auch bei einem Chip Erase nicht geloescht wird, aendern wir vor dem loeschen die fueses
	AVRDUDE_ERASE_CHIP1 = avrdude -c avrispmkII -p m32 -P usb -U hfuse:w:0xc8:m -U lfuse:w:0x0f:m
	AVRDUDE_ERASE_CHIP2 = avrdude -c avrispmkII -p m32 -P usb -e
	AVRDUDE_ERASE_CHIP3 = avrdude -c avrispmkII -p m32 -P usb $(FUSES)
endif


CFLAGS= -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues \
		-std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct \
		-fshort-enums -DF_CPU=$(F_CPU) -DF_CAN=$(F_CAN) -DCAN_SPEED=$(CAN_SPEED)

# Linker flags.
#  -Wl,...:   tell GCC to pass this to linker.
#  -Map:      create map file
#  --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref

################## BOOTLOADER ######################
# MT_BOOTLOADER_ADDRESS =Start of bootloader section in bytes - not words
LDFLAGS += -Wl,--section-start=.text=$(MT_BOOTLOADER_ADDRESS)

all: $(TARGET).hex
	avr-size $(TARGET).hex
	test -d ./hcanBL_$(MCU) || mkdir hcanBL_$(MCU)
	sudo mv $(TARGET).hex hcanBL_$(MCU)

$(TARGET).hex : $(TARGET).out 
	$(OBJCOPY) -R .eeprom -R .signature -O ihex $(TARGET).out $(TARGET).hex 

$(TARGET).out : $(OBJ)
	$(CXX) $(CFLAGS) -o $(TARGET).out $(SRC) $(LDFLAGS)

flash:
	$(AVRDUDE_FLASH)

erase_chip:
	sudo $(AVRDUDE_ERASE_CHIP1)
	sudo $(AVRDUDE_ERASE_CHIP2)
	sudo $(AVRDUDE_ERASE_CHIP3)

# BL und FW lesen (alles was im flash-Speicher enthalten ist)
read_flash:
	$(AVRDUDE_READ_FLASH)

clean_part:
	sudo rm -f *.map *.out

clean:
	make clean_part
	sudo rm -fR ./hcanBL_$(MCU)

