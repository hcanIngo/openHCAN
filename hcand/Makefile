TARGET = hcand
SRC = main.c socket.c

CXX = clang
CFLAGS = -O2 -I. -Wall -g #-DDEBUG0

all:
	$(CXX) $(CFLAGS) -o $(TARGET) $(SRC)
	
clean:
	rm -f $(TARGET)
	test -d ./scanBuild || sudo rm -R ./scanBuild

install:
	strip $(TARGET)
	sudo install $(TARGET) /usr/sbin
