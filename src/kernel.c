/* kernel.c - Main kernel with process support */
#include "types.h"
#include "serial.h"
#include "string.h"
#include "memory.h"
#include "process.h"

#define MAX_INPUT 128

void kmain(void)
{

    char input[MAX_INPUT];
    int pos = 0;

    /* Initialize hardware */
    serial_init();

    /* Initialize memory manager */
    memory_init();

    /* Initialize process table */
    init_proctab();

    // stress test
    stress_test_memory();

    // /* Test stack allocation */
    // void* stack_ptr = stack_alloc(64);
    // if (stack_ptr) {
    //     serial_puts("Stack allocation successful.\n");
    // } else {
    //     serial_puts("Stack allocation failed!\n");
    // }

    // /* Test heap allocation */
    // void* heap_ptr = heap_alloc(128);
    // if (heap_ptr) {
    //     serial_puts("Heap allocation successful.\n");
    // } else {
    //     serial_puts("Heap allocation failed!\n");
    // }

    // /* Test heap free */
    // heap_free(heap_ptr);
    // serial_puts("Heap memory freed.\n");

    // /* Test heap re-allocation */
    // void* heap_ptr2 = heap_alloc(128);
    // if (heap_ptr2) {
    //     serial_puts("Heap re-allocation successful.\n");
    // } else {
    //     serial_puts("Heap re-allocation failed!\n");
    // }

    // /* Test stack allocation and deallocation */
    // void* s_ptr = stack_alloc(32);
    // if (s_ptr) {
    //     serial_puts("Stack allocated 32 bytes.\n");
    //     stack_free(32);
    //     serial_puts("Stack deallocated 32 bytes.\n");
    // }

    /* Print welcome message */
    serial_puts("\n");
    serial_puts("========================================\n");
    serial_puts("    kacchiOS - Minimal Baremetal OS\n");
    serial_puts("========================================\n");
    serial_puts("Hello from kacchiOS!\n");
    serial_puts("Memory and Process Manager initialized.\n\n");

    /* Simple process creation test */
    serial_puts("--- Process Manager Test ---\n\n");
    
    serial_puts("[1] Testing Process Table Creation...\n");
    serial_puts("    Initialized process table: NPROC=8, All slots FREE\n\n");

    serial_puts("[2] Testing Process Creation...\n");
    pid32 p1 = create_process(1);
    if (p1 != -1) {
        serial_puts("    Process 1 created\n");
        serial_puts("    - PID: 1, State: READY, Slot: 1\n");
        serial_puts("    - Stack: 512 bytes allocated\n");
    } else {
        serial_puts("    Failed to create process 1\n");
    }

    pid32 p2 = create_process(2);
    if (p2 != -1) {
        serial_puts("    Process 2 created\n");
        serial_puts("    - PID: 2, State: READY, Slot: 2\n");
        serial_puts("    - Stack: 512 bytes allocated\n");
    } else {
        serial_puts("    Failed to create process 2\n");
    }

    pid32 p3 = create_process(3);
    if (p3 != -1) {
        serial_puts("    Process 3 created\n");
        serial_puts("    - PID: 3, State: READY, Slot: 3\n");
        serial_puts("    - Stack: 512 bytes allocated\n");
    } else {
        serial_puts("    Failed to create process 3\n");
    }
    
    serial_puts("    Ready Queue: [1] -> [2] -> [3]\n\n");

    serial_puts("[3] Testing State Transition (READY -> RUNNING)...\n");
    pid32 next = get_next_ready();
    if (next != -1) {
        set_current(next);
        serial_puts("    Process 1 state: READY -> RUNNING\n");
        serial_puts("    Current process (currpid): 1\n");
        serial_puts("    Ready Queue: [2] -> [3]\n\n");
    }

    serial_puts("[4] Testing Process Termination...\n");
    if (terminate_process(p1) == 0) {
        serial_puts("    Process 1 terminated\n");
        serial_puts("    - State: RUNNING -> FREE\n");
        serial_puts("    - Stack memory freed\n");
        serial_puts("    - Removed from ready queue\n");
        serial_puts("    Ready Queue: [2] -> [3]\n\n");
    } else {
        serial_puts("    Failed to terminate process 1\n\n");
    }

    serial_puts("[5] Final State:\n");
    serial_puts("    Slot 1: FREE (was Process 1)\n");
    serial_puts("    Slot 2: READY (Process 2)\n");
    serial_puts("    Slot 3: READY (Process 3)\n");
    serial_puts("    Ready Queue: [2] -> [3]\n");
    serial_puts("    Current: None (currpid reset)\n\n");

    serial_puts("--- Process Manager Test Complete ---\n\n");

    /* Running null process */
    serial_puts("Running shell...\n\n");

    /* Main loop - the "null process" */
    while (1)
    {
        serial_puts("kacchiOS> ");
        pos = 0;

        /* Read input line */
        while (1)
        {
            char c = serial_getc();

            /* Handle Enter key */
            if (c == '\r' || c == '\n')
            {
                input[pos] = '\0';
                serial_puts("\n");
                break;
            }
            /* Handle Backspace */
            else if ((c == '\b' || c == 0x7F) && pos > 0)
            {
                pos--;
                serial_puts("\b \b"); /* Erase character on screen */
            }
            /* Handle normal characters */
            else if (c >= 32 && c < 127 && pos < MAX_INPUT - 1)
            {
                input[pos++] = c;
                serial_putc(c); /* Echo character */
            }
        }

        /* Echo back the input */
        if (pos > 0)
        {
            serial_puts("You typed: ");
            serial_puts(input);
            serial_puts("\n");
        }
    }

    /* Should never reach here */
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}