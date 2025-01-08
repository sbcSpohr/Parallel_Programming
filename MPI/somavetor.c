#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int vetor[21] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int soma_local = 0;

    int elementos_por_processos = 21 / size;
    int resto = 21 % size;
    
    int start_index = rank * elementos_por_processos;
    int end_index = start_index + elementos_por_processos;
    if(rank == size - 1) {
        end_index+=resto;
    }

    for(int i = start_index; i < end_index; i++) {
        soma_local += vetor[i];
    }
    
    int soma_total = 0;
    MPI_Reduce(&soma_local, &soma_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    printf("\nSoma do processo %d = %d\n", rank, soma_local);

    if(rank == 0) {
        printf("\nSoma: %d\n", soma_total);
    }

    MPI_Finalize();

    return 0;
}