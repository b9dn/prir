#include "mpi.h"
#include <stdio.h>
#define SIZE 4
#define LEFT  0
#define RIGHT 1
#define SEND_ARR_SIZE 10

void print_array(int *array, int size) {
    printf("{ ");
    for(int i = 0; i < size; i++)
        printf("%d ", array[i]);
    printf("}\n");
}

void do_sth_with_input(int *inbuf, int *outbuf, int index, int size) {
    for(int i = index; i < index + size; i++) {
        outbuf[i] = inbuf[i] * 2;
    }
}

int main(int argc, char *argv[])  {
    int numtasks, rank, source, dest, i, tag = 1,
        nbrs[2], dims[1] = {4},
        periods[1] = {0}, reorder = 0, coords[1];

    MPI_Comm cartcomm;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if(numtasks != SIZE) {
        printf("Must specify %d processors. Terminating.\n", SIZE);
        MPI_Finalize();
        return -1;
    }

    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &cartcomm);
    MPI_Comm_rank(cartcomm, &rank);
    MPI_Cart_coords(cartcomm, rank, 1, coords);
    MPI_Cart_shift(cartcomm, 0, 1, &nbrs[LEFT], &nbrs[RIGHT]);

    int outbuf[SEND_ARR_SIZE] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int inbuf[SEND_ARR_SIZE] = {0};
    source = nbrs[LEFT];
    dest = nbrs[RIGHT];
    
    int total_sum_last = 0;
    for(int i = 0; i < SEND_ARR_SIZE; i++) {
        MPI_Recv(&inbuf[i], 1, MPI_INT, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        if(rank != 0)
            do_sth_with_input(inbuf, outbuf, i, 1);

        MPI_Send(&outbuf[i], 1, MPI_INT, dest, tag, MPI_COMM_WORLD);

        if(rank == numtasks - 1) {
            total_sum_last += inbuf[i];
        }
    }

    printf("rank = %d inbuf =\n\t", rank);
    print_array(inbuf, SEND_ARR_SIZE);

    if(rank == numtasks - 1)
        printf("Total last sum = %d\n", total_sum_last);

    MPI_Finalize();
}
