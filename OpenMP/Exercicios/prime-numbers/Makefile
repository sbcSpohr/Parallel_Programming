CPP=g++

DEFS =-O3 -std=c++11

all: prime_numbers 

prime_numbers: pn.cpp
	$(CPP) $(DEFS) $< -o $@

clean:
	rm -rf prime_numbers
