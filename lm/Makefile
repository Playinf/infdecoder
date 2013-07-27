# makefile for srilm lib

INCDIR = ../include
LIBDIR = ../lib
SRIDIR = /home/playinf/Applications/srilm

CXX = g++
CFLAGS = -Wall -O3 -I $(INCDIR) -std=c++0x


lib:
	$(CXX) $(CFLAGS) -I $(SRIDIR)/include -c srilm.cpp -o libsrilm.o
	ar -crv libsrilm.a libsrilm.o
	mv libsrilm.a $(LIBDIR)

.PHONY: clean
clean:
	-rm *.o


