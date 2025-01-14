#include <iostream>
#include <random>

#include <mpi.h>

int main(int argc, char **argv) {


///////////////////////////////////////////////
///////////////////////////////////////////////
    const int tam = 100000;
    int vetor1[tam];
    int vetor2[tam];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<>dis(1, 100);

    for(int i = 0; i < tam; i++) {
        vetor1[i] = dis(gen);
    }
///////////////////////////////////////////////
///////////////////////////////////////////////

    MPI_Init(&argc, &argv);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int elementos_por_processo = 25000;

    if(rank == 0) {

        for(int i = 0; i < tam; i+=elementos_por_processo){
            for(int j = 0; j < elementos_por_processo; j++) {
                vetor1[i + j] *= 2;
            }
            MPI_Send(&vetor1[i], elementos_por_processo, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
    }

    if(rank == 1) {
        
        for(int i = 0; i < tam; i += elementos_por_processo) {
            MPI_Recv(&vetor2[i], elementos_por_processo, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    
    if(rank == 0) {
        std::cout << "Primeiros 100 elementos do vetor 1 " << std::endl;
        for(int i = 0; i < 100; i++) {
            std::cout << vetor1[i] << ", ";
        }

        std::cout << "\n\n\nPrimeiros 100 elementos do vetor 2 " << std::endl;
        for(int i = 0; i < 100; i++) {
            std::cout << vetor2[i] << ", ";
        }
    }

    MPI_Finalize();

    return 0;
}