#include <cmsis_os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "random.h"
#include "lfsr113.h"

#define SERVICE_RATE_HZ 10
#define ARRIVAL_RATE_HZ 9

typedef struct {
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t;


osMessageQueueId_t queue_1;
osMessageQueueId_t queue_2;

int iterations, prev_iterations = 0;
int queue1Sent, queue2Sent, queue1Received, queue2Received, queue1Overflows, queue2Overflows = 0;

void server(void *arg) {
	MSGQUEUE_OBJ_t msg;
	bool server_queue_1 = true;
	
	while(true){
		msg.Buf[0] = 0x55U;
		msg.Idx  = 0U;
		
		int delay = osKernelGetTickFreq() * next_event(); // pseudo-floating-point value into an integer that�s been multiplied by 2^16
		delay /= SERVICE_RATE_HZ;
		delay = delay >> 16;
		osDelay(delay);
		
		if(queue_1 == (osMessageQueueId_t *) arg) {
			server_queue_1 = true;
		} else {
			server_queue_1 = false;
		}
		// osMessageQueueGet(osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout)
		if(osMessageQueueGet((osMessageQueueId_t *) arg, &msg, NULL, osWaitForever) == osOK) { 
			if(server_queue_1) {
				queue1Received++;
			} else {
			queue2Received++;
			}
		}
	}
}

void client(void *arg) {
	MSGQUEUE_OBJ_t msg;
	bool receiver_queue_1 = true;
	
	while(true){
		msg.Buf[0] = 0x55U;
		msg.Idx  = 0U;
		
		int delay = osKernelGetTickFreq() * next_event();
		delay /= (ARRIVAL_RATE_HZ * 2); // Since each of the 2 queues should be 9 Hz, the delay should be halved
		delay = delay >> 16;
		osDelay(delay);

		if(receiver_queue_1) {
			// osStatus_t osMessageQueuePut(osMessageQueueId_t mq_id, const void * msg_ptr, uint8_t msg_prio, uint32_t timeout)
			if(osMessageQueuePut(queue_1, &msg, 0, 0) == osErrorResource) {
				queue1Overflows++;
			} else {
				queue1Sent++;
			}
		} else {
			if(osMessageQueuePut(queue_2, &msg, 0, 0) == osErrorResource) {
				queue2Overflows++;
			} else {
				queue2Sent++;
			}
		}
		receiver_queue_1 = receiver_queue_1 ? false : true; // alternate every run
		osThreadYield();
	}
}	

void monitor(void *arg) {
	while(true){
		osDelay(1 * osKernelGetTickFreq());
		iterations++;
		printf("Seconds passed: %d, queue1Sent: %d, queue2Sent: %d, queue1Received: %d, queue2Received: %d, queue1Overflows: %d, queue2Overflows: %d\n",
		iterations, queue1Sent, queue2Sent, queue1Received, queue2Received, queue1Overflows, queue2Overflows);
	}
}

int main() {
	osKernelInitialize();
	
	queue_1 = osMessageQueueNew(10, sizeof(int), NULL);
	queue_2 = osMessageQueueNew(10, sizeof(int), NULL);
	
	// osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
	osThreadNew(server, queue_1, NULL); // server thread for queue 1
	osThreadNew(server, queue_2, NULL); // server thread for queue 2
	osThreadNew(client, NULL, NULL); // client thread for message generation
	osThreadNew(monitor, NULL, NULL); // monitor thread to wait 1 sec and print
	
	osKernelStart();
	for(;;);
}
