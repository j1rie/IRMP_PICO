CXX ?= g++

CXXFLAGS ?= -Wall -O

.c.o:
	$(CXX) $(CXXFLAGS) -c $<

irmpemit: irmpemit.o
	$(CXX) $(LDFLAGS) -o irmpemit irmpemit.o

all: irmpemit

clean:
	rm -f *.o irmpemit
