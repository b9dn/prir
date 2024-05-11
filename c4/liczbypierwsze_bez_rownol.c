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
  long i, k, liczba, reszta;
  long int lpodz;
  long int llpier = 0;
  double czas;
  FILE *fp;
  double start = omp_get_wtime();
  for (i = 2; i <= S; i++)
    a[i] = 1;
  for (i = 2; i <= S; i++)
    if (a[i] == 1) {
      pierwsze[llpier++] = i;
      for (k = i + i; k <= S; k += i)
        a[k] = 0;
    }

  lpodz = llpier;
  for (liczba = S + 1; liczba <= N; liczba++) {
    for (k = 0; k < lpodz; k++) {
      reszta = (liczba % pierwsze[k]);
      if (reszta == 0)
        break;
    }
    if (reszta != 0) {
      {
        pierwsze[llpier++] = liczba;
      }
    }
  }

  double end = omp_get_wtime();
  printf("%lf\n", end - start);
  if ((fp = fopen("primes_bez.txt", "w")) == NULL) {
    printf("Nie moge otworzyc pliku do zapisu\n");
    exit(1);
  }
  for (i = 0; i < llpier; i++)
    fprintf(fp, "%ld\n", pierwsze[i]);
  fclose(fp);
  return 0;
}
