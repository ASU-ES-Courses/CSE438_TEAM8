#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include<pthread.h>


#include"rdtsc.h"

//Macro definition
#define MAX_MSGS 10
#define MAX_MSG_LENGTH 512
int message_id = -1;
//Structure definition
struct message{
	long double buff; //16
	int m_id;	//4
	unsigned long long enqueue_time, dequeue_time; //8 *2 = 16
	pthread_t sender_id;	//8
	
};
	
struct mq{ 
	//int *item[MAX_MSGS];			//pointer to keep track of message
	struct message *msg[MAX_MSGS];
	int front;				//front points to front end of the queue
	int rear;				//rear points to last element of the queue
	int maxSize;				//Maximum capacity of queue
	int currentSize;			//keep track of current number of messages in queue
	
};

//********* FUNCTION DEFINITIONS *************//

//function to create queue SQ_CREATE
struct mq* sq_create(int size){
	int i;
	struct mq *q = NULL;
	q = (struct mq*)malloc(sizeof(struct mq));
	
	for(i=0;i<MAX_MSGS;i++){
		//q->item[i] = (int*)malloc(sizeof(int) * MAX_MSG_LENGTH);
		(q->msg[i]) = (struct message *)malloc(sizeof(char)*64);
		
		(q->msg[i])->buff = 0;
		(q->msg[i])->m_id = -1;
		(q->msg[i])->enqueue_time = 0;
		(q->msg[i])->dequeue_time = 0;
		(q->msg[i])->sender_id = 1;
	}
	q->front = -1;
	q->rear = -1;
	q->maxSize = size;
	q->currentSize = 0;
	
	return q;
	
}

//funtion to check queue empty or not
int isEmptyMQ(struct mq *q){
	int status = 0; // 0 = FALSE ; 1 = TRUE
	if(q->front == -1){	// if front and rear are both -1
		status = 1;
	}
	return status;
	
}

//function to check queue full or not
int isFullMQ(struct mq *q){
	int status = 0; // 0 = FALSE ; 1 = TRUE
	if(((q->rear == q->maxSize -1) && (q->front == 0)) || (q->rear == q->front -1)){
		status = 1;
	}
	return status;

}

//function to enqueue SQ_WRITE
int sq_write(struct mq *q, long double buff){
	
		if(isFullMQ(q)){
			
			return -1;
		}
		
		if((q->front == -1) && (q->rear == -1)){
			q->front = 0;
		} 
		q->rear = (q->rear + 1) % q->maxSize ;
		
		
		(q->msg[q->rear])->buff = buff;
		(q->msg[q->rear])->m_id = ++message_id;
		(q->msg[q->rear])->sender_id = pthread_self(); //get current thread id
		(q->msg[q->rear])->enqueue_time = rdtsc();
		
		q->currentSize++;
		
		return 1;	//-1 = failure ; 1 = success
	
}

//function to dequeue SQ_READ
struct message * sq_read(struct mq *q){
		
		if(isEmptyMQ(q)){
			return NULL;
	
		}

		struct message *temp;
		temp = (struct message*)malloc(sizeof(struct message));

		temp->buff = (q->msg[q->front])->buff;
		temp->m_id = q->msg[q->front]->m_id;
		temp->enqueue_time = q->msg[q->front]->enqueue_time;
		temp->dequeue_time = rdtsc();
		temp->sender_id = q->msg[q->front]->sender_id;
		
		if(q->front == q->rear){
			q->front = -1;
			q->rear = -1;
		}
		else if(q->front == q->maxSize-1){
			q->front = 0;
		}
		else{
			q->front++;
			
		}		
		q->currentSize--;
		
		return temp; 
	
}

//function to delete queue SQ_DELETE
int sq_delete(struct mq *q){
	int i;
	
	printf("Check1\n");
		
	for(i=0;i<MAX_MSGS;i++){
		free(q->msg[i]);
	}
	printf("Check2\n");
	free(q);	
	printf("Check3\n");
	return 1;
}
