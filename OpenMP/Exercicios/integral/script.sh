#!/bin/bash

DIR="./resultados.txt"

> "$DIR"

for threads in {1..3}
do
	echo "$threads threads:" >> "$DIR"

	for i in {1..5}
	do
		g++ -fopenmp teste.cpp
		export OMP_NUM_THREADS=$threads
		./a.out >> $DIR
	done
done
