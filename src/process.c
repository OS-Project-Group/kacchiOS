#include "process.h"
#include "memory.h"
#include "serial.h"

// Process Table Creation
pcb_t proctab[NPROC];

// Initially, no current process
pid32 currpid = -1;

// Next PID to assign
static pid32 next_pid = 1;

// Ready queue
static queue_t readylist = {-1, -1};

#define STACK_PER_PROC 512 // Process stack size

// Queue operations
void q_insert(int slot, queue_t *q)
{
    if (slot < 0 || slot >= NPROC || !q)
        return;

    proctab[slot].next = -1; // Mark end of queue

    if (q->head == -1) {
        // Queue empty
        q->head = slot;
        q->tail = slot;
    } else {
        // Add behind tail
        proctab[q->tail].next = slot;
        q->tail = slot;
    }
}

int q_remove(queue_t *q)
{
    int slot;

    if (!q || q->head == -1)
        return -1; // Queue empty

    slot = q->head;
    q->head = proctab[slot].next;

    if (q->head == -1)
        q->tail = -1; // Queue now empty

    proctab[slot].next = -1;
    return slot;
}

int q_empty(queue_t *q)
{
    return (q && q->head == -1);
}

// Enqueue a process to ready list
static void enqueue_ready(int slot)
{
    if (slot < 0 || slot >= NPROC)
        return;

    proctab[slot].prstate = PR_READY;
    q_insert(slot, &readylist);
}

// Returns next ready process PID
pid32 get_next_ready(void)
{
    if (q_empty(&readylist))
        return -1;

    return readylist.head;
}

// Initialize process table
void init_proctab(void)
{
    for (int i = 0; i < NPROC; i++)
    {
        proctab[i].pid = -1; 
        proctab[i].prstate = PR_FREE;
        proctab[i].prprio = 0;
        proctab[i].prstkptr = NULL;
        proctab[i].prstkbase = NULL;
    }
    next_pid = 1;
    readylist.head = -1;
    readylist.tail = -1;
    currpid = -1;
}

// Create a new process
// 3. Initialize PCB
// 4. Enqueue to ready queue
pid32 create_process(int priority)
{
    int i;
    char *stkbase;

    // 1. Find a free slot
    for (i = 0; i < NPROC; i++)
    {
        if (proctab[i].prstate == PR_FREE)
            break;
    }

    if (i == NPROC)
        return -1; // No free process slot

    // 2. Allocate kernel stack
    stkbase = (char *)heap_alloc(STACK_PER_PROC);
    if (!stkbase)
        return -1; // Memory allocation failed

    // 3. Initialize PCB
    proctab[i].pid = next_pid++;
    proctab[i].prstate = PR_READY;
    proctab[i].prprio = priority;
    proctab[i].prstkbase = stkbase;
    proctab[i].prstkptr = stkbase + STACK_PER_PROC - 4; // stack grows down
    proctab[i].next = -1;

    // 4. Enqueue to ready queue
    enqueue_ready(i);

    return proctab[i].pid;
}

// Set a process as currently running
void set_current(pid32 pid)
{
    int slot = find_slot(pid);
    int old_slot, i;
    queue_t temp;

    if (slot == -1)
        return; // Not found

    // Move old current back to READY and add to ready queue
    if (currpid != -1)
    {
        old_slot = find_slot(currpid);
        if (old_slot != -1 && proctab[old_slot].prstate == PR_CURR)
        {
            proctab[old_slot].prstate = PR_READY;
            q_insert(old_slot, &readylist);
        }
    }

    // Remove new current from ready queue
    if (proctab[slot].prstate == PR_READY)
    {
        // Rebuild queue without this process
        temp.head = -1;
        temp.tail = -1;
        while (!q_empty(&readylist))
        {
            i = q_remove(&readylist);
            if (i != slot)
                q_insert(i, &temp);
        }
        readylist = temp;
    }

    proctab[slot].prstate = PR_CURR;
    currpid = pid;
}

// Terminate a process
int terminate_process(pid32 pid)
{
    int slot = find_slot(pid);
    int i;
    queue_t temp;
    int state;

    if (slot == -1)
        return -1; // Not found

    state = get_process_state(pid);

    // Remove from ready queue if present
    if (state == PR_READY)
    {
        // Rebuild queue without this process
        temp.head = -1;
        temp.tail = -1;
        while (!q_empty(&readylist))
        {
            i = q_remove(&readylist);
            if (i != slot)
                q_insert(i, &temp);
        }
        readylist = temp;
    }

    // Free stack
    if (proctab[slot].prstkbase)
    {
        heap_free((void *)proctab[slot].prstkbase);
        proctab[slot].prstkbase = NULL;
        proctab[slot].prstkptr = NULL;
    }

    proctab[slot].prstate = PR_FREE;
    proctab[slot].pid = -1;
    proctab[slot].next = -1;

    if (currpid == pid)
        currpid = -1;

    return 0;
}

/* ============= UTILITY/ACCESSOR FUNCTIONS ============= */

// Get current process ID
pid32 getpid(void)
{
    return currpid;
}

// Find process table slot index by PID
int find_slot(pid32 pid)
{
    int i;
    for (i = 0; i < NPROC; i++)
    {
        if (proctab[i].pid == pid)
            return i;
    }
    return -1;  // Not found
}

// Get process state
int get_process_state(pid32 pid)
{
    int slot = find_slot(pid);
    if (slot == -1)
        return -1;  // Not found
    return proctab[slot].prstate;
}

// Get process priority
int get_process_priority(pid32 pid)
{
    int slot = find_slot(pid);
    if (slot == -1)
        return -1;  // Not found
    return proctab[slot].prprio;
}

// Check if PID is valid (process exists and is not free)
int is_valid_pid(pid32 pid)
{
    int slot = find_slot(pid);
    if (slot == -1)
        return 0;  // Not found
    return (proctab[slot].prstate != PR_FREE);
}

// Get process stack base
char* get_stack_base(pid32 pid)
{
    int slot = find_slot(pid);
    if (slot == -1)
        return NULL;  // Not found
    return proctab[slot].prstkbase;
}

// Get number of ready processes in queue
int get_num_ready(void)
{
    int count = 0;
    int slot = readylist.head;
    while (slot != -1)
    {
        count++;
        slot = proctab[slot].next;
    }
    return count;
}
