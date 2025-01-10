#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int tam = 21;
    int vetor[tam] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int soma_local = 0;

    int elementos_por_processos = tam / size;
    int resto = tam % size;

    int start_index = rank * elementos_por_processos;
    int end_index = start_index + elementos_por_processos;
    if (rank == size - 1) {
        end_index += resto;
    }

    for (int i = start_index; i < end_index; i++) {
        soma_local += vetor[i];
    }

    int soma_total = 0;

    if (rank != 0) {
        MPI_Send(&soma_local, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    if(rank == 0) {
        soma_total = soma_local;
        for (int i = 1; i < size; i++) {
            int soma_recebida;
            MPI_Recv(&soma_recebida, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            soma_total += soma_recebida;
        }
    }

    printf("\nProcesso %d, soma local = %d\n", rank, soma_local);

    if (rank == 0) {
        printf("\nSoma total: %d\n", soma_total);
    }

    MPI_Finalize();
    return 0;
}
