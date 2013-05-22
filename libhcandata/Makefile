include ../ARCH.inc

OBJ = file_format_v1.o data_file_writer.o data_storage.o data_file.o \
		data_query.o

ifeq ($(ARCH),i386)	
	CXX = g++
else	
	CXX = arm-linux-gnu-g++	
endif

INC = -I.. -I. -I../include
CXXFLAGS = -Wall -O2 $(INC) -g #-Werror 

all: clean dep libhcandata.so.2.0

install:
	sudo /usr/bin/install ./binary_$(ARCH)/libhcandata.so.2.0 /usr/lib
	sudo ln -fs /usr/lib/libhcandata.so.2.0 /usr/lib/libhcandata.so.2
	sudo ln -fs /usr/lib/libhcandata.so.2 /usr/lib/libhcandata.so

libhcandata.so.2.0: $(OBJ)
	$(CXX) -shared -o $@ -Wl,-soname,libhcandata.so.2 $(OBJ)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv libhcandata.so.2.0 binary_$(ARCH)

libhcandata.a: $(OBJ)
	$(ARXX) rcs $@ $(OBJ)
	test -d ./binary_$(ARCH) || mkdir binary_$(ARCH)
	mv $@ binary_$(ARCH)

$(OBJ): %.o:%.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f *.o libhcandata.a libhcandata.so.2.0

ctags:
	ctags *.cc *.h

dep:
	(for i in *.cc; do echo; $(CXX) -M -MG $(INC) $$i; done) > .depend

doc:
	doxygen

include .depend

