TARGET = hcanextid
OBJ = main.o
CXX = gcc
CFLAGS = -O2 -I. -Wall -g #-DDEBUG0


all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ)
	strip $@

$(OBJ): %.o:%.c
	$(CXX) $(CFLAGS) -o $@ -c $<
    
clean:
	sudo rm -f *.o $(TARGET)

ctags:
	ctags *.c *.h

install:
	sudo install $(TARGET) /usr/sbin

# -s 512 -d 35 -p 1
run: 
	./hcanextid -D -s 512 -d 35 -p 1