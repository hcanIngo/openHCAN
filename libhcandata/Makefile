TARGET = libhcandata
OBJ = file_format_v1.o data_file_writer.o data_storage.o data_file.o data_query.o

CXX = g++

INC = -I.. -I. -I../include
CXXFLAGS = -Wall -O2 $(INC) -fPIC #-Werror

all: $(TARGET).so

install:
	sudo /usr/bin/install ./$(TARGET).so /usr/lib
	sudo ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2
	sudo ln -fs /usr/lib/$(TARGET).so /usr/lib/$(TARGET).so.2.0

$(TARGET).so: $(OBJ)
	$(CXX) -shared -o $@ -Wl,-soname,$(TARGET).so $(OBJ)

$(TARGET).a: $(OBJ)
	$(ARXX) rcs $@ $(OBJ)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	sudo rm -f *.o ./$(TARGET).*

ctags:
	ctags *.cc *.h

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

doc:
	doxygen

include .depend

