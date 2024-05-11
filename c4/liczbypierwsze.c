#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#define N 10000000
#define S (int)sqrt(N)
#define M N / 10

int main(int argc, char **argv) {
    long int a[S + 1];    /*tablica pomocnicza*/
    long int pierwsze[M]; /*liczby pierwsze w przedziale 2..N*/
    /*long int pierwsze_thread_private_arr[omp_get_max_threads()][M];*/
    long int** pierwsze_thread_private_arr;
    long i, k, liczba, reszta;
    long int lpodz;      /* l. podzielnikow*/
    long int llpier = 0; /*l. liczb pierwszych w tablicy pierwsze*/
    long int* llpier_thread_private_arr;
    double czas;         /*zmienna do  mierzenia czasu*/
    FILE *fp;
    /*wyznaczanie podzielnikow z przedzialow 2..S*/

    int max_threads = omp_get_max_threads();
    pierwsze_thread_private_arr = malloc(max_threads * sizeof(long int*));
    for (int j = 0; j < max_threads; j++) {
        pierwsze_thread_private_arr[j] = malloc(M * sizeof(long int));
    }

    llpier_thread_private_arr = malloc(max_threads * sizeof(long int));
    for (int j = 0; j < max_threads; j++) {
        llpier_thread_private_arr[j] = 0;
    }

    double start = omp_get_wtime();
#pragma omp parallel for schedule(dynamic) private(i) shared(a)
    for (i = 2; i <= S; i++)
        a[i] = 1; /*inicjowanie*/
    for (i = 2; i <= S; i++)
        if (a[i] == 1) {
            pierwsze[llpier++] = i; /*zapamietanie podzielnika*/
            /*wykreslanie liczb zlozonych bedacych wielokrotnosciami i*/
#pragma omp parallel for shared(a) private(k)
            for (k = i + i; k <= S; k += i)
                a[k] = 0;
        }

    lpodz = llpier; /*zapamietanie liczby podzielnikow*/
/*wyznaczanie liczb pierwszych*/
#pragma omp parallel for private(k, liczba, reszta) shared(pierwsze, pierwsze_thread_private_arr, llpier_thread_private_arr)
    for (liczba = S + 1; liczba <= N; liczba++) {
        for (k = 0; k < lpodz; k++) {
            reszta = (liczba % pierwsze[k]);
            if (reszta == 0)
                break; /*liczba zlozona*/
        }
        if (reszta != 0) {
            int thread_num = omp_get_thread_num();
            pierwsze_thread_private_arr[thread_num][llpier_thread_private_arr[thread_num]++] = liczba;
        }
    }
/*#pragma omp parallel for private(k) shared(llpier_thread_private_arr, pierwsze_thread_private_arr, pierwsze, llpier)*/
    for (k = 0; k < max_threads; k++) {
        for (int j = 0; j < llpier_thread_private_arr[k]; j++) {
/*#pragma omp critical*/
            pierwsze[llpier++] = pierwsze_thread_private_arr[k][j];
        }
    }

    double end = omp_get_wtime();
    printf("%lf\n", end - start);
    if ((fp = fopen("primes.txt", "w")) == NULL) {
        printf("Nie moge otworzyc pliku do zapisu\n");
        exit(1);
    }
    for (i = 0; i < llpier; i++)
        fprintf(fp, "%ld\n", pierwsze[i]);
    fclose(fp);

    return 0;
}
