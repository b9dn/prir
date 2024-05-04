#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int nwd(int a, int b){
    while (b != 0) {
        int pom = b;
        b = a % b;
        a = pom;
    }

    return a;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    srand(world_rank + time(NULL));
    int rand_liczba = rand();
    int liczba = rand_liczba == 0 ? 1 : rand_liczba % 29 + 1;
    /* int liczba = world_rank * 4 + 4; */
    printf("Process %d, Liczba %d\n", world_rank, liczba);

    int received;
    int result = liczba;

    int index_to_add = 1;
    int iter_n = 0;
    int max_iters = log2(world_size);

    while(iter_n < max_iters) {
        int destination = (world_rank + index_to_add) % world_size;
        MPI_Send(&liczba, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);

        int src = (world_rank - index_to_add + world_size) % world_size;
        MPI_Recv(&received, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int result = nwd(received, liczba);

        printf("Proces nr: %d, iteracja: %d, liczone nwd(%d, %d) = %d\n", world_rank, index_to_add, liczba, received, result);
        liczba = result;
        index_to_add *= 2;
        iter_n++;
    }

    if(world_rank == 0) {
        printf("Koniec proces %d, wynik nwd = %d <------------------- wynik\n", world_rank, liczba);
    }

    MPI_Finalize();

    return 0;
}
