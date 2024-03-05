#include "primes.h"
#include "calc.h"
#include "file.h"
#include "shm.h"
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

void on_usr1(int signal) {
    printf("Otrzymałem USR1\n");
}

// TAG/folder LAB_1_SIG_MEM  + "_COS"
int main (int argc, char** argv) {
    printf("Moj PID - %d\n", getpid());
    sigset_t mask; /* Maska sygnałów */

    signal(SIGUSR1, &on_usr1);

    int n = argc < 2 ? 5 : atoi(argv[1]);
    int z = argc < 3 ? 20 : atoi(argv[2]);
    char* data_filename = "liczby.out";
    char* result_filename = "wynik.out";
    int* primes = generate_primes_list(z);
    pid_t* child_pids = malloc(n * sizeof(pid_t));

    int* result = (int*)create_shared_memory(n * sizeof(int));
    int* range = (int*)create_shared_memory(2 * n * sizeof(int));
    calc_range_array(range, n, z);
    
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    for(int i = 0; i < n; i++) {
        switch(child_pids[i] = fork()) {
            case -1:
                fprintf(stderr, "Blad w fork\n");
                return EXIT_FAILURE;
            case 0: {
                sigsuspend(&mask);
                result[i] = calc_sum_in_range(primes, range[i*2], range[i*2+1]);
                printf("Zakonczono %d\n", getpid());
                return EXIT_SUCCESS;
            }
            default:
                continue;
        }
    }

    for (int i = 0; i < n; i++) {
        printf("child %d pid: %d\n", i, child_pids[i]);
    }
    while(wait(0) != -1) {}

    int sum = calc_sum(result, n);
    printf("Suma calkowita = %d\n", sum);

    write_nums_to_file(primes, z, data_filename, "Liczba");
    write_nums_to_file(result, n, result_filename, "Wynik");

    free_primes_list(primes);
    free_shared_memory(range, 2 * n * sizeof(int));
    free_shared_memory(result, n * sizeof(int));
    return 0;
}

