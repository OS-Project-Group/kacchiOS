#include "memory.h"

uint8_t stack[STACK_SIZE];
uint8_t heap[HEAP_SIZE];

static size_t stack_top = 0;

// heap block 
static MemBlock* heap_head = NULL;

void memory_init(void) {
    heap_head = (MemBlock*)heap;
    heap_head->size = HEAP_SIZE - sizeof(MemBlock);
    heap_head->free = 1;
    heap_head->next = NULL;
}

// stack allocation (no free, just bump pointer)
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

// best fit heap
void* heap_alloc(size_t size) {
    // Alignment
    size = (size + 3) & ~3;

    MemBlock* curr = heap_head;
    MemBlock* best_block = NULL;

    //Best-Fit Search
    while (curr) {
        if (curr->free && curr->size >= size) {
            if (best_block == NULL || curr->size < best_block->size) {
                best_block = curr;
            }
            if (curr->size == size) break;
        }
        curr = curr->next;
    }

    if (best_block) {
        // Splitting Logic Only split if we can fit a new header AND at least 4 bytes of data
    
        size_t min_split_size = sizeof(MemBlock) + 4;

        if (best_block->size >= size + min_split_size) {
            MemBlock* next = (MemBlock*)((uint8_t*)best_block + sizeof(MemBlock) + size);
            next->size = best_block->size - size - sizeof(MemBlock);
            next->free = 1;
            next->next = best_block->next;

            best_block->size = size;
            best_block->next = next;
        }

        best_block->free = 0;
        // Return pointer to data 
        return (void*)((uint8_t*)best_block + sizeof(MemBlock));
    }

    return NULL;
}

// free heap block
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
            // no need for checking
            if (curr > block) break;
            curr = curr->next;
        }
    }
}

// stress test
void stress_test_memory(void) {
    serial_puts("\n--- Starting KacchiOS Memory Stress Test ---\n");

    /* Stack Deallocation Test */
    serial_puts("Testing Stack...\n");
    void* s1 = stack_alloc(100);
    if (s1) {
        serial_puts("  Allocated 100 bytes on stack.\n");
        stack_free(100); 
        serial_puts("  Deallocated 100 bytes. Stack OK.\n");
    }

    /* Heap Fragmentation & Merging Test */
    serial_puts("Testing Heap Merging (Coalescing)...\n");

    // Allocate 3 blocks to fill space
    void* p1 = heap_alloc(512);
    void* p2 = heap_alloc(512);
    void* p3 = heap_alloc(512);

    if (!p1 || !p2 || !p3) {
        serial_puts("  Initial heap allocation FAILED!\n");
        return;
    }
    serial_puts("  Allocated three 512-byte blocks.\n");

    // create fragmentation by freeing them

    serial_puts("  Freeing all blocks to trigger coalescing...\n");
    heap_free(p1);
    heap_free(p2);
    heap_free(p3);

    /* Verification of Merging */
    void* big_p = heap_alloc(1024);
    if (big_p) {
        serial_puts("  SUCCESS: 1024-byte block allocated after merging!\n");
        heap_free(big_p);
    }
    else {
        serial_puts("  FAILURE: Heap is still fragmented. Merge failed.\n");
    }

    serial_puts("--- Stress Test Complete ---\n\n");
    // Now we will start deallocating from the end of the heap
}