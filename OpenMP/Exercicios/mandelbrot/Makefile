CPP=g++

DEFS =-O3 -std=c++11
DEFS2 =-fopenmp -O3 -std=c++11

all: mandelbrot_sequential mandelbrot_parallel

mandelbrot_sequential: mandelbrot.cpp
		$(CPP) $(DEFS) $< -o $@

mandelbrot_parallel: mandelbrot.cpp
		$(CPP) $(DEFS2) $< -o $@

clean:
	rm -rf mandelbrot_sequential mandelbrot_parallel output.bmp
