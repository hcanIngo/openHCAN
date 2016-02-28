include ../ARCH.inc
include ARCH_letzter_Compilerlauf.inc

ifneq ($(ARCH),$(last_ARCH))
	CONDITIONAL_CLEAN=clean
endif

TARGET = hcan4mqttc
OBJ = main.o mqttclient.o

ifeq ($(ARCH),i386)
	# sudo apt-get install llvm clang
	#CXX = clang
	CXX = gcc
	LDFLAGS += -lmosquitto #-lpopt
else
	CXX = arm-linux-gnu-gcc
	LDFLAGS += -lmosquitto
endif

CFLAGS = -I. -Wall -g #-O2
#CFLAGS += -DDEBUG

all: $(CONDITIONAL_CLEAN) $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv $@ binary_$(ARCH)
	strip binary_$(ARCH)/$@

$(OBJ): %.o:%.c
	$(CXX) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f *.o $(TARGET)
	@echo last_ARCH=$(ARCH) > ARCH_letzter_Compilerlauf.inc
	rm -f ./binary_$(ARCH)/$(TARGET)

ctags:
	ctags *.c *.h

install:
	install binary_$(ARCH)/$(TARGET) /usr/sbin
	
run: 
	./binary_i386/$(TARGET) -D
	
ZIEL_PC = bananapi
ZIEL_USER = tt
ZIEL_ORDNER = myOpenHabInstallation
_SCP = sshpass -p il scp -r
SCP_ZIEL_DIR = $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)/$(ZIEL_ORDNER)
bInstall:
	$(_SCP) ./* $(SCP_ZIEL_DIR)/../openHCAN/$(TARGET)/
	#$(_SCP) ./*.c $(SCP_ZIEL_DIR)/../openHCAN/$(TARGET)/
	#$(_SCP) ./*.h $(SCP_ZIEL_DIR)/../openHCAN/$(TARGET)/
