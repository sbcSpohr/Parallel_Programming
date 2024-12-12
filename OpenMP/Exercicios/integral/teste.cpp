#include <stdio.h>
#include <omp.h>

static long num_steps = 1000000000;
double step;

int main() {

    int i;
    double x, pi, sum = 0.0;

    step = 1.0 / (double) num_steps;

    double tempo_inicio = omp_get_wtime();

    #pragma omp parallel for reduction(+ : sum) private(x)
    for(int i = 0; i < num_steps; i++) {
        x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x*x);
    }
    pi = step * sum;

    double tempo_final = omp_get_wtime();

    printf("\nTempo de execução: %f segundos\n", tempo_final - tempo_inicio);
   
}