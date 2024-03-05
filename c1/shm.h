#ifndef SHM_H
#define SHM_H

void* create_shared_memory(int size);
void free_shared_memory(void* ptr, int size);

#endif