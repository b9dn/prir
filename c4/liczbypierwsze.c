#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#define N 10000000
#define S (int)sqrt(N)
#define M N / 10

int main(int argc, char **argv) {
    long int a[S + 1];
    long int pierwsze[M];
    long int **pierwsze_thread_private_arr;
    long i, k, liczba, reszta;
    long int lpodz;
    long int llpier = 0;
    long int *llpier_thread_private_arr;
    double czas;
    FILE *fp;

    int max_threads = omp_get_max_threads();
    pierwsze_thread_private_arr =
        (long int **)malloc(max_threads * sizeof(long int *));
    if (pierwsze_thread_private_arr == NULL) {
        printf("Malloc failed\n");
        exit(1);
    }
    for (int j = 0; j < max_threads; j++) {
        pierwsze_thread_private_arr[j] =
            (long int *)malloc(M * sizeof(long int));
        if (pierwsze_thread_private_arr[j] == NULL) {
            printf("Malloc failed\n");
            exit(1);
        }
    }

    llpier_thread_private_arr =
        (long int *)malloc(max_threads * sizeof(long int));
    if (llpier_thread_private_arr == NULL) {
        printf("Malloc failed\n");
        exit(1);
    }
    for (int j = 0; j < max_threads; j++) {
        llpier_thread_private_arr[j] = 0;
    }

    double start = omp_get_wtime();
#pragma omp parallel for schedule(dynamic) private(i) shared(a)
    for (i = 2; i <= S; i++)
        a[i] = 1;
    for (i = 2; i <= S; i++)
        if (a[i] == 1) {
            pierwsze[llpier++] = i;
#pragma omp parallel for shared(a) private(k)
            for (k = i + i; k <= S; k += i)
                a[k] = 0;
        }

    lpodz = llpier;
#pragma omp parallel for private(k, liczba, reszta) shared(pierwsze, pierwsze_thread_private_arr, llpier_thread_private_arr)
    for (liczba = S + 1; liczba <= N; liczba++) {
        for (k = 0; k < lpodz; k++) {
            reszta = (liczba % pierwsze[k]);
            if (reszta == 0)
                break;
        }
        if (reszta != 0) {
            int thread_num = omp_get_thread_num();
            pierwsze_thread_private_arr
                [thread_num][llpier_thread_private_arr[thread_num]++] = liczba;
        }
    }
    for (k = 0; k < max_threads; k++) {
        for (int j = 0; j < llpier_thread_private_arr[k]; j++) {
            pierwsze[llpier++] = pierwsze_thread_private_arr[k][j];
        }
    }

    double end = omp_get_wtime();
    printf("Time - %lf\n", end - start);

    if ((fp = fopen("primes.txt", "w")) == NULL) {
        printf("Nie moge otworzyc pliku do zapisu\n");
        exit(1);
    }
    for (i = 0; i < llpier; i++)
        fprintf(fp, "%ld\n", pierwsze[i]);
    fclose(fp);

    for (int j = 0; j < max_threads; j++) {
        free(pierwsze_thread_private_arr[j]);
    }
    free(pierwsze_thread_private_arr);

    return 0;
}
