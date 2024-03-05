#include "file.h"
#include <stdio.h>
#include <stdlib.h>

void write_nums_to_file(int *nums, int size, char *filename, char* line_prefix) {
    FILE *file = fopen(filename, "w");
    if(file == NULL) {
        fprintf(stderr, "Blad w fopen");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < size; i++) {
        fprintf(file, "%s %d: %d\n", line_prefix, i, nums[i]);
    }
    fclose(file);
}