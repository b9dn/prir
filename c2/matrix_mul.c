#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

double** A_shared;
double** B_shared;
double** C_shared;
int mc, nc, mid;

void mnoz(double**A, int a, int b, double**B, int c, int d, double**C) {
    int i, j, k;
    double s;
    for(i=0; i<a; i++) {
        for(j=0; j<d; j++) {
            s = 0;
            for(k=0; k<b; k++) {
                s+=A[i][k]*B[k][j];
            }
            C[i][j] = s;
        }

    }
}

void print_matrix(double**A, int m, int n) {
    int i, j;
    printf("[");
    for(i =0; i< m; i++)
    {
        for(j=0; j<n; j++)
        {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }
    printf("]\n");
}

void init_matrix(double*** matrix_A, int* ma, int* na, double*** matrix_B, int* mb, int* nb, double*** matrix_C) {
    double **A;
    double **B;
    double **C;

    FILE *fpa;
    FILE *fpb;
    int i, j;
    double x;

    fpa = fopen("A.txt", "r");
    fpb = fopen("B.txt", "r");
    if( fpa == NULL || fpb == NULL ) {
        perror("błąd otwarcia pliku");
        exit(-10);
    }

    fscanf (fpa, "%d", ma);
    fscanf (fpa, "%d", na);


    fscanf (fpb, "%d", mb);
    fscanf (fpb, "%d", nb);

    printf("pierwsza macierz ma wymiar %d x %d, a druga %d x %d\n", *ma, *na, *mb, *nb);

    if(*na != *mb) {
        printf("Złe wymiary macierzy!\n");
        exit(-1);
    }
    
    /*Alokacja pamięci*/
    A = malloc(*ma*sizeof(double));
    for(i=0; i< *ma; i++) {
        A[i] = malloc(*na*sizeof(double));
    }

    B = malloc(*mb*sizeof(double));
    for(i=0; i< *mb; i++) {
        B[i] = malloc(*nb*sizeof(double));
    }

    /*Macierz na wynik*/
    C = malloc(*ma*sizeof(double));
    for(i=0; i< *ma; i++) {
        C[i] = malloc(*nb*sizeof(double));
    }
    printf("Rozmiar C: %dx%d\n", *ma, *nb);

    for(i =0; i< *ma; i++) {
        for(j = 0; j<*na; j++) {
            fscanf( fpa, "%lf", &x );
            A[i][j] = x;
        }
    }

    printf("A:\n");
    print_matrix(A, *ma, *mb);

    for(i =0; i< *mb; i++) {
        for(j = 0; j<*nb; j++) {
            fscanf( fpb, "%lf", &x );
            B[i][j] = x;
        }
    }
    printf("B:\n");
    print_matrix(B, *mb, *nb);

    mnoz(A, *ma, *na, B, *mb, *nb, C);

    printf("C:\n");
    print_matrix(C, *ma, *nb);

    fclose(fpa);
    fclose(fpb);


    *matrix_A = A;
    *matrix_B = B;
    *matrix_C = C;
}

void* calc_matrix_chunk(void* args) {
    int chunk_size = *(int*)args;
    int offset = ((int*)args)[1];
    int col = offset % nc;
    int row = offset / nc;
    /* printf("Thread cs %d  off %d - col  %d x row %d\n", chunk_size, offset, start_col, start_row); */
    for(int i = 0; i < chunk_size; i++) {
        double sum = 0;
        for(int j = 0; j < mid; j++)
            sum += A_shared[row][j] * B_shared[j][col];

        C_shared[row][col] = sum;

        col++;
        if(col == nc) {
            col = 0;
            row++;
        }
    }

    return NULL;
}

pthread_t* create_n_threads(int n) {
    int fields = mc * nc;
    int default_chunk_size = fields / n;

    pthread_t* threads = malloc(n * sizeof(pthread_t));
    for(int i = 0; i < n; i++) {
        int chunk_size = i == n - 1 ? fields - (n-1) * default_chunk_size : default_chunk_size;
        int* args = malloc(3 * sizeof(int));
        args[0] = chunk_size;
        args[1] = i * default_chunk_size;
        args[2] = mid;
        pthread_create(&threads[i], NULL, calc_matrix_chunk, (void*)args);
    }
    
    return threads;
}

int main(int argc, char** argv) {

    int N = argc > 1 ? atoi(argv[1]) : 5;
    
    double **A;
    double **B;
    double **C;

    int ma, mb, na, nb;

    init_matrix(&A, &ma, &na, &B, &mb, &nb, &C);

    C_shared = malloc(ma*sizeof(double));
    for(int i = 0; i < ma; i++) {
        C_shared[i] = malloc(nb*sizeof(double));
    }
    mc = ma;
    nc = nb;
    mid = mb;
    printf("ncmcm sadkfj asf  mc %d  nc %d\n", mc, nc);

    A_shared = A; // do poprawy
    B_shared = B; // do poprawy

    pthread_t* threads = create_n_threads(N);

    for(int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    sleep(1);
    print_matrix(C_shared, mc, nc);

    // zwalnianie
    for(int i=0; i<na; i++) {
        free(A[i]);
    }
    free(A);

    for(int i=0; i<nb; i++) {
        free(B[i]);
    }
    free(B);

    for(int i=0; i<nb; i++) {
        free(C[i]);
    }
    free(C);

    return 0;
}
