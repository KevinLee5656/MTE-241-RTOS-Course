/**
 * Simulate FPP scheduling.
 * @author Andrew Morton
 */

#include "sched.h"
#include <stdio.h>

extern tcb_t *running;

typedef enum { TIMESLICE, END } action_t;

int main(void) {

	tcb_t tcbs[] = {
		{ "task0", P_LOW },
		{ "task1", P_LOW },
		{ "task2", P_MED },
		{ "task3", P_MED },
		{ "task4", P_MED },
		{ "task5", P_HI }
	};
	
	const int nTask = sizeof(tcbs)/sizeof(tcb_t);

	action_t actions[] = {
		TIMESLICE, END, TIMESLICE, TIMESLICE, TIMESLICE, END, END, END,
		TIMESLICE, TIMESLICE, TIMESLICE, END, END, TIMESLICE, END };
	const int nAction = sizeof(actions)/sizeof(action_t);

	// release all tasks
	for(int i=0; i<nTask; i++)
		release(&tcbs[i]);
	printf("%s", running->name); // print running task's name

	// do actions
	for(int i=0; i<nAction; i++) {
		switch(actions[i]) {
			case TIMESLICE: printf(" ^ "); timeslice(); break;
			case END: printf(" ! "); terminate(); break;
		}
		printf("%s", running->name);
	}
	printf("\n");
}

