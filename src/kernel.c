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
    
    /* Test 1: Create processes */
    int test1_pass = 1;
    pid32 p1 = create_process(1);
    pid32 p2 = create_process(2);
    pid32 p3 = create_process(3);
    
    if (p1 == -1 || p2 == -1 || p3 == -1)
        test1_pass = 0;

    /* Test 2: Verify process states using utility functions */
    int test2_pass = 1;
    if (get_process_state(p1) != 1)  // PR_READY = 1
        test2_pass = 0;
    if (get_process_state(p2) != 1)
        test2_pass = 0;
    if (get_process_state(p3) != 1)
        test2_pass = 0;

    /* Test 3: Verify process priorities */
    int test3_pass = 1;
    if (get_process_priority(p1) != 1)
        test3_pass = 0;
    if (get_process_priority(p2) != 2)
        test3_pass = 0;
    if (get_process_priority(p3) != 3)
        test3_pass = 0;

    /* Test 4: Verify ready queue count */
    int test4_pass = (get_num_ready() == 3) ? 1 : 0;

    /* Test 5: Verify valid PIDs */
    int test5_pass = 1;
    if (!is_valid_pid(p1) || !is_valid_pid(p2) || !is_valid_pid(p3))
        test5_pass = 0;

    /* Test 6: State transition (READY -> RUNNING) */
    int test6_pass = 1;
    pid32 next = get_next_ready();
    if (next == -1)
        test6_pass = 0;
    else {
        /* next is actually a slot index, need to convert to PID */
        int next_slot = next;
        pid32 next_pid = proctab[next_slot].pid;
        set_current(next_pid);
        if (get_process_state(p1) != 2)  // PR_CURR = 2
            test6_pass = 0;
        if (getpid() != p1)
            test6_pass = 0;
    }

    /* Test 7: Ready queue after transition */
    int test7_pass = (get_num_ready() == 2) ? 1 : 0;

    /* Test 8: Process termination */
    int test8_pass = 1;
    if (terminate_process(p1) != 0)
        test8_pass = 0;
    if (is_valid_pid(p1))  // Should be invalid now
        test8_pass = 0;
    /* After termination, PID is cleared, so we can't look it up - just check validity */

    /* Test 9: Ready queue after termination */
    int test9_pass = (get_num_ready() == 2) ? 1 : 0;

    /* Test 10: Stack allocation verification */
    int test10_pass = 1;
    if (get_stack_base(p2) == NULL)
        test10_pass = 0;
    if (get_stack_base(p1) != NULL)  // Terminated process should have NULL
        test10_pass = 0;

    /* Print results */
    serial_puts("\n========================================\n");
    serial_puts("    Process Manager Utility Tests\n");
    serial_puts("========================================\n\n");

    serial_puts("Test 1 (Process Creation): ");
    serial_puts(test1_pass ? "PASS\n" : "FAIL\n");

    serial_puts("Test 2 (Process States): ");
    serial_puts(test2_pass ? "PASS\n" : "FAIL\n");

    serial_puts("Test 3 (Process Priorities): ");
    serial_puts(test3_pass ? "PASS\n" : "FAIL\n");

    serial_puts("Test 4 (Ready Queue Count): ");
    serial_puts(test4_pass ? "PASS\n" : "FAIL\n");

    serial_puts("Test 5 (Valid PID Check): ");
    serial_puts(test5_pass ? "PASS\n" : "FAIL\n");

    serial_puts("Test 6 (State Transition): ");
    serial_puts(test6_pass ? "PASS\n" : "FAIL\n");

    serial_puts("Test 7 (Queue After Transition): ");
    serial_puts(test7_pass ? "PASS\n" : "FAIL\n");

    serial_puts("Test 8 (Process Termination): ");
    serial_puts(test8_pass ? "PASS\n" : "FAIL\n");

    serial_puts("Test 9 (Queue After Termination): ");
    serial_puts(test9_pass ? "PASS\n" : "FAIL\n");

    serial_puts("Test 10 (Stack Allocation): ");
    serial_puts(test10_pass ? "PASS\n" : "FAIL\n");

    /* Overall result */
    int all_pass = test1_pass && test2_pass && test3_pass && test4_pass && 
                   test5_pass && test6_pass && test7_pass && test8_pass && 
                   test9_pass && test10_pass;

    serial_puts("\n");
    serial_puts(all_pass ? "All tests PASSED!\n" : "Some tests FAILED!\n");
    serial_puts("========================================\n\n");

    /* IPC (Inter-Process Communication) Tests */
    serial_puts("========================================\n");
    serial_puts("    IPC (Inter-Process Communication)\n");
    serial_puts("========================================\n\n");

    /* Create two new processes for IPC testing */
    pid32 sender = create_process(1);
    pid32 receiver = create_process(1);
    
    int ipc_test1 = (sender != -1 && receiver != -1) ? 1 : 0;
    serial_puts("Test IPC-1 (Process Creation): ");
    serial_puts(ipc_test1 ? "PASS\n" : "FAIL\n");

    /* Set sender as current process before sending */
    set_current(sender);

    /* Test send message */
    char test_msg[] = "Hello IPC!";
    int send_result = send(receiver, test_msg, 10);
    int ipc_test2 = (send_result == 0) ? 1 : 0;
    serial_puts("Test IPC-2 (Send Message): ");
    serial_puts(ipc_test2 ? "PASS\n" : "FAIL\n");

    /* Test message was set in receiver's inbox */
    int receiver_slot = find_slot(receiver);
    int msg_received = proctab[receiver_slot].has_msg;
    int ipc_test3 = (msg_received == 1) ? 1 : 0;
    serial_puts("Test IPC-3 (Message Available): ");
    serial_puts(ipc_test3 ? "PASS\n" : "FAIL\n");

    /* Test message sender field is correct */
    int sender_correct = (proctab[receiver_slot].msg_inbox.sender_pid == sender);
    int ipc_test4 = sender_correct ? 1 : 0;
    serial_puts("Test IPC-4 (Sender Identification): ");
    serial_puts(ipc_test4 ? "PASS\n" : "FAIL\n");

    /* Test message length is correct */
    int msg_len = proctab[receiver_slot].msg_inbox.len;
    int ipc_test5 = (msg_len == 10) ? 1 : 0;
    serial_puts("Test IPC-5 (Message Length): ");
    serial_puts(ipc_test5 ? "PASS\n" : "FAIL\n");

    /* Test message content */
    int content_match = 1;
    for (int i = 0; i < 10; i++) {
        if (proctab[receiver_slot].msg_inbox.data[i] != test_msg[i])
            content_match = 0;
    }
    int ipc_test6 = content_match ? 1 : 0;
    serial_puts("Test IPC-6 (Message Content): ");
    serial_puts(ipc_test6 ? "PASS\n" : "FAIL\n");

    /* Now test receive() function with src_pid parameter */
    /* First reset the message and set receiver as current */
    proctab[receiver_slot].has_msg = 1;  /* Reset has_msg */
    set_current(receiver);  /* Set receiver as current process */
    
    char rcv_buffer[128];
    int rcv_result = receive(sender, rcv_buffer, 128);
    int ipc_test7 = (rcv_result == 10) ? 1 : 0;
    serial_puts("Test IPC-7 (Receive with Sender Check): ");
    serial_puts(ipc_test7 ? "PASS\n" : "FAIL\n");

    /* Overall IPC result */
    int ipc_all_pass = ipc_test1 && ipc_test2 && ipc_test3 && ipc_test4 && 
                       ipc_test5 && ipc_test6 && ipc_test7;

    serial_puts("\n");
    serial_puts(ipc_all_pass ? "All IPC tests PASSED!\n" : "Some IPC tests FAILED!\n");
    serial_puts("========================================\n\n");

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
