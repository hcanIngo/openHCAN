# Makfile fuer versuchsboard1
# 
.PHONY:	release

SRC = $(patsubst %.c, %.c , $(wildcard *.c))
SRC += $(patsubst %.c, %.c , $(wildcard ../../canix/*.c))

MCU     = atmega32
TARGET  = main

OBJCOPY = avr-objcopy
CXX     = avr-gcc

ifeq ($(MCU),atmega328p)
	F_CPU = 16000000
else # MCU_atmega32 || MCU_atmega644p
	F_CPU = 3686400
endif

F_CAN   = 8000000
CAN_SPEED = 125000


DEFINES = -DMCU_$(MCU) -DF_CPU=$(F_CPU) -DF_CAN=$(F_CAN) -DCAN_SPEED=$(CAN_SPEED) \
          -DCANIX_SYSLOG -DCANIX_HMS -DCANIX_RTS -DCANIX_EDS

CFLAGS =  -mmcu=$(MCU) -Wall -Werror -Wstrict-prototypes -Os -mcall-prologues
# clang:  CFLAGS  = --target=avr -march=$(MCU) -Wall -Werror -Wstrict-prototypes -Os
CFLAGS += -I. -I../../canix -I../.. -I../../include
CFLAGS += $(DEFINES)

all: $(TARGET).hex

$(TARGET).hex : $(TARGET).out 
	$(OBJCOPY) -R .eeprom -O ihex $(TARGET).out $(TARGET).hex 
	avr-size $(TARGET).hex

$(TARGET).out : $(OBJ)
	$(CXX) $(CFLAGS) -o $(TARGET).out $(SRC) -Wl,-Map,$(TARGET).map

clean:
	cd ../../canix; rm -f *.lst
	rm -f *.map *.out *.hex

load: $(TARGET).hex
	telican -s 853 -c 400 -e "flash $(TARGET).hex"

