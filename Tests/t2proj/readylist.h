/**
 * Readylist declarations.
 * @author Andrew Morton
 */

#ifndef READYLIST_H
#define READYLIST_H

#include "sched.h"
#include <stdbool.h>

/**
 * Insert a task at the tail of the readylist for given priority.
 * @param pri which readylist to append to
 * @param task TCB of task to append
 */
void enqueue(int pri, tcb_t *task);

/**
 * Remove a task from the head of the readylist for given priority.
 * @param pri which readylist to remove from
 * @return a pointer to dequeued task or NULL if empty
 */
tcb_t *dequeue(int pri);

/**
 * Query whether priority's readylist is empty.
 * @param pri priority of list to check
 * @return true if empty, false othewise
 */
bool isEmpty(int pri);

#endif
