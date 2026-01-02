/* scheduler.h - Process Scheduler Interface */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

/* Scheduling Policies */
#define SCHED_RR       0    // Round-Robin
#define SCHED_PRIO     1    // Priority-based Round-Robin

/* Default Configuration */
#define DEFAULT_QUANTUM     10      // Default time quantum
#define AGING_THRESHOLD     50      // Wait time before priority boost
#define AGING_BOOST         1       // Priority increment for aging

/* Current scheduling policy */
extern int sched_policy;

/* Scheduler Functions */
void sched_init(void);
void resched(void);
void yield(void);
pid32 schedule_next(void);
void user_process_exit(void);  // Called when process function returns

/* Time Quantum Management */
void set_quantum(pid32 pid, int quantum);
int get_quantum(pid32 pid);

/* Aging Mechanism */
void apply_aging(void);

/* Time Tracking */
void update_process_time(void);

/* Utility */
void print_scheduler_stats(void);

#endif
