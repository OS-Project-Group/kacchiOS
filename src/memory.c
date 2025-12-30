#include "memory.h"

uint8_t stack[STACK_SIZE];
uint8_t heap[HEAP_SIZE];

static size_t stack_top = 0;

// Heap block management
static MemBlock* heap_head = NULL;

void memory_init(void) {
    heap_head = (MemBlock*)heap;
    heap_head->size = HEAP_SIZE - sizeof(MemBlock);
    heap_head->free = 1;
    heap_head->next = NULL;
}

// Stack allocation (no free, just bump pointer)
void* stack_alloc(size_t size) {
    if (stack_top + size > STACK_SIZE)
        return NULL;
    void* ptr = &stack[stack_top];
    stack_top += size;
    return ptr;
}

//stack deallocation
void stack_free(size_t size) {
    if (size > stack_top) {
        stack_top = 0;
    }
    else {
        stack_top -= size;
    }
}

// First-fit heap allocation
void* heap_alloc(size_t size) {
    MemBlock* curr = heap_head;
    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + sizeof(MemBlock)) {
                // Split block
                MemBlock* next = (MemBlock*)((uint8_t*)curr + sizeof(MemBlock) + size);
                next->size = curr->size - size - sizeof(MemBlock);
                next->free = 1;
                next->next = curr->next;
                curr->size = size;
                curr->next = next;
            }
            curr->free = 0;
            return (void*)((uint8_t*)curr + sizeof(MemBlock));
        }
        curr = curr->next;
    }
    return NULL; // No suitable block found
}

// Free heap block
void heap_free(void* ptr) {
    if (!ptr) return;

    // Locate the header exactly like the original code 
    MemBlock* block = (MemBlock*)((uint8_t*)ptr - sizeof(MemBlock));
    block->free = 1;

    //Forward Merge
    if (block->next && block->next->free) {
        block->size += sizeof(MemBlock) + block->next->size;
        block->next = block->next->next;
    }

    //Global Coalesce 
    MemBlock* curr = heap_head;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(MemBlock) + curr->next->size;
            curr->next = curr->next->next;
        }
        else {
            // Since memory addresses increase, if 'curr' has passed 'block',
            // and no more adjacent free blocks were found, we can stop.
            if (curr > block) break;
            curr = curr->next;
        }
    }
}