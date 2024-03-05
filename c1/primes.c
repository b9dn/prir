#include "primes.h"
#include <stdlib.h>

int is_prime(int number) {
    for(int j = 2; j <= number/2; j++) {
        if(number % j == 0)
            return 0;
    }
    return 1;
}

int* generate_primes_list(int n) {
    int* primes = (int*)malloc(n * sizeof(int));
    int ptr = 0;
    int potential_prime = 2;
    while(ptr < n) {
        if(is_prime(potential_prime)) {
            primes[ptr++] = potential_prime;
        }
        potential_prime++;
    }

    return primes;
}

void free_primes_list(int* primes) {
    free(primes);
}