#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include <fcntl.h> // for open
#include<sys/stat.h>
#include <unistd.h> // for close
#include<errno.h>
#include<linux/input.h>
#include<string.h>
#include <semaphore.h>
#include<math.h>

#include"lib_sq.h"


#define MAX_MSGS 10
#define max_number 50
#define minimum_number 10
#define Base_period 1000
#define EVENT_FILE_NAME "/dev/input/event2"

//Global variables

int fd;
struct input_event event;
unsigned long long prev = 0;

int error_status[2], error_status2[7];
int mutex_lock_error[2];

pthread_t p_thread[7];
pthread_attr_t tattr[7];
struct sched_param param[7];

struct mq* q1 ;
struct mq* q2 ;
struct message * object;
pthread_mutex_t lock[2]; //lock[0] -> Q1 ; lock[1] -> Q2
int q_Flag[2] = {0,0};
int q_delete[2] = {0,0};
int termination_Flag = 0;
int terminate = 0;

const int p_period_multiplier[] = {12,32,18,28};
const int r_period_multiplier[] = {40};

//semaphores
sem_t sLock;
sem_t sLock2;
int s_status;

long long total_QTime = 0, mean = 0,sd = 0;
int total_messages = 0;
long long sum_sq = 0;

long long q_array[1000];


//Function declaration
void thread_create();

void* onPeriodicActive(void * id);
void* onReceiverActive(void * id);
void* onAperiodicActive1(void * id);
void* onAperiodicActive2(void * id);

long double calcPI();

// MAIN
int main(){
	//Local variables
	int j; 

	//opening mouse-event file
	fd = open(EVENT_FILE_NAME, O_RDONLY);
	if (fd < 0)
	{
	    printf("failed to open input device %s: %d\n", EVENT_FILE_NAME, errno);
	    
	}

	//Initiating Shared Queues
	printf("\nInitiating Shared Queues...\n");
	q1 = sq_create(MAX_MSGS);
	q2 = sq_create(MAX_MSGS);

	//Initializing Mutex Locks
	printf("\nInitializing Mutex Locks...\n");
	for(j=0;j<2;j++){
		error_status[j] = pthread_mutex_init(&lock[j], NULL);
		if(error_status[j] != 0){
			printf("mutex_init %d error status = %d\n",j,error_status[j]);
		}
	}
	
	//Initializing semaphore
	s_status = sem_init(&sLock, 0, 0);
	if(s_status == -1){
		printf("Semaphore 1 init error = %d\n",s_status);
	}

	s_status = sem_init(&sLock2, 0, 0);
	if(s_status == -1){
		printf("Semaphore 2 init error = %d\n",s_status);
	}

	//Creating all the Threads
	printf("\nCreating Threads...\n");
	thread_create();	
	
	while(read(fd,&event,sizeof(event)) && termination_Flag != 1){
		if(event.code == 272 && event.value == 1 ){
			//LEFT CLICK
				
			if(prev != 0 && ((event.time).tv_sec - prev < 0.5)){
				printf("Double left click encountered\n");
				printf("Entered termination sequence\n");
				termination_Flag = 1;		
			}
			else{	
			printf("left click\n");	
			sem_post(&sLock);
			}
		}
		if(event.code == 273 && event.value == 1  ){
			//RIGHT CLICK
			printf("right click\n");	
			sem_post(&sLock2);
		}
	}
	
	//termination started;

	//waiting for thread termination
	for(j=0;j<7;j++){
		//printf("pthread_join\n");
		error_status2[j] = pthread_join(p_thread[j], NULL);
		if(error_status2[j] != 0){
			printf("thread %d join error status = %d\n",j,error_status2[j]);
		}
		error_status2[j] = pthread_attr_destroy(&tattr[j]);
		if(error_status2[j] != 0){
			printf("thread %d attr_destroy error status = %d\n",j,error_status2[j]);
		}	
  	}

	//Destroying mutex locks
	printf("Destroying Mutex Locks\n");
	for(j=0;j<2;j++){
		error_status[j] = pthread_mutex_destroy(&lock[j]);	
		if(error_status[j] != 0){
			printf("mutex_destroy %d error status = %d\n",j,error_status[j]);
		}
	}
/*
	//Calculating queueing time	
	mean = total_QTime/total_messages;

	for(i=0;i<total_messages;i++){
		sd = sd + pow((q_array[i] - mean),2);
	}
	
	sd = sqrt(sd/total_messages);
	
	printf("Average Queueing time of queues = %lld\n",mean);
	printf("Total number of messages in queues = %d\n",total_messages);
	printf("SD of queueing time of queues = %lld\n", sd);
*/	
	//closing mouse-event file
	printf("Closing mouse-event file...\n");
	close(fd);

	//Destroying shared queues
	printf("Deleting Shared Queues...\n");
	q_delete[0] = sq_delete(q1);
	if(q_delete[0] != 1){
		printf("Q1 delete error = %d\n",q_delete[0]);
	}
	q_delete[1] = sq_delete(q2);
	if(q_delete[1] != 1){
		printf("Q2 delete error = %d\n",q_delete[1]);
	}

	return 0;
}


