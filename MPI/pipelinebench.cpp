#include <iostream>
#include <random>

#include <mpi.h>
#include <omp.h>

int main(int argc, char **argv) {

    const int tam = 200000;
    const int n = 30;
    const int m = 60;
    int soma = 0;
    int elementos_por_processo = 50000;

    int vetor1[tam], vetor2[tam], vetor3[tam] = {0};
    double m1[n][n], m2[n][n], m3[n][n];

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


/////////////////  PROCESSO 0  /////////////////

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

/////////////////  PROCESSO 1  /////////////////

    if(rank == 1) {

        for(int i = 0; i < tam; i += elementos_por_processo) {
            MPI_Recv(&vetor2[i], elementos_por_processo, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            #pragma omp parallel for
            for(int j = 0; j < elementos_por_processo; j++) {

                for(int x = 0; x < m; x++) {
                    for(int y = 0; y < m; y++) {
                        vetor2[i + j] += pow(vetor2[i + j], 3) + sin(vetor2[i + j] * x) * cos(vetor2[i + j] * y);
                        vetor2[i + j] += log(vetor2[i + j] + 1) * exp(vetor2[i + j] / 2);
                    }
                }
            }
                MPI_Send(&vetor2[i], elementos_por_processo, MPI_INT, 2, 100, MPI_COMM_WORLD);
            }
        }

/////////////////  PROCESSO 2  /////////////////

    if(rank == 2) {
        
        for(int i = 0; i < tam; i += elementos_por_processo) {
            MPI_Recv(&vetor3[i], elementos_por_processo, MPI_INT, 1, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        #pragma omp parallel for reduction(+ : soma)
        for (int i = 0; i < tam; i++) {

            for(int x = 0; x < n; x++) {
                for(int y = 0; y < n; y++) {
                    m1[x][y] = vetor3[i] + x + y;
                    m2[x][y] = vetor3[i] - x - y;
                    m3[x][y] = 0;
                }
            }

            for(int x = 0; x < n; x++) {
                for(int y = 0; y < n; y++) {
                    for(int z = 0; z < n; z++) {
                        m3[x][y] += m1[x][z] * m2[z][y];
                    }
                }
            }  
            soma += static_cast<int>(m3[0][0]);
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