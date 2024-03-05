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

void init_range_and_result(int* range, int* result, int n, int z) {
    int default_chunk_size = (z + 1) / n;
    int ptr = 0;
    for(int i = 0; i < n; i++) {
        int chunk_size = i == n - 1 ? z - (n-1) * default_chunk_size : default_chunk_size;
        range[i*2] = ptr;
        range[i*2+1] = ptr + chunk_size - 1;
        ptr += chunk_size;
        printf("%d - %d   chunk_size %d\n", range[i*2], range[i*2+1], chunk_size);
    }
}

void* create_shared_memory(int size) {
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}

int is_prime(int number) {
    for(int j = 2; j <= number/2; j++) {
        if(number % j == 0)
            return 0;
    }
    return 1;
}

int* generate_primes(int z) {
    int* primes = (int*)calloc(z, sizeof(int));
    int ptr = 0;
    int potential_prime = 2;
    while(ptr < z) {
        if(is_prime(potential_prime)) {
            primes[ptr++] = potential_prime;
        }
        potential_prime++;
    }

    return primes;
}

void sum(int* primes, int* range, int* result, int index) {
    int sum = 0;
    for(int i = range[index*2]; i <= range[index*2+1]; i++)
        sum += primes[i];

    result[index] = sum;
}

// TAG/folder LAB_1_SIG_MEM  + "_COS"
int main (int argc, char** argv) {
    printf("Moj PID - %d\n", getpid());
    /* Pobierz swój PID */
    sigset_t mask; /* Maska sygnałów */

    signal(SIGUSR1, &on_usr1);

    int n = argc < 2 ? 5 : atoi(argv[1]);
    int z = argc < 3 ? 20 : atoi(argv[2]);
    char* filename = "liczby.out";
    int* primes = generate_primes(z);
    pid_t* child_pids = malloc(n * sizeof(pid_t));

    int* range = (int*)create_shared_memory(2 * n * sizeof(int));
    int* result = (int*)create_shared_memory(n * sizeof(int));
    init_range_and_result(range, result, n, z);
    
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    for(int i = 0; i < n; i++) {
        switch(child_pids[i] = fork()) {
            case -1:
                fprintf(stderr, "Blad w fork\n");
                return EXIT_FAILURE;
            case 0: {
                sigsuspend(&mask);
                sum(primes, range, result, i);
                printf("Zakonczono %d\n", getpid());
                return EXIT_SUCCESS;
            }
            default:
                continue;
        }
    }

    for (int i = 0; i < n; i++) {
        printf("child - %d\n", child_pids[i]);
    }
    while(wait(0) != -1) {}

    int sum = 0;
    for(int i = 0; i < n; i++) {
        sum += result[i];
        printf("Result i - %d   =   %d\n", i, result[i]);
    }
    printf("Sum = %d\n", sum);

    free(primes);
    return 0;
}

