#ifndef PROCESS_H
#define PROCESS_H

/* Maximum number of processes */
#define NPROC 8

/* Process states */
#define PR_FREE 0  /* Slot unused / terminated */
#define PR_READY 1 /* Ready to run */
#define PR_CURR 2  /* Currently running */

/* Process ID type */
typedef int pid32;

/* Process Control Block (PCB) */
typedef struct pcb
{
    pid32 pid;       /* Process ID */
    int prstate;     /* Process state */
    int prprio;      /* Priority */
    char *prstkptr;  /* Saved stack pointer */
    char *prstkbase; /* Base of stack */
} pcb_t;

/* Process table */
extern pcb_t proctab[NPROC];

/* Currently running process */
extern pid32 currpid;

/* Function prototypes */
void init_proctab(void);
pid32 create_process(int priority);
int terminate_process(pid32 pid);
void set_current(pid32 pid);

#endif
