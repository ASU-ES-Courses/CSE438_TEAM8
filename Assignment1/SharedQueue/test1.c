#include<stdio.h>
#include<stdlib.h>
#include"lib_sq.h"


#define MAX_MSGS 10
// MAIN
int main(){	
	struct mq* q1 = sq_create(MAX_MSGS);
	int i;
	int delete;
	//struct mq* q2 = sq_create(MAX_MSGS);
	printf("Check\n");
	struct message * object = (struct message *)malloc(sizeof(sizeof(char)*64));
	long double buff;
	//Collect the returned values of read and write operation
	
	for(i=0;i<12;i++){
		buff = rand();
		if(sq_write(q1,buff) == 1){
			printf("%dth enqueued successfully\n",i);
		}
		else{
			printf("Enqueue unsuccessful\n");
			printf("\nREADING OBJECT | DEQUEUE\n\n");
			object = sq_read(q1);
			printf("object ->buff = %LG\n",(object->buff));
			printf("object->m_id = %d\n",object->m_id);
			printf("object->time = %llu\n",object->dequeue_time - object->enqueue_time);
			printf("object->sender_id = %lu\n",object->sender_id);
			//break;
		}
	}
	/*
	//printf("\nElements of Q1:\n\n");
	while(q1->front < 10){
		printf("\nREADING OBJECTS FROM QUEUE Q1\n\n");
		printf("front = %d\n",q1->front);
		printf("rear = %d\n",q1->rear);
		printf("maxSize = %d\n",q1->maxSize);
		printf("currentSize = %d\n",q1->currentSize);
		//printf("lock = %d\n",q1->lock);
		printf("msg = %LG\n", (q1->msg[q1->front])->buff);
		printf("m_id = %d\n", (q1->msg[q1->front])->m_id);
		printf("enqueue_time = %llu\n", (q1->msg[q1->front])->enqueue_time);
		printf("dequeue_time = %llu\n",	(q1->msg[q1->front])->dequeue_time);
		printf("sender_id = %lu\n", (q1->msg[q1->front])->sender_id);
		//q1->currentSize--;
		q1->front++;
	}
	*/
	printf("\nDeleting the queue\n\n");
	delete = sq_delete(q1);
	return 0;
	//printf("q1 deleted\n");
	//exit(0);
	//sq_delete(q2);
	//printf("q2 deleted\n");
	//return 0;
}
