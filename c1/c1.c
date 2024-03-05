#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void child_on_usr1(int signal) {
    printf("Child otrzymal USR1\n");
}

void init_range_array(int* arr, int n, int z) {
    if(z < n) {
        for(int i = 0; i < z; i++) {
            arr[i*2] = i;
            arr[i*2+1] = i;
            printf("Range segment %d: %d - %d\n", i, arr[i*2], arr[i*2+1]);
        }
        for(int i = z; i < n; i++) {
            arr[i*2] = 0;
            arr[i*2+1] = 0;
            printf("Range segment %d: %d - %d\n", i, arr[i*2], arr[i*2+1]);
        }
        return;
    }

    int default_chunk_size = z / n;
    int reminder = z % n;
    int ptr = 0;
    for(int i = 0; i < n; i++) {
        int chunk_size = reminder-- > 0 ? default_chunk_size + 1 : default_chunk_size;
        arr[i*2] = ptr;
        arr[i*2+1] = ptr + chunk_size - 1;
        ptr += chunk_size;
        printf("Range segment %d: %d - %d\n", i, arr[i*2], arr[i*2+1]);
    }
}

int calc_sum_in_range(int* nums, int start, int end) {
    int sum = 0;
    for(int i = start; i <= end; i++)
        sum += nums[i];
    return sum;
}

int calc_sum(int* nums, int size) {
    int sum = 0;
    for(int i = 0; i < size; i++)
        sum += nums[i];
    return sum;
}

void write_nums_to_file(int *nums, int size, char *filename, char* line_prefix) {
    FILE *file = fopen(filename, "w");
    if(file == NULL) {
        fprintf(stderr, "Blad w fopen");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < size; i++)
        fprintf(file, "%s %d: %d\n", line_prefix, i, nums[i]);

    fclose(file);
}

int is_prime(int number) {
    for(int j = 2; j <= number/2; j++)
        if(number % j == 0)
            return 0;
    return 1;
}

int* generate_primes_list(int n) {
    int* primes = (int*)malloc(n * sizeof(int));
    int ptr = 0;
    int potential_prime = 2;
    while(ptr < n) {
        if(is_prime(potential_prime))
            primes[ptr++] = potential_prime;
        potential_prime++;
    }

    return primes;
}

void* create_shared_memory(int size) {
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    int *shm_seg = mmap(NULL, size, protection, visibility, -1, 0);
    if(shm_seg == MAP_FAILED) {
        fprintf(stderr, "Blad w mmap");
        exit(EXIT_FAILURE);
    }
    return shm_seg;
}

void free_shared_memory(void* ptr, int size) {
    if(munmap(ptr, size) == -1) {
        fprintf(stderr, "Blad w munmap");
        exit(EXIT_FAILURE);
    }
}

int main (int argc, char** argv) {
    printf("Moj PID - %d\n", getpid());

    int n = argc < 2 ? 5 : atoi(argv[1]);
    if (n < 1) {
        fprintf(stderr, "Bledna liczba n\n");
        return EXIT_FAILURE;
    }
    int z = argc < 3 ? 20 : atoi(argv[2]);
    if (z < 1) {
        fprintf(stderr, "Bledna liczba z\n");
        return EXIT_FAILURE;
    }
    char* data_filename = "liczby.out";
    char* result_filename = "wynik.out";
    int* primes = generate_primes_list(z);
    pid_t* child_pids = (pid_t*)malloc(n * sizeof(pid_t));

    int* result = (int*)create_shared_memory(n * sizeof(int));
    int* range = (int*)create_shared_memory(2 * n * sizeof(int));
    init_range_array(range, n, z);
    
    signal(SIGUSR1, &child_on_usr1);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);

    for(int i = 0; i < n; i++) {
        switch(child_pids[i] = fork()) {
            case -1:
                fprintf(stderr, "Blad w fork\n");
                return EXIT_FAILURE;
            case 0:
                sigsuspend(&mask);
                int sum = range[i*2+1] ? calc_sum_in_range(primes, range[i*2], range[i*2+1]) : 0;
                result[i] = sum;
                printf("Zakonczono %d\n", getpid());
                return EXIT_SUCCESS;
            default:
                continue;
        }
    }

    for (int i = 0; i < n; i++)
        printf("Child %d pid: %d\n", i, child_pids[i]);

    sleep(1);
    for(int i = 0; i < n; i++)
        kill(child_pids[i], SIGUSR1);

    while(wait(0) != -1) {}

    int sum = calc_sum(result, n);
    printf("Suma calkowita = %d\n", sum);

    write_nums_to_file(primes, z, data_filename, "Liczba");
    write_nums_to_file(result, n, result_filename, "Wynik");

    free(child_pids);
    free(primes);
    free_shared_memory(range, 2 * n * sizeof(int));
    free_shared_memory(result, n * sizeof(int));
    return 0;
}
