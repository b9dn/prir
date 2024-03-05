#ifndef PRIMES_H
#define PRIMES_H

int is_prime(int number);
int* generate_primes_list(int n);
void free_primes_list(int* primes_list);

#endif