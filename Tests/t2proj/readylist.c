/**
 * Readylist implementation.  Do not modify.
 * @author Andrew Morton
 */

#include "readylist.h"
#include <stddef.h>  // included for NULL definition
#include <stdio.h>

/* Used to validate inputs. */
#define c_assert(e) ((e) ? (true) : \
		(printf("%s,%d: assertion '%s' failed\n", \
		__FILE__, __LINE__, #e), false))

/**
 * Define a doubly-linked list type.
 */
typedef struct {
	tcb_t *head;
	tcb_t *tail;
} list_t;

/**
 * Allocate the readylist array and initialize them all empty.  There is
 * one list per priority level.
 */
list_t ready[P_HI + 1] = { { 0 } };;

void enqueue(int pri, tcb_t *task) {
	c_assert(pri >= P_IDLE && pri <= P_HI);

	if(ready[pri].tail) {
		ready[pri].tail->next = task;
		task->prev = ready[pri].tail;
	} else {
		ready[pri].head = task;
		task->prev = NULL;
	}
	ready[pri].tail = task;
	task->next = NULL;
}

tcb_t *dequeue(int pri) {
	c_assert(pri >= P_IDLE && pri <= P_HI);

	if(ready[pri].head == NULL)
		return NULL;

	tcb_t *removed = ready[pri].head;
	ready[pri].head = ready[pri].head->next;
	if(ready[pri].head != NULL) {
		ready[pri].head->prev = NULL;
	} else {
		ready[pri].tail = NULL;
	}

	return removed;
}

bool isEmpty(int pri) {
	c_assert(pri >= P_IDLE && pri <= P_HI);
	return ready[pri].head == NULL;
}

