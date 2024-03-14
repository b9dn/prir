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
    int start_field;
    int fields_num;
    int thread_id;
} mlt_thread_args_t;

matrix_t matrix_A;
matrix_t matrix_B;
matrix_t matrix_C;
double squares_sum;
double frobenius_norm;
pthread_mutex_t lock;


void check_malloc(void* ptr) {
    if(ptr == NULL) {
        printf("Blad malloc\n");
        exit(EXIT_FAILURE);
    }
}

void print_matrix(matrix_t matrix) {
    printf("[\n");
    for(int i = 0; i< matrix.rows; i++)
    {
        for(int j = 0; j < matrix.cols; j++)
        {
            printf("%f ", matrix.data[i][j]);
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

    matrix->data = malloc(matrix->rows * sizeof(double));
    check_malloc(matrix->data);
    for(int i = 0; i < matrix->rows; i++) {
        matrix->data[i] = malloc(matrix->cols * sizeof(double));
        check_malloc(matrix->data[i]);
    }

    for(int row_ptr = 0; row_ptr < matrix->rows; row_ptr++) {
        for(int col_ptr = 0; col_ptr < matrix->cols; col_ptr++) {
            fscanf( fp, "%lf", &num );
            matrix->data[row_ptr][col_ptr] = num;
        }
    }

    fclose(fp);
}

void init(int threads_num) {
    read_matrix("A.txt", &matrix_A);
    read_matrix("B.txt", &matrix_B);

    if(matrix_A.cols != matrix_B.rows) {
        printf("Złe wymiary macierzy!\n");
        exit(-1);
    }

    printf("Wczytana macierz A:\n");
    print_matrix(matrix_A);

    printf("Wczytana macierz B:\n");
    print_matrix(matrix_B);

    matrix_C.rows = matrix_A.rows;
    matrix_C.cols = matrix_B.cols;
    matrix_C.data = malloc(matrix_A.rows * sizeof(double));
    check_malloc(matrix_C.data);
    for(int i = 0; i < matrix_A.rows; i++) {
        matrix_C.data[i] = malloc(matrix_B.cols * sizeof(double));
        check_malloc(matrix_C.data[i]);
    }

    if(pthread_mutex_init(&lock, NULL) != 0) { 
        printf("Blad w mutex init\n"); 
        exit(EXIT_FAILURE);
    }

    squares_sum = 0;
    frobenius_norm = 0;
}

void free_data() {
    for(int i = 0; i < matrix_A.rows; i++) {
        free(matrix_A.data[i]);
    }
    free(matrix_A.data);

    for(int i = 0; i < matrix_B.rows; i++) {
        free(matrix_B.data[i]);
    }
    free(matrix_B.data);

    for(int i = 0; i < matrix_C.rows; i++) {
        free(matrix_C.data[i]);
    }
    free(matrix_C.data);

    pthread_mutex_destroy(&lock);
}

void* calc_matrix_chunk(void* args) {
    mlt_thread_args_t* targs = (mlt_thread_args_t*)args;
    int start_field = targs->start_field;
    int fields_num = targs->fields_num;

    double sqres_sum = 0;
    for(int i = start_field; i < start_field + fields_num; i++) {
        int row = i / matrix_B.cols;
        int col = i % matrix_B.cols;
        double sum = 0;
        for(int j = 0; j < matrix_A.cols; j++) {
            sum += matrix_A.data[row][j] * matrix_B.data[j][col];
        }
        matrix_C.data[row][col] = sum;
        sqres_sum += sum * sum;
    }

    pthread_mutex_lock(&lock);
    squares_sum += sqres_sum;
    pthread_mutex_unlock(&lock);

    free(targs);
    return NULL;
}

pthread_t* calc_mlt_matrix(int threads_num) {
    int fields = matrix_C.rows * matrix_C.cols;
    int default_chunk_size = fields / threads_num;
    int remainder = fields % threads_num;
    int ptr = 0;
    pthread_t* threads = malloc(threads_num * sizeof(pthread_t));
    check_malloc(threads);
    for(int i = 0; i < threads_num; i++) {
        int chunk_size = remainder-- > 0 ? default_chunk_size + 1 : default_chunk_size;
        
        mlt_thread_args_t* args = malloc(sizeof(mlt_thread_args_t));
        check_malloc(args);
        args->start_field = ptr;
        args->fields_num = chunk_size;
        args->thread_id = i;

        pthread_create(&threads[i], NULL, calc_matrix_chunk, (void*)args);
        ptr += chunk_size;
    }
    
    return threads;
}

int main(int argc, char** argv) {
    int N = argc > 1 ? atoi(argv[1]) : 5;

    init(N);

    pthread_t* mlt_threads = calc_mlt_matrix(N);

    for(int i = 0; i < N; i++) {
        pthread_join(mlt_threads[i], NULL);
    }
    frobenius_norm = sqrt(squares_sum);

    printf("Obliczona macierz C:\n");
    print_matrix(matrix_C);

    printf("Norma frobeniusa: %lf\n", frobenius_norm);

    free(mlt_threads);
    free_data();
}

