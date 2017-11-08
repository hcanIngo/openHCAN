TARGET = hcanaddressd
OBJ = main.o socket.o

CXX = clang
CFLAGS = -O2 -I. -Wall -g #-DDEBUG0

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
