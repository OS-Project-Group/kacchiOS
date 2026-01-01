#ifndef PROCESS_H
#define PROCESS_H

// Max number of processes
#define NPROC 8

// Process states
#define PR_FREE 0  // Slot unused / terminated
#define PR_READY 1 // Ready to run
#define PR_CURR 2  // Currently running

// Process ID type
typedef int pid32;

// Process Control Block (PCB) structure
typedef struct pcb
{
    pid32 pid;       // Process ID
    int prstate;     // Process state
    int prprio;      // Priority
    char *prstkptr;  // Saved stack pointer
    char *prstkbase; // Base of stack
    int next;        // Next process in queue (slot index)
} pcb_t;

// Queue structure
typedef struct queue
{
    int head;        // Front of queue (slot index)
    int tail;        // Back of queue (slot index)
} queue_t;

// Process table
extern pcb_t proctab[NPROC];

// Process currently running
extern pid32 currpid;

// Functions - Process Management
void init_proctab(void);
pid32 create_process(int priority);
int terminate_process(pid32 pid);
void set_current(pid32 pid);
pid32 get_next_ready(void);

// Functions - Queue Operations
void q_insert(int slot, queue_t *q);
int q_remove(queue_t *q);
int q_empty(queue_t *q);

// Functions - Utility/Accessor
pid32 getpid(void);
int find_slot(pid32 pid);
int get_process_state(pid32 pid);
int get_process_priority(pid32 pid);
int is_valid_pid(pid32 pid);
char* get_stack_base(pid32 pid);
int get_num_ready(void);

#endif
