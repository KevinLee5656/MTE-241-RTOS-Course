/**
 * TCB and scheduling functions.
 * @author Andrew Morton
 */

#ifndef SCHED_H
#define SCHED_H

typedef enum { P_IDLE = 0, P_LOW = 1, P_MED = 2, P_HI = 3 } PRIORITY;

/**
 * Task Control Block definition.
 */
typedef struct tcb {
	const char *name;
	PRIORITY pri;
	struct tcb *next;
	struct tcb *prev;
} tcb_t;

/**
 * Releases a new task to run.  May change the running task.
 * @param task TCB of new task (assume it is valid)
 */
void release(tcb_t *task);

/**
 * Terminates the currently running task.  Calls to terminate the idle task
 * should be quietly ignored.
 */
void terminate(void);

/**
 * Called at the end of a timeslice.  Should schedule next task to run.
 */
void timeslice(void);

#endif
