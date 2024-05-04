#include "mpi.h"
#include <stdio.h>
#define SIZE 4
#define LEFT  0
#define RIGHT 1

int main(int argc, char *argv[])  {
    int numtasks, rank, source, dest, outbuf, i, tag = 1,
        inbuf[2]={MPI_PROC_NULL, MPI_PROC_NULL},
        nbrs[2], dims[1] = {4},
        periods[1] = {0}, reorder = 0, coords[1];

    MPI_Request reqs[4];
    MPI_Status stats[4];
    MPI_Comm cartcomm;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if(numtasks == SIZE) {
        MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &cartcomm);
        MPI_Comm_rank(cartcomm, &rank);
        MPI_Cart_coords(cartcomm, rank, 1, coords);
        MPI_Cart_shift(cartcomm, 1, 1, &nbrs[LEFT], &nbrs[RIGHT]);

        printf("rank = %d coords = %d  neighbors(l,r) = %d %d\n",
                rank, coords[0], nbrs[LEFT], nbrs[RIGHT]);

        outbuf = rank;

        for(i = 0; i < 2; i++) {
            dest = nbrs[i];
            source = nbrs[i];
            MPI_Isend(&outbuf, 1, MPI_INT, dest, tag,
                    MPI_COMM_WORLD, &reqs[i]);
            MPI_Irecv(&inbuf[i], 1, MPI_INT, source, tag,
                    MPI_COMM_WORLD, &reqs[i+2]);
        }

        MPI_Waitall(4, reqs, stats);

        printf("rank = %d inbuf(l,r) = %d %d\n",
                rank, inbuf[LEFT], inbuf[RIGHT]);  
    }
    else
        printf("Must specify %d processors. Terminating.\n", SIZE);

    MPI_Finalize();
}