//Thread Create Function
void thread_create(){	
	int j;

	int *id;
	id = (int*)malloc(sizeof(int));					//To keep track of thread #

	//prio = 1(low) and prio = 99(high)
	param[0].sched_priority = 70; // Periodic thread
	param[1].sched_priority = 70; // " "
	param[2].sched_priority = 70; // " "
	param[3].sched_priority = 70; // " "

	param[4].sched_priority = 68; // Receiver thread

	param[5].sched_priority = 90; // Aperiodic thread
	param[6].sched_priority = 90; // " "
	

	for(j=0;j<7;j++){
		//thread attribute creation
		error_status2[j] = pthread_attr_init(&tattr[j]);
		if(error_status2[j] != 0){
			printf("attr_init error %d= %d\n",j,error_status2[j]);
		}

		//Thread attribute initialization
		error_status2[j] = pthread_attr_setinheritsched(&tattr[j],PTHREAD_EXPLICIT_SCHED);
		if(error_status2[j] != 0){
			printf("thread %d setinherit_sched error = %d\n",j,error_status2[j]);
		}

		//Thread policy initialization
		error_status2[j] = pthread_attr_setschedpolicy(&tattr[j],SCHED_FIFO);
		if(error_status2[j] != 0 ){
			printf("thread %d setschedpolicy error = %d\n",j,error_status2[j]);
		}
	
		//Thread priority initialization
		error_status2[j] = pthread_attr_setschedparam(&tattr[j], &param[j]);
		if(error_status2[j] != 0){
			printf("thread %d setschedparam error = %d\n", j,error_status2[j]);
		}
	
		//Thread Creation
		switch(j){
			case 0:{
				*id = 0;	
				error_status2[j] = pthread_create( &p_thread[j], &tattr[j], &onPeriodicActive, (void*)id);
				if(error_status2[j] != 0){
					printf("thread %d create error status = %d\n",j,error_status2[j]);
				}
				printf("Created 1st periodic thread...\n");
				break;
			}
			case 1:{
				*id = 1;	
				error_status2[j] = pthread_create( &p_thread[j], &tattr[j], &onPeriodicActive, (void*)id);
				if(error_status2[j] != 0){
					printf("thread %d create error status = %d\n",j,error_status2[j]);
				}
				printf("Created 2nd periodic thread...\n");
				break;
			}
			case 2:{
				*id = 2;	
				error_status2[j] = pthread_create( &p_thread[j], &tattr[j], &onPeriodicActive, (void*)id);
				if(error_status2[j] != 0){
					printf("thread %d create error status = %d\n",j,error_status2[j]);
				}
				printf("Created 3rd periodic thread...\n");
				break;
			}
			case 3:{
				*id = 3;	
				error_status2[j] = pthread_create( &p_thread[j], &tattr[j], &onPeriodicActive,(void*)id);
				if(error_status2[j] != 0){
					printf("thread %d create error status = %d\n",j,error_status2[j]);
				}
				printf("Created 4th periodic thread...\n");
				break;
			}
			case 4:{
				*id = 4;
				error_status2[j] = pthread_create( &p_thread[j], &tattr[j], &onReceiverActive, (void*)id);
				if(error_status2[j] != 0){
					printf("thread %d create error status = %d\n",j,error_status2[j]);
				}
				printf("\nCreated Receiver thread...\n");
				break;
			}
			case 5:{
				*id = 5;
				error_status2[j] = pthread_create( &p_thread[j], &tattr[j], &onAperiodicActive1, (void*)id);
				if(error_status2[j] != 0){
					printf("thread %d create error status = %d\n",j,error_status2[j]);
				}
				printf("\nCreated  1st Aperiodic thread...\n");
				break;
			}
			case 6:{
				*id = 6;
				error_status2[j] = pthread_create( &p_thread[j], &tattr[j], &onAperiodicActive2, (void*)id);
				if(error_status2[j] != 0){
					printf("thread %d create error status = %d\n",j,error_status2[j]);
				}
				printf("Created  2nd Aperiodic thread...\n");
				break;
			}			
		}
	}	
}

