#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

#define STACK_SIZE 4096
#define HEAP_SIZE  8192

extern uint8_t stack[STACK_SIZE];
extern uint8_t heap[HEAP_SIZE];

// Stack allocation
void* stack_alloc(size_t size);

// Heap allocation
void* heap_alloc(size_t size);
void heap_free(void* ptr);

// Memory block structure for heap
typedef struct MemBlock {
    size_t size;
    int free;
    struct MemBlock* next;
} MemBlock;

void memory_init(void);

#endif