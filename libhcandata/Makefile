include ../ARCH.inc

OBJ = file_format_v1.o data_file_writer.o data_storage.o data_file.o \
		data_query.o

ifeq ($(ARCH),i386)	
	CXX = g++
else	
	CXX = arm-linux-gnu-g++	
endif

INC = -I.. -I. -I../include
CXXFLAGS = -Wall -O2 $(INC) -fPIC #-Werror

all: clean dep libhcandata.so

install:
	sudo /usr/bin/install ./binary_$(ARCH)/libhcandata.so /usr/lib
	sudo ln -fs /usr/lib/libhcandata.so /usr/lib/libhcandata.so.2
	sudo ln -fs /usr/lib/libhcandata.so /usr/lib/libhcandata.so.2.0

libhcandata.so: $(OBJ)
	$(CXX) -shared -o $@ -Wl,-soname,libhcandata.so $(OBJ)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv libhcandata.so binary_$(ARCH)

libhcandata.a: $(OBJ)
	$(ARXX) rcs $@ $(OBJ)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv $@ binary_$(ARCH)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	sudo rm -f *.o ./binary_$(ARCH)/libhcandata.*

ctags:
	ctags *.cc *.h

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

doc:
	doxygen

include .depend

