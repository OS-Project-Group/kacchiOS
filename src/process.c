#include "process.h"
#include "memory.h"
#include "serial.h"

/* Global process table */
pcb_t proctab[NPROC];

/* Current running process */
pid32 currpid = -1;

/* PID allocator */
static pid32 next_pid = 1;

/* Ready queue (simple linked list) */
static pid32 ready_head = -1;

#define STACK_PER_PROC 512 /* 512 bytes per process stack */

/* Helper: enqueue process to ready queue */
static void enqueue_ready(pid32 pid)
{
    if (pid < 0 || pid >= NPROC)
        return;

    proctab[pid].prstate = PR_READY;

    /* Simple FIFO: append to end */
    if (ready_head == -1)
    {
        ready_head = pid;
    }
}

/* Helper: get next ready process (simple - just find first READY) */
pid32 get_next_ready(void)
{
    for (int i = 0; i < NPROC; i++)
    {
        if (proctab[i].prstate == PR_READY)
        {
            return i;
        }
    }
    return -1;
}

/* Initialize process table */
void init_proctab(void)
{
    for (int i = 0; i < NPROC; i++)
    {
        proctab[i].pid = -1; /* Invalid until allocated */
        proctab[i].prstate = PR_FREE;
        proctab[i].prprio = 0;
        proctab[i].prstkptr = NULL;
        proctab[i].prstkbase = NULL;
    }
    next_pid = 1;
    ready_head = -1;
    currpid = -1;
}

/* Find a free slot and create process */
pid32 create_process(int priority)
{
    int i;
    char *stkbase;

    /* Find a free slot */
    for (i = 0; i < NPROC; i++)
    {
        if (proctab[i].prstate == PR_FREE)
            break;
    }

    if (i == NPROC)
        return -1; /* No free process slot */

    /* Allocate kernel stack */
    stkbase = (char *)heap_alloc(STACK_PER_PROC);
    if (!stkbase)
        return -1; /* Memory allocation failed */

    /* Initialize PCB */
    proctab[i].pid = next_pid++;
    proctab[i].prstate = PR_READY;
    proctab[i].prprio = priority;
    proctab[i].prstkbase = stkbase;
    proctab[i].prstkptr = stkbase + STACK_PER_PROC - 4; /* stack grows down */

    /* Enqueue to ready queue */
    enqueue_ready(i);

    return proctab[i].pid;
}

/* Set a process as currently running */
void set_current(pid32 pid)
{
    int slot;

    /* Find slot by pid */
    for (slot = 0; slot < NPROC; slot++)
    {
        if (proctab[slot].pid == pid)
            break;
    }

    if (slot == NPROC)
        return; /* Not found */

    /* Move old current back to READY */
    if (currpid != -1)
    {
        int old_slot;
        for (old_slot = 0; old_slot < NPROC; old_slot++)
        {
            if (proctab[old_slot].pid == currpid && proctab[old_slot].prstate == PR_CURR)
            {
                proctab[old_slot].prstate = PR_READY;
                break;
            }
        }
    }

    proctab[slot].prstate = PR_CURR;
    currpid = pid;
}

/* Terminate a process */
int terminate_process(pid32 pid)
{
    int slot;

    /* Find slot by pid */
    for (slot = 0; slot < NPROC; slot++)
    {
        if (proctab[slot].pid == pid)
            break;
    }

    if (slot == NPROC)
        return -1; /* Not found */

    /* Free stack */
    if (proctab[slot].prstkbase)
    {
        heap_free((void *)proctab[slot].prstkbase);
        proctab[slot].prstkbase = NULL;
        proctab[slot].prstkptr = NULL;
    }

    proctab[slot].prstate = PR_FREE;
    proctab[slot].pid = -1;

    if (currpid == pid)
        currpid = -1;

    return 0;
}
