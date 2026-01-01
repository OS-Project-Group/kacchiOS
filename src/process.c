#include "process.h"

/* Global process table */
pcb_t proctab[NPROC];

/* Current running process */
pid32 currpid = -1;

/* Initialize process table */
void init_proctab(void)
{
    for (int i = 0; i < NPROC; i++)
    {
        proctab[i].pid = i;
        proctab[i].prstate = PR_FREE;
        proctab[i].prprio = 0;
        proctab[i].prstkptr = 0;
        proctab[i].prstkbase = 0;
    }
}

/* Find a free slot and create process */
pid32 create_process(int priority)
{
    for (int i = 0; i < NPROC; i++)
    {
        if (proctab[i].prstate == PR_FREE)
        {
            proctab[i].prstate = PR_READY;
            proctab[i].prprio = priority;
            return i;
        }
    }
    return -1; /* No free process slot */
}

/* Set a process as currently running */
void set_current(pid32 pid)
{
    if (pid < 0 || pid >= NPROC)
        return;

    /* Move old current back to READY */
    if (currpid != -1 && proctab[currpid].prstate == PR_CURR)
    {
        proctab[currpid].prstate = PR_READY;
    }

    proctab[pid].prstate = PR_CURR;
    currpid = pid;
}

/* Terminate a process */
int terminate_process(pid32 pid)
{
    if (pid < 0 || pid >= NPROC)
        return -1;

    proctab[pid].prstate = PR_FREE;

    if (pid == currpid)
        currpid = -1;

    return 0;
}
