#include <cmsis_os2.h>
#include "general.h"

// add any #includes here
#include <string.h>
// add any #defines here
#define MAX_GENERALS 7

// add global variables here
osMessageQueueId_t queues[MAX_GENERALS][3]; // 0 - broadcast, 1 - m_OM=1, 2 - m_OM=2
osSemaphoreId_t semBroadcast;
uint8_t n, m, reporter_id = 0;
bool loyalty[MAX_GENERALS];

/** Record parameters and set up any OS and other resources
  * needed by your general() and broadcast() functions.
  * nGeneral: number of generals
  * loyal: array representing loyalty of corresponding generals
  * reporter: general that will generate output
  * return true if setup successful and n > 3*m, false otherwise
  */
bool setup(uint8_t nGeneral, bool loyal[], uint8_t reporter) {
	n = nGeneral;
	reporter_id = reporter;
	m = 0;
	for(int i=0; i<n; i++) {
		loyalty[i] = loyal[i];
		if(loyal[i] == false) {
			m++;
		}
	}
	if(n <= 3*m) {
		return c_assert(n > 3*m);
	}
	for(int i=0; i<n; i++) {
		for(int j=0; j<m+1; j++) {
			queues[i][j] = osMessageQueueNew(20, 8 * sizeof(char), NULL);
			if(queues[i][j] == NULL) {
				printf("ERROR: failed to make queue\n");
			}
		}
	}
	
	semBroadcast = osSemaphoreNew(n-1, 0, NULL);
	if(semBroadcast == NULL) {
		printf("ERROR: failed to make semaphore\n");
	}
	return c_assert(n > 3*m);
}



/** This function performs the initial broadcast to n-1 generals.
  * It should wait for the generals to finish before returning.
  * Note that the general sending the command does not participate
  * in the OM algorithm.
  * command: either 'A' or 'R'
  * sender: general sending the command to other n-1 generals
  */
void broadcast(char command, uint8_t commander) {
	char message[8];
	uint8_t count = 0;
	
	for(int gen=0; gen<n; gen++) {
		if(gen!=commander) {
			if(loyalty[commander] == false) {
				if(gen%2==0) {
					command = 'R';
				}
				else {
					command = 'A';
				}
			}
			sprintf(message, "%d:%c", commander, command);
			if(osMessageQueuePut(queues[gen][0], message, 0, 0) != osOK) {
				printf("ERROR: Message failed to send\n");
			}
		}
	}
	
	while(count < n-1) {
		if(osSemaphoreAcquire(semBroadcast, osWaitForever) == osOK) {
			count++;
		}
	}
}



/** Generals are created before each test and deleted after each
  * test.  The function should wait for a value from broadcast()
  * and then use the OM algorithm to solve the Byzantine General's
  * Problem.  The general designated as reporter in setup()
  * should output the messages received in OM(0).
  * idPtr: pointer to general's id number which is in [0,n-1]
  */
void OM(uint8_t m_OM, uint8_t id, char receivedMessage[]) {	
	char prependedMessage[8];
	char idChar[1];
	if(m_OM > 0) {
		
		if(loyalty[id] == false) { 
			if(id%2==0) {
				receivedMessage[(strlen(receivedMessage)-1)] = 'R';
			}
			else {
				receivedMessage[(strlen(receivedMessage)-1)] = 'A';
			}
		}
		
		sprintf(prependedMessage, "%d:%s", id, receivedMessage);
		
		for(int i=0; i<n; i++) {
			sprintf(idChar, "%d", i);
			if(strstr(prependedMessage, idChar) == NULL) {
				if(osMessageQueuePut(queues[i][m_OM], prependedMessage, 0, 0) != osOK) { // modify
					printf("ERROR: Message failed to send\n");
				}
			}
		}
		
		osThreadYield();
		while(osMessageQueueGet(queues[id][m_OM], receivedMessage, 0, 0) == osOK) { // modify
			OM(m_OM-1, id, receivedMessage);
		}
	}
	else if(m_OM==0) {
		if(id==reporter_id) {
			printf("%s ", receivedMessage);
		}
	}
}

void general(void *idPtr) {
	uint8_t id = *(uint8_t *)idPtr;
	char broadcastMessage[8];
	
	while(true) {
		if(osMessageQueueGet(queues[id][0], broadcastMessage, NULL, osWaitForever)==osOK) {
			OM(m, id, broadcastMessage); 
			osSemaphoreRelease(semBroadcast);
		}
	}
}


/** Delete any OS resources created by setup() and free any memory
  * dynamically allocated by setup().
  */
void cleanup(void) {
	for(int i=0; i<n; i++) {
		for(int j=0; j<m+1; j++) {
			if(osMessageQueueDelete(queues[i][j]) != osOK) {
				printf("ERROR: Queue failed to delete\n");
			}
		}
	}
	if(osSemaphoreDelete(semBroadcast) != osOK) {
		printf("ERROR: Semaphore failed to delete\n");
	}
}
