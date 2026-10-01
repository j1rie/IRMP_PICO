CXX ?= g++

CXXFLAGS ?= -Wall -O -DF_INTERRUPTS=25000

.c.o:
	$(CXX) $(CXXFLAGS) -c $<

irmp-25kHz: irmp.o
	$(CXX) $(LDFLAGS) -o irmp-25kHz irmp.o

irsnd-25kHz: irsnd.o
	$(CXX) $(LDFLAGS) -o irsnd-25kHz irsnd.o

all: irmp-25kHz irsnd-25kHz

clean:
	rm -f *.o irmp-25kHz irsnd-25kHz
