TARGET = hcand
OBJ = main.o socket.o
CFLAGS = -O2 -I. -Wall -g #-DDEBUG0

CXX = clang

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ)

$(OBJ): %.o:%.c
	$(CXX) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f *.o $(TARGET)

ctags:
	ctags *.c *.h

install:
	strip ./$(TARGET)
	sudo install ./$(TARGET) /usr/sbin