void* onReceiverActive(void * id){
	//int n = *(int*)id;
	int count = 0;							// Local variable to implement functiononality of reading alternatively from Q1 and Q2.
	struct timespec next, period;

	object = (struct message *)malloc(sizeof(sizeof(char)*64));	//Creating a temporary message object for reading messages from Queues 

	clock_gettime(CLOCK_MONOTONIC, &next);
	period.tv_nsec = Base_period * r_period_multiplier[0];		//Base multiplier of receiver thread
	printf("Checking receiver\n");
	//Iterate till both queues are empty and termination flag is set to 1
	while(1){

		if(isEmptyMQ(q1) == 1 && isEmptyMQ(q2) ==1 && termination_Flag == 1){
			break;
		}
		printf("Entered receiver routine\n");

		count++;

		if( count%2 == 0){					//To implement functiononality of reading alternatively from Q1 and Q2.
			//If count even read from Q1

			pthread_mutex_lock(&lock[0]);			//requesting MUTEX LOCK for Q1 for mutually exclusive execution of Dequeue operation
			printf("\nNow running receiver thread #1 with thread_id = %lu dequeuing from Q1\n",pthread_self());
			object = sq_read(q1);				//Reading from Q1
			pthread_mutex_unlock(&lock[0]);			//Unlocking MUTEX LOCK for Q1
		}

		else{
			//If count odd read from Q2

			pthread_mutex_lock(&lock[1]);			//requesting MUTEX LOCK for Q2 for mutually exclusive execution of Dequeue operation
			printf("\nNow running receiver thread #1 with thread_id = %lu dequeuing from Q2\n",pthread_self());
			object = sq_read(q2);				//Reading from Q2
			pthread_mutex_unlock(&lock[1]);			//Unlocking MUTEX LOCK for Q1
		}
	/*
		sum_sq = sum_sq + pow((object->dequeue_time - object->enqueue_time),2);
		total_QTime = total_QTime + (object->dequeue_time - object->enqueue_time);
		total_messages++;

		q_array[total_messages - 1] = (object->dequeue_time - object->enqueue_time);
*/		

		//Implementing periodicity
		if((next.tv_nsec+period.tv_nsec)>=1000000000){
			next.tv_nsec = (next.tv_nsec+period.tv_nsec)%1000000000;
			next.tv_sec++;
		}
		else{		
			next.tv_nsec = next.tv_nsec+period.tv_nsec;
		}
		
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);		
	}
	
	return 0;
}

