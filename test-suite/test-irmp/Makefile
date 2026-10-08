CXX ?= g++

CXXFLAGS ?= -Wall -O -DF_INTERRUPTS=50000

.c.o:
	$(CXX) $(CXXFLAGS) -c $<

irmp-50kHz: irmp.o
	$(CXX) $(LDFLAGS) -o irmp-50kHz irmp.o

irsnd-50kHz: irsnd.o
	$(CXX) $(LDFLAGS) -o irsnd-50kHz irsnd.o

all: irmp-50kHz irsnd-50kHz

clean:
	rm -f *.o irmp-50kHz irsnd-50kHz
