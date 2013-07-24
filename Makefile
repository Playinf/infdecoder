# makefile for infdecoder

BIN = infdecoder
BINDIR = ./bin
INCDIR = ./include
SRCDIR = ./source
LIBDIR = ./lib
TESTDIR = ./test
EXTDIR = ./extension/moses

CXX = g++
CFLAGS = -Wall -O3 -I $(INCDIR) -std=c++0x
LIBS = $(LIBDIR)/libsrilm.a $(LIBDIR)/liboolm.a $(LIBDIR)/dstruct.a \
       $(LIBDIR)/misc.a
LDFLAGS = $(LIBS)

SRC = $(wildcard $(SRCDIR)/*.cpp)
SRC += $(wildcard $(EXTDIR)/*.cpp)
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

.PHONY: clean
clean:
	-rm -f $(SRCDIR)/*.o $(EXTDIR)/*.o
	-rm -f -r $(BINDIR)

