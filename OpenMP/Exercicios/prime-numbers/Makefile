CPP=g++

DEFS =-O3 -std=c++11
DEFS2 =-fopenmp -O3 -std=c++11

all: prime_numbers_sequential prime_numbers_parallel

prime_numbers_sequential: pn.cpp
	$(CPP) $(DEFS) $< -o $@

prime_numbers_parallel: pn.cpp
	$(CPP) $(DEFS2) $< -o $@
clean:
	rm -rf prime_numbers_sequential prime_numbers_parallel
