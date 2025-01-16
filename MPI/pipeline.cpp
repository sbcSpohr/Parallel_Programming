#include <iostream>
#include <random>

#include <mpi.h>
#include <omp.h>

int main(int argc, char **argv) {


///////////////////////////////////////////////
///////////////////////////////////////////////
    const int tam = 100000;
    int vetor1[tam] = {0};
    int vetor2[tam] = {0};
    int vetor3[tam] = {0};
    int soma = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<>dis(1, 100);

   
///////////////////////////////////////////////
///////////////////////////////////////////////

    MPI_Init(&argc, &argv);
    int rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int elementos_por_processo = 25000;

    if(rank == 0) {

        for(int i = 0; i < tam; i++) {
            vetor1[i] = i+1;
        }

        for(int i = 0; i < tam; i+=elementos_por_processo) {
            MPI_Send(&vetor1[i], elementos_por_processo, MPI_INT, 1, 99, MPI_COMM_WORLD);
        }
    }

    if(rank == 0) {
        printf("\n\n");
        for(int i = 0; i < 10; i++) {
            std::cout << vetor1[i] << ", ";
        }
    }

    if(rank == 1) {
        
        for(int i = 0; i < tam; i += elementos_por_processo) {
            MPI_Recv(&vetor2[i], elementos_por_processo, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
            for(int j = 0; j < elementos_por_processo; j++) {
                vetor2[i + j] *= vetor2[i + j];
            }
                MPI_Send(&vetor2[i], elementos_por_processo, MPI_INT, 2, 100, MPI_COMM_WORLD);
            }
        }

    if(rank == 2) {
        
        for(int i = 0; i < tam; i += elementos_por_processo) {
            MPI_Recv(&vetor3[i], elementos_por_processo, MPI_INT, 1, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        #pragma omp parallel for reduction(+:soma)
        for(int i = 0; i < tam; i++) {
            soma += vetor3[i];
        }
        std::cout << "\n\nSoma total: " << soma << std::endl;
    }

    
    if(rank == 0) {
        std::cout << "\n\nPrimeiros 10 elementos do vetor 1 " << std::endl;
        for(int i = 0; i < 10; i++) {
            std::cout << vetor1[i] << ", ";
        }
    } else if(rank == 1){
        std::cout << "\n\n\nPrimeiros 10 elementos do vetor 2 " << std::endl;
        for(int i = 0; i < 10; i++) {
            std::cout << vetor2[i] << ", ";
        }
    }

    MPI_Finalize();

    return 0;
}