//Periodic thread function
void* onPeriodicActive(void * id){
	int q_status = 0;
	int n = *(int*)id;
	long double pi;
	struct timespec next, period;


	clock_gettime(CLOCK_MONOTONIC, &next);

	//To select respective period multiplier
	/*switch(n){
		case 0:{
			
			period.tv_nsec = Base_period * p_period_multiplier[0];
			break;
		} 
		case 1:{
			period.tv_nsec = Base_period * p_period_multiplier[1];
			break;
		}  
		case 2:{
			period.tv_nsec = Base_period * p_period_multiplier[2];
			break;
		}  
		case 3:{
			period.tv_nsec = Base_period * p_period_multiplier[3];
			break;
		} 
	}*/

	period.tv_nsec = Base_period * p_period_multiplier[n];

	while(termination_Flag != 1){

		pi = calcPI();

		//If periodic thread #0 and #1 are active enqueue to Queue1
		if( n == 0 || n == 1){			

			pthread_mutex_lock(&lock[0]);				//requesting MUTEX LOCK for Q1 for mutually exclusive execution of Enqueue operation
			printf("\nNow running periodic thread #%d with thread_id = %lu\n",n,pthread_self());										
			sleep(1);
			q_status = sq_write(q1,pi);				//Writing to Q1
			if( q_status == -1){
				printf("Message dropped in Queue1\n");
			}
			//sleep(1);
			pthread_mutex_unlock(&lock[0]);				//Unlocking MUTEX LOCK for Q1
		}

		//If periodic thread #2 and #3 are active enqueue to Queue2
		else if(n==2 || n== 3){			

			pthread_mutex_lock(&lock[1]);				//requesting MUTEX LOCK for Q2 for mutually exclusive execution of Enqueue operation
			printf("\nNow running periodic thread #%d with thread_id = %lu\n",n,pthread_self());
			sleep(1);
			q_status = sq_write(q2,pi);				//Writing to Q2
			if( q_status == -1){
				printf("Message dropped in Queue2\n");
			}	
			//sleep(1);		
			pthread_mutex_unlock(&lock[1]);				//Unlocking MUTEX LOCK for Q2
		}

		//Implementing periodicity
		if((next.tv_nsec+period.tv_nsec)>=1000000000){
			next.tv_nsec = (next.tv_nsec+period.tv_nsec)%1000000000;
			next.tv_sec++;

		}
		else{		
			next.tv_nsec = next.tv_nsec+period.tv_nsec;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);				
	}
	
	return 0;
}

//Aperiodic thread 2 function implementing "RIGHT CLICK" functionality	
void* onAperiodicActive2(void * id){
	//int n = *(int*)id;
	long double pi;
	int q_status = 0;

	while(termination_Flag != 1){

		sem_wait(&sLock2);					//requesting semaphore lock for Q2 for sequential execution of RIGHT CLICK event
			
		pi = calcPI();

		pthread_mutex_lock(&lock[1]);				//requesting MUTEX LOCK for Q2 for mutually exclusive execution of Enqueue operation
		printf("\nNow running aperiodic thread #2 with thread_id = %lu\n",pthread_self());
		sleep(1);
		q_status = sq_write(q2,pi);				//Writing to Q2
		if( q_status == -1){
			printf("Message dropped from Queue2\n");
		}
		//sleep(1);
		pthread_mutex_unlock(&lock[1]);				//Unlocking MUTEX LOCK for Q2
	}
	return 0;	
}

//Aperiodic thread 1 function implementing "LEFT CLICK" functionality	
void* onAperiodicActive1(void * id){
	//int n = *(int*)id;
	long double pi;
	int q_status = 0;
	
	while(termination_Flag != 1){		
			
		sem_wait(&sLock);					//requesting semaphore lock for Q1 for sequential execution of LEFT CLICK event
		
		prev = (event.time).tv_sec;
		pi = calcPI();

		pthread_mutex_lock(&lock[0]);				//requesting MUTEX LOCK for Q1 for mutually exclusive execution of Enqueue operation
		printf("\nNow running aperiodic thread #1 with thread_id = %lu\n",pthread_self());	
		sleep(1);			
		q_status = sq_write(q1,pi);				//Writing to Q1
		if( q_status == -1){
			printf("Message dropped from Queue1\n");
		}
		//sleep(1);
		pthread_mutex_unlock(&lock[0]);				//Unlocking MUTEX LOCK for Q1
	}					
	return 0;
}

//PIE value calculation function
long double calcPI(){
	//Taken from https://www.codeproject.com/Articles/813185/Calculating-the-Number-PI-Through-Infinite-Sequenc
	// Approximation of the number pi through the Leibniz's series
	
	double _n,_i;          
	double s = 1;
	long double pi = 0;

	// Generating random number of iterations between 10 to 50
	_n = (rand()%(max_number - minimum_number))+minimum_number;	
	
	for(_i = 1; _i <= (_n * 2); _i += 2){
     		pi = pi + s * (4 / _i);
     		s = -s;
   	}
	return pi;	
}


