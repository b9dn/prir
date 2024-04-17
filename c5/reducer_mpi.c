#include <mpi.h>
#include <math.h>
#include <stdio.h> 
#include <stdlib.h>

double f1(double x) {
    return x;
}

double f2(double x) {
    return 2*x;
}

double f3(double x) {
    return sin(x);
}

double integrate (double (*func)(double), double begin, double end, int n) {
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    double step = (end - begin) / n;
    double local_sum = 0.0;
    double total_sum = 0.0;
    int reminder = n % world_size;
    int parts_per_process = world_rank < reminder ? n / world_size + 1 : n / world_size;
    printf("rank %d | parts per process %d\n", world_rank, parts_per_process);
    
    int default_parts_per_process = n / world_size;
    double xi_start = begin + step * world_rank * default_parts_per_process;
    if(world_rank < reminder)
        xi_start += world_rank * step;
    else
        xi_start += reminder * step;

    for(int i = 0; i < parts_per_process; i++) {
        double xi = xi_start + i * step;
        #ifdef SIMPSON
        double mid = (2 * xi + step) / 2.0;
        local_sum += step/6*(func(xi) + 4*func(mid) + func(xi+step));
        #else
        local_sum += (func(xi) + func(xi + step)) * step / 2.0;
        #endif
    }
    
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(world_rank == 0)
        return total_sum;

    return 0.0;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if(argc != 4) {
        if(world_rank == 0)
            printf("Bad argument number\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    double begin = atof(argv[1]);
    double end = atof(argv[2]);
    double n_intervals = atoi(argv[3]);

    if(n_intervals <= 0 || begin >= end) {
        if(world_rank == 0)
            printf("Incorrect arguments\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    double result = integrate(f2, begin, end, n_intervals);
    
    if(world_rank == 0)
        printf("Result - %d %f\n", world_size, result);

    // Finalize the MPI environment.
    MPI_Finalize();

    return EXIT_SUCCESS;
}

