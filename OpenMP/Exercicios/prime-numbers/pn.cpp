/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
 *
 *   Application which counts the number of primes between 1 and N
 *
 ============================================================================ 
  Name        : pn.cpp
  Author      : Dalvan Griebler <dalvangriebler@gmail.com>
  Version     : 1.0 (24/07/2015)
  Copyright   : GNU General Public License
  Description : Prime Number Application
 ============================================================================
*/

#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <iostream>
#include <getopt.h>
#include <cstring>



#include <omp.h>

int prime_number(int n){
	int t = 0;
	#pragma omp parallel for reduction(+:t)
	for (int i = 2; i <= n; i++){
		int p = 1;
		for (int j = 2; j < i; j++){
			if (i % j == 0){
				p = 0;
				break;
			}
		}
		t += p;
	}
	return t;
}

int main(int argc, char const *argv[])
{
	int n = atoi(argv[1]);
	auto tstart = std::chrono::high_resolution_clock::now();
	volatile int result = prime_number(n);
	auto tend = std::chrono::high_resolution_clock::now();
    // Execution time
	double TT = std::chrono::duration<double>(tend - tstart).count();
	// Throughput -> numbers processed per second
    double TR = ((double)(n)) / TT;
	std::cout << "1\t" << TT << "\t" << TR << std::endl;
	return 0;
}
