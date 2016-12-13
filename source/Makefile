# makefile for srilm lib

INCDIR = ../include
LIBDIR = ../lib
SRIDIR = #SRILM-PATH#
NNJMDIR = #NNJM-PATH#

CXX = g++
CFLAGS = -Wall -O3 -I $(INCDIR) -std=c++0x

lib: libsrilm libnnjm libnnlm

libsrilm:
	$(CXX) $(CFLAGS) -I $(SRIDIR)/include -c srilm.cpp -o libsrilm.o
	ar -crv libsrilm.a libsrilm.o

libnnjm:
	$(CXX) $(CFLAGS) -I $(NNJMDIR)/include -c innjm.cpp -o nnjm.o
	ar -crv nnjm.a nnjm.o

libnnlm:
	$(CXX) $(CFLAGS) -I $(NNJMDIR)/include -c innlm.cpp -o nnlm.o
	ar -crv nnlm.a nnlm.o

.PHONY: clean
clean:
	-rm *.o


