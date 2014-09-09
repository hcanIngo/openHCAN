TARGET = hcan4linuxd
OBJ = main.o

ifeq ($(ARCH),i386)
	CXX = gcc
	#LDFLAGS = -lpopt
else
	CXX = arm-linux-gnu-gcc
endif

CXX = gcc

CFLAGS = -O2 -I. -Wall -g #-DDEBUG0


all: $(CONDITIONAL_CLEAN) $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ)
	strip $@

$(OBJ): %.o:%.c
	$(CXX) $(CFLAGS) -o $@ -c $<
    
clean:
	rm -f *.o $(TARGET)
	@echo last_ARCH=$(ARCH) > ARCH_letzter_Compilerlauf.inc
	sudo rm -f $(TARGET)

ctags:
	ctags *.c *.h

install:
	sudo install $(TARGET) /usr/sbin
