#include "shm.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

void* create_shared_memory(int size) {
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    int *shm_seg = mmap(NULL, size, protection, visibility, -1, 0);
    if(shm_seg == MAP_FAILED) {
        fprintf(stderr, "Blad w mmap");
        exit(EXIT_FAILURE);
    }
    return shm_seg;
}

void free_shared_memory(void* ptr, int size) {
    if(munmap(ptr, size) == -1) {
        fprintf(stderr, "Blad w munmap");
        exit(EXIT_FAILURE);
    }
}