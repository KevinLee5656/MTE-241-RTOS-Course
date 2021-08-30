/**
 * Implement release(), terminate(), and timeslice().
 * @author your name
 */

#include "sched.h"
#include "readylist.h"
#include <stddef.h> 

/**
 * Define idle task and set as initial running task.  Do not change the
 * following two lines of code.
 */
tcb_t tcbIdle = { "idle", P_IDLE };
tcb_t* running = &tcbIdle;
/**
 * Don't change the preceding two lines of code.
 */

// GLOBALS
bool highest_pri = true;
bool release_idle = false;

/**
 * Releases a new task to run.  May change the running task.
 * @param task TCB of new task (assume it is valid)
 */
void release(tcb_t *task) {
	if(!release_idle) {
		enqueue(P_IDLE, &tcbIdle);
		release_idle = true;
	}
	
	if(isEmpty(task->pri)) {
		for(int i=P_HI; i>(task->pri); i--) {
			if(!isEmpty(i)) {
				highest_pri = false;
				break;
			}
		}
		if(highest_pri) {
			enqueue(running->pri, dequeue(running->pri));
			running = task;
		}
	}
	enqueue(task->pri, task);
}

/**
 * Terminates the currently running task. Calls to terminate the idle task
 * should be quietly ignored.
 */
void terminate(void) {
	if(running==&tcbIdle) {
		return;
	}
	
	if(running->prev==NULL && running->next==NULL) {
		dequeue(running->pri);
		for(int i=(running->pri)-1; i>=0; i--) {
			if(!isEmpty(i)) {
				running = dequeue(i);
				enqueue(i, running);
				break;
			}
		}			
	}
	else if(running->next==NULL) {
		while(running->prev!=NULL) {
			enqueue(running->pri, dequeue(running->pri));
		}
		dequeue(running->pri);
		running = running->next;
	}
	else {
		dequeue(running->pri);
				running = running->next;
	}
}

/**
 * Called at the end of a timeslice.  Should schedule next task to run.
 */
void timeslice(void) {
	if(running->next!=NULL) {
		enqueue(running->pri, dequeue(running->pri));
	}
	running = dequeue(running->pri);
	enqueue(running->pri, running);
}

