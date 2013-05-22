include ../ARCH.inc
include ARCH_letzter_Compilerlauf.inc

ifneq ($(ARCH),$(last_ARCH))
	CONDITIONAL_CLEAN=clean
endif

TARGET = hcanaddressd
OBJ = main.o socket.o

ifeq ($(ARCH),i386)
	CXX = gcc
	CFLAGS = -O2 -I. -Wall -g #-DDEBUG0
else
	CXX = arm-linux-gnu-gcc
	CFLAGS = -O2 -I/usr/arm-linux-gnu/include -Wall -g #.I tut auch!
endif


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
	cd binary_$(ARCH); sudo rm -f $(TARGET)

ctags:
	ctags *.c *.h

install:
	sudo install binary_$(ARCH)/$(TARGET) /usr/sbin

