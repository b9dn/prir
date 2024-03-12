#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

typedef struct {
    double** data;
    int rows;
    int cols;
} matrix_t;

typedef struct {
    matrix_t matrix_A;
    matrix_t matrix_B;
    matrix_t* matrix_C;
    int start_field;
    int fields_num;
    int thread_id;
} mlt_thread_args_t;

typedef struct {
    matrix_t matrix;
    int start_field;
    int fields_num;   
    double* result; 
} frbns_thread_args_t;

void print_matrix(double**A, int m, int n) {
    printf("[\n");
    for(int i = 0; i< m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }
    printf("]\n\n");
}

void read_matrix(char* file_name, matrix_t* matrix) {
    FILE *fp;
    double num;

    fp = fopen(file_name, "r");
    if( fp == NULL ) {
        perror("Blad otwarcia pliku");
        exit(-10);
    }

    fscanf (fp, "%d", &matrix->rows);
    fscanf (fp, "%d", &matrix->cols);

    double **A = malloc(matrix->rows * sizeof(double));
    for(int i = 0; i < matrix->rows; i++) {
        A[i] = malloc(matrix->cols * sizeof(double));
    }

    for(int row_ptr = 0; row_ptr < matrix->rows; row_ptr++) {
        for(int col_ptr = 0; col_ptr < matrix->cols; col_ptr++) {
            fscanf( fp, "%lf", &num );
            A[row_ptr][col_ptr] = num;
        }
    }

    matrix->data = A;
    fclose(fp);
}


void init_matrixes(matrix_t* matrix_A, matrix_t* matrix_B, matrix_t* matrix_C) {
    read_matrix("A.txt", matrix_A);
    read_matrix("B.txt", matrix_B);

    if(matrix_A->cols != matrix_B->rows) {
        printf("Złe wymiary macierzy!\n");
        exit(-1);
    }

    printf("Wczytana macierz A:\n");
    print_matrix(matrix_A->data, matrix_A->rows, matrix_A->cols);

    printf("Wczytana macierz B:\n");
    print_matrix(matrix_B->data, matrix_B->rows, matrix_B->cols);

    matrix_C->rows = matrix_A->rows;
    matrix_C->cols = matrix_B->cols;
    matrix_C->data = malloc(matrix_A->rows * sizeof(double));
    for(int i = 0; i < matrix_A->rows; i++) {
        matrix_C->data[i] = malloc(matrix_B->cols * sizeof(double));
    }
}

void free_matrix(matrix_t* matrix) {
    for(int i = 0; i < matrix->rows; i++) {
        free(matrix->data[i]);
    }
    free(matrix->data);
}

void* calc_matrix_chunk(void* args) {
    mlt_thread_args_t* targs = (mlt_thread_args_t*)args;
    int start_field = targs->start_field;
    int fields_num = targs->fields_num;
    double** A = targs->matrix_A.data;
    double** B = targs->matrix_B.data;
    double** C = targs->matrix_C->data;

    for(int i = start_field; i < start_field + fields_num; i++) {
        int row = i / targs->matrix_B.cols;
        int col = i % targs->matrix_B.cols;
        double sum = 0;
        for(int j = 0; j < targs->matrix_A.cols; j++) {
            sum += A[row][j] * B[j][col];
        }
        C[row][col] = sum;
    }

    return NULL;
}

pthread_t* calc_mlt_matrix(int threads_num, matrix_t matrix_A, matrix_t matrix_B, matrix_t* matrix_C) {
    int fields = matrix_C->rows * matrix_C->cols;
    int default_chunk_size = fields / threads_num;
    int remainder = fields % threads_num;
    int ptr = 0;
    pthread_t* threads = malloc(threads_num * sizeof(pthread_t));
    for(int i = 0; i < threads_num; i++) {
        int chunk_size = remainder-- > 0 ? default_chunk_size + 1 : default_chunk_size;
        
        mlt_thread_args_t* args = malloc(sizeof(mlt_thread_args_t));
        args->matrix_A = matrix_A;
        args->matrix_B = matrix_B;
        args->matrix_C = matrix_C;
        args->start_field = ptr;
        args->fields_num = chunk_size;
        args->thread_id = i;

        pthread_create(&threads[i], NULL, calc_matrix_chunk, (void*)args);
        ptr += chunk_size;
    }
    
    return threads;
}

void* calc_frbns_norm_chunk(void* args) {
    frbns_thread_args_t* targs = (frbns_thread_args_t*)args;
    int start_field = targs->start_field;
    int fields_num = targs->fields_num;
    int cols = targs->matrix.cols;
    double** matrix = targs->matrix.data;
    double* result = targs->result;

    double sum = 0;
    for(int i = start_field; i < start_field + fields_num; i++) {
        int row = i / cols;
        int col = i % cols;
        sum += pow(matrix[row][col], 2);
    }

    *result = sum;
    return NULL;
}

pthread_t* calc_frbns_norm(int threads_num, double* partial_square_sums, matrix_t matrix) {
    int matrix_size = matrix.rows * matrix.cols;
    int default_chunk_size = matrix_size / threads_num;
    int remainder = matrix_size % threads_num;
    int ptr = 0;
    pthread_t* threads = malloc(threads_num * sizeof(pthread_t));
    for(int i = 0; i < threads_num; i++) {
        int chunk_size = remainder-- > 0 ? default_chunk_size + 1 : default_chunk_size;
        
        frbns_thread_args_t* args = malloc(sizeof(frbns_thread_args_t));
        args->matrix = matrix;
        args->start_field = ptr;
        args->fields_num = chunk_size;
        args->result = &partial_square_sums[i];
        
        pthread_create(&threads[i], NULL, calc_frbns_norm_chunk, (void*)args);
        ptr += chunk_size;
    }
    
    return threads;
}

int main(int argc, char** argv) {
    int N = argc > 1 ? atoi(argv[1]) : 5;

    matrix_t matrix_A;
    matrix_t matrix_B;
    matrix_t matrix_C;

    init_matrixes(&matrix_A, &matrix_B, &matrix_C);

    pthread_t* mlt_threads = calc_mlt_matrix(N, matrix_A, matrix_B, &matrix_C);
    for(int i = 0; i < N; i++) {
        pthread_join(mlt_threads[i], NULL);
    }
    printf("Obliczona macierz C:\n");
    print_matrix(matrix_C.data, matrix_C.rows, matrix_C.cols);

    double* partial_square_sums = malloc(N * sizeof(double));
    pthread_t* frbns_threads = calc_frbns_norm(N, partial_square_sums, matrix_C);
    for(int i = 0; i < N; i++) {
        pthread_join(frbns_threads[i], NULL);
    }
    double total_sum = 0;
    printf("Czesciowe sumy kwadratow:\n[ ");
    for(int i = 0; i < N; i++) {
        total_sum += partial_square_sums[i];
        printf("%f ", partial_square_sums[i]);
    }
    printf("]\nSuma kwadratow: %f\n", total_sum);
    printf("Norma Frobeniusa: %f\n", sqrt(total_sum));

    free_matrix(&matrix_A);
    free_matrix(&matrix_B);
    free_matrix(&matrix_C);
}
