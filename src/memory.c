#include "memory.h"
#include "serial.h"

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

// Best fit heap allocation
void* heap_alloc(size_t size) {
    // 1. Alignment: Round size up to the nearest 4 bytes
    size = (size + 3) & ~3;

    MemBlock* curr = heap_head;
    MemBlock* best_block = NULL;

    // 2. Best-Fit Search
    while (curr) {
        if (curr->free && curr->size >= size) {
            if (best_block == NULL || curr->size < best_block->size) {
                best_block = curr;
            }
            if (curr->size == size) break; // Perfect match found
        }
        curr = curr->next;
    }

    if (best_block) {
        // 3. Splitting Logic with Minimum Fragment Check
        // Only split if we can fit a new header AND at least 4 bytes of data
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
        // Return pointer to data (skipping the header)
        return (void*)((uint8_t*)best_block + sizeof(MemBlock));
    }

    return NULL;
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

// stress test
void stress_test_memory(void) {
    serial_puts("\n--- Starting KacchiOS Memory Stress Test ---\n");

    /* Phase 1: Stack Deallocation Test */
    serial_puts("Testing Stack...\n");
    void* s1 = stack_alloc(100);
    if (s1) {
        serial_puts("  Allocated 100 bytes on stack.\n");
        stack_free(100); // Your new function
        serial_puts("  Deallocated 100 bytes. Stack OK.\n");
    }

    /* Phase 2: Heap Fragmentation & Merging Test */
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

    // Create fragmentation by freeing them
    // Your optimized logic should merge these back into one ~1536 byte block
    serial_puts("  Freeing all blocks to trigger coalescing...\n");
    heap_free(p1);
    heap_free(p2);
    heap_free(p3);

    /* Phase 3: Verification of Merging */
    // If merging failed, the largest free block is only 512 bytes.
    // If your code works, a 1024-byte block will now fit easily.
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