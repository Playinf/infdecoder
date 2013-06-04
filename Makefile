# makefile for infdecoder

BIN = infdecoder
BINDIR = ./bin
INCDIR = ./include
SRCDIR = ./source
LIBDIR = ./lib
TESTDIR = ./test

CXX = g++-4.8
CFLAGS = -Wall -O3 -I $(INCDIR) -std=c++11
LIBS = $(LIBDIR)/libsrilm.a $(LIBDIR)/lm.a $(LIBDIR)/dstruct.a $(LIBDIR)/misc.a
LDFLAGS = $(LIBS)

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

# Compile and Assemble C++ Source Files into Object Files
%.o: %.cpp
	echo compiling...$<
	$(CXX) $(CFLAGS) -c $< -o $@

# Link all Object Files with external Libraries into Binaries
$(BIN): $(OBJ)
	echo linking...
	$(CXX) $(CFLAGS) $(OBJ) $(LDFLAGS) -lz -o $(BINDIR)/$(BIN)

.PHONY: clean
clean:
	 -rm -f $(SRCDIR)/*.o $(BINDIR)/$(BIN)

