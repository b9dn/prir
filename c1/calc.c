#include "calc.h"
#include <stdlib.h>
#include <stdio.h>

void calc_range_array(int* arr, int n, int z) {
    int default_chunk_size = z / n;
    int reminder = z % n;
    int ptr = 0;
    for(int i = 0; i < n; i++) {
        int chunk_size = reminder-- > 0 ? default_chunk_size + 1 : default_chunk_size;
        arr[i*2] = ptr;
        arr[i*2+1] = ptr + chunk_size - 1;
        ptr += chunk_size;
        printf("range segment %d: %d - %d\n", i, arr[i*2], arr[i*2+1]);
    }
}

int calc_sum_in_range(int* arr, int start, int end) {
    int sum = 0;
    for(int i = start; i <= end; i++)
        sum += arr[i];
    return sum;
}

int calc_sum(int* arr, int size) {
    int sum = 0;
    for(int i = 0; i < size; i++)
        sum += arr[i];
    return sum;
}