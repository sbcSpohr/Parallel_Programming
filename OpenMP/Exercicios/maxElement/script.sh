#include <stdio.h>
#include <omp.h>


int main() {




    int vetor[8] = {5, 39, 4, 3 , 6, 387, 932, 1005};

    int max = 0;
    int tam = sizeof(vetor)/sizeof(int);

    #pragma omp parallel
    {
        int max_local = 0;

        int id = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        int chunk_size = tam / nthreads;
        int start = id * chunk_size;
        int end = (id == nthreads - 1) ? tam : start + chunk_size;

        for(int i = start; i < end; i++) {
            if(max_local < vetor[i]) {
                max_local = vetor[i];
            }
        }
        #pragma omp atomic
        max += max_local;
    }

    printf("Maior valor: %d", max);
}