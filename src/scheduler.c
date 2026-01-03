/* scheduler.c - Process Scheduler Implementation */
#include "scheduler.h"
#include "process.h"
#include "serial.h"

/* Current scheduling policy */
int sched_policy = SCHED_PRIO;  // Default: Priority-based Round-Robin

/* Context switch function (defined in ctxsw.S) */
extern void ctxsw(void** old_sp, void** new_sp);

/* Scheduler Functions */
void sched_init(void);
void resched(void);
void yield(void);
pid32 schedule_next(void);

/* Process exit handler - called when process function returns */
void user_process_exit(void)
{
    if (currpid != -1) {
        terminate_process(currpid);
    }
    
    /* Reschedule to next process */
    resched();
    
    /* Should never reach here */
    while(1);
}

/* Initialize scheduler */
void sched_init(void)
{
    sched_policy = SCHED_PRIO;
    serial_puts("[Scheduler] Initialized with Priority-based Round-Robin policy\n");
}

/* Main scheduler - select and switch to next process */
void resched(void)
{
    pid32 old_pid = currpid;
    pid32 next_pid;
    int old_slot, next_slot;

    /* Get next process to run */
    next_pid = schedule_next();
    
    if (next_pid == -1) {
        /* No process to run - should not happen in well-designed system */
        serial_puts("[Scheduler] WARNING: No process ready to run!\n");
        return;
    }

    /* If same process, no need to switch */
    if (next_pid == old_pid) {
        return;
    }

    old_slot = find_slot(old_pid);
    next_slot = find_slot(next_pid);

    /* Move current process back to ready (if it was running) */
    if (old_pid != -1 && old_slot != -1) {
        if (proctab[old_slot].prstate == PR_CURR) {
            proctab[old_slot].prstate = PR_READY;
            enqueue_ready(old_slot);
        }
    }

    /* Remove next process from ready queue and mark as current */
    dequeue_process(next_slot);
    proctab[next_slot].prstate = PR_CURR;
    
    /* Reset quantum for new process */
    proctab[next_slot].prtime = proctab[next_slot].prquantum;
    
    currpid = next_pid;

    /* Perform context switch if we had a previous process */
    if (old_pid != -1 && old_slot != -1) {
        ctxsw((void**)&proctab[old_slot].prstkptr, 
              (void**)&proctab[next_slot].prstkptr);
    }
}

/* Voluntarily yield CPU */
void yield(void)
{
    int curr_slot;
    
    if (currpid == -1)
        return;
    
    curr_slot = find_slot(currpid);
    if (curr_slot == -1)
        return;

    /* Update time tracking */
    update_process_time();
    
    /* Apply aging to waiting processes */
    apply_aging();
    
    /* Reschedule */
    resched();
}

/* Select next process based on scheduling policy */
pid32 schedule_next(void)
{
    int slot, best_slot;
    int highest_prio;

    if (sched_policy == SCHED_RR) {
        /* Round-Robin: Just get next from ready queue */
        slot = get_next_ready();
        if (slot == -1)
            return -1;
        return proctab[slot].pid;
    }
    else if (sched_policy == SCHED_PRIO) {
        /* Priority-based: Find highest priority ready process */
        best_slot = -1;
        highest_prio = -1;

        /* Scan ready queue for highest priority */
        slot = readylist.head;
        while (slot != -1) {
            if (proctab[slot].prstate == PR_READY) {
                if (proctab[slot].prprio > highest_prio) {
                    highest_prio = proctab[slot].prprio;
                    best_slot = slot;
                }
            }
            slot = proctab[slot].next;
        }

        if (best_slot == -1)
            return -1;
        
        return proctab[best_slot].pid;
    }

    return -1;
}

/* Set time quantum for a process */
void set_quantum(pid32 pid, int quantum)
{
    int slot = find_slot(pid);
    if (slot != -1) {
        proctab[slot].prquantum = quantum;
        proctab[slot].prtime = quantum;
    }
}

/* Get time quantum for a process */
int get_quantum(pid32 pid)
{
    int slot = find_slot(pid);
    if (slot == -1)
        return -1;
    return proctab[slot].prquantum;
}

/* Apply aging to prevent starvation */
void apply_aging(void)
{
    int i;
    
    for (i = 0; i < NPROC; i++) {
        /* Only age processes in READY state */
        if (proctab[i].prstate == PR_READY) {
            proctab[i].prwait_time++;
            
            /* If waiting too long, boost priority */
            if (proctab[i].prwait_time >= AGING_THRESHOLD) {
                if (proctab[i].prprio < 10) {  // Cap at priority 10
                    proctab[i].prprio += AGING_BOOST;
                }
                proctab[i].prwait_time = 0;  // Reset wait time
            }
        }
        else if (proctab[i].prstate == PR_CURR) {
            /* Reset wait time for running process */
            proctab[i].prwait_time = 0;
            
            /* Restore original priority if it was boosted */
            if (proctab[i].prprio > proctab[i].original_prio) {
                proctab[i].prprio = proctab[i].original_prio;
            }
        }
    }
}

/* Update time tracking for current process */
void update_process_time(void)
{
    int slot;
    
    if (currpid == -1)
        return;
    
    slot = find_slot(currpid);
    if (slot == -1)
        return;
    
    /* Decrement remaining time quantum */
    if (proctab[slot].prtime > 0) {
        proctab[slot].prtime--;
    }
    
    /* Increment total CPU time consumed */
    proctab[slot].prcputime++;
}

/* Print scheduler statistics */
void print_scheduler_stats(void)
{
    int i;
    
    serial_puts("\n========================================\n");
    serial_puts("    Scheduler Statistics\n");
    serial_puts("========================================\n");
    
    serial_puts("Policy: ");
    if (sched_policy == SCHED_RR)
        serial_puts("Round-Robin\n");
    else
        serial_puts("Priority-based Round-Robin\n");
    
    serial_puts("\nProcess Table:\n");
    serial_puts("PID\tState\tPrio\tCPU Time\tWait Time\n");
    
    for (i = 0; i < NPROC; i++) {
        if (proctab[i].prstate != PR_FREE) {
            /* Note: serial_puts doesn't support formatting, 
               so we'd need to implement number printing */
            serial_puts("Process info available\n");
        }
    }
    
    serial_puts("========================================\n\n");
}
