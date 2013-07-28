# makefile for infdecoder

BIN = infdecoder
BINDIR = ./bin
INCDIR = ./include
SRCDIR = ./source
LIBDIR = ./lib
TESTDIR = ./test
EXTDIR = ./extension/
EXTIODIR = $(EXTDIR)/io
EXTMOSESDIR = $(EXTDIR)/moses
SRIDIR = /home/playinf/Applications/srilm

CXX = g++
CFLAGS = -Wall -O3 -I $(INCDIR) -std=c++0x
LIBS = $(LIBDIR)/libsrilm.a $(LIBDIR)/liboolm.a $(LIBDIR)/libdstruct.a \
       $(LIBDIR)/libmisc.a
LDFLAGS = $(LIBS)

SRC = $(wildcard $(SRCDIR)/*.cpp)
SRC += $(wildcard $(EXTIODIR)/*.cpp)
SRC += $(wildcard $(EXTMOSESDIR)/*.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

# Compile and Assemble C++ Source Files into Object Files
%.o: %.cpp
	echo compiling...$<
	$(CXX) $(CFLAGS) -c $< -o $@

# Link all Object Files with external Libraries into Binaries
$(BIN): dir $(OBJ)
	echo linking...
	$(CXX) $(CFLAGS) $(OBJ) $(LDFLAGS) -lz -lpthread -o $(BINDIR)/$(BIN)

dir:
	-mkdir -p $(BINDIR)

lib:
	$(CXX) $(CFLAGS) -I $(SRIDIR)/include -c source/lm.cpp -o libsrilm.o
	ar -crv libsrilm.o libsrilm.a
	mv libsrilm.a lib
	rm libsrilm.o

.PHONY: clean
clean:
	-rm -f $(SRCDIR)/*.o $(EXTIODIR)/*.o $(EXTMOSESDIR)/*.o
	-rm -f -r $(BINDIR)

