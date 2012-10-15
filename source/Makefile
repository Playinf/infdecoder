# makefile for infdecoder

EXE = ../infdecoder
CXX = g++
CFLAGS = -Wall -O3 -I ../include -std=c++0x
LIBS = ../lib/libsrilm.a ../lib/lm.a ../lib/dstruct.a ../lib/misc.a
LDFLAGS = $(LIBS)

# Compile and Assemble C++ Source Files into Object Files
%.o: %.cpp
	echo compiling...$<
	$(CXX) $(CFLAGS) -c $<
# -o $@
# Source and Object files
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

# Link all Object Files with external Libraries into Binaries
$(EXE): $(OBJ)
	echo linking...
	$(CXX) $(CFLAGS) $(OBJ) $(LDFLAGS) -lz -o $(EXE)

.PHONY: clean
clean:
	 -rm -f core *.o

