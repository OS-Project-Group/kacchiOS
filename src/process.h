#ifndef PROCESS_H
#define PROCESS_H

// Max number of processes
#define NPROC 8

// Process states
#define PR_FREE     0   // Slot unused / terminated
#define PR_READY    1   // Ready to run
#define PR_CURR     2   // Currently running
#define PR_BLOCKED  3   // Waiting for I/O
#define PR_WAITING  4   // Waiting for message/event
#define PR_SUSPEND  5   // Suspended by user

// Process ID type
typedef int pid32;

// Message structure for IPC (defined first, used in PCB)
#define MSG_SIZE 128

typedef struct message
{
    pid32 sender_pid;      // Who sent this message
    char data[MSG_SIZE];   // Message content
    int len;               // Message length
} Message;

// Process Control Block (PCB) structure
typedef struct pcb
{
    pid32 pid;              // Process ID
    int prstate;            // Process state
    int prprio;             // Priority
    char *prstkptr;         // Saved stack pointer
    char *prstkbase;        // Base of stack
    int next;               // Next process in queue (slot index)
    
    // IPC (Inter-Process Communication)
    Message msg_inbox;      // Latest message received
    int has_msg;            // 1 if message available, 0 otherwise
    pid32 sender_pid;       // Last sender PID
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

// Functions - IPC (Inter-Process Communication)
int send(pid32 dest_pid, char *message, int len);
int receive(pid32 src_pid, char *buffer, int max_len);

#endif
