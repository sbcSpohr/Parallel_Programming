#include <mpi.h>
#include <stdio.h>

const int TAG_REQUEST = 1;
const int TAG_EOS = 2;
const int TAG_WORK = 3;
const int N = 1000;

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int array[N];
    for(int i = 0; i < N; i++) {
        array[i] = 1;
    }

    if(rank == 0) {
        
        int total_sum = 0;

        int index = 0;
        int tasks_send = 0;

        int eos = 0;

        while(eos < size - 1) {

            MPI_Status status;

            int a;
            MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, TAG_REQUEST, MPI_COMM_WORLD, &status);

            if(index < N) {

                int start = index;
                int end = index + 100;
                MPI_Send(&start, 1, MPI_INT, status.MPI_SOURCE, TAG_WORK, MPI_COMM_WORLD);
                MPI_Send(&end, 1, MPI_INT, status.MPI_SOURCE, TAG_WORK, MPI_COMM_WORLD);
                index+=100;
                tasks_send++;

            } else {
                int b = 0;
                MPI_Send(&b, 1, MPI_INT, status.MPI_SOURCE, TAG_EOS, MPI_COMM_WORLD);
                eos++;
            }
        }

        for(int i = 0; i < tasks_send; i++) {
            int local_sum = 0;
            MPI_Status status;
            MPI_Recv(&local_sum, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_sum += local_sum;
        }
        printf("Result: %d ", total_sum);
    } 
    else
    {
        int eos_received = 0;

        while(eos_received < 1) {

            int a = 0;
            MPI_Send(&a, 1, MPI_INT, 0, TAG_REQUEST, MPI_COMM_WORLD);

            MPI_Status status;
            MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == TAG_EOS) {
                eos_received++;
                continue;
            }    
            int start;
            MPI_Recv(&start, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int end;
            MPI_Recv(&end, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

           

            int local_sum = 0;
            for(int i = start; i < end; i++) {
                local_sum += array[i];
            }
            MPI_Send(&local_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }        

    MPI_Finalize();
}