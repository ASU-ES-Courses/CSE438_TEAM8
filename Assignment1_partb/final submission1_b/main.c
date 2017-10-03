#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<time.h>
#include<pthread.h>
#include<linux/input.h>
#include<errno.h>
#include<unistd.h>
#include <time.h>
#include <semaphore.h>
#include<math.h>

//#include"rdtsc.h"
#define MAX_MSGS 10
#define max_number 50
#define minimum_number 10
#define Base_period 1000
#define EVENT_FILE_NAME "/dev/input/event2"
#define CPU_FREQ 2.5e9
//file variables
int fdm,fd[2], res;

//flag variables
int termination_Flag = 0;

//variables for click events 
struct input_event event;
unsigned long long prev = 0;

//thread variables
pthread_t p_thread[7];
pthread_attr_t tattr[7];
struct sched_param param[7];
int error_status[2], error_status2[7];

static int message_id = 0;

//period multipliers
const int p_period_multiplier[] = {12,32,18,28};
const int r_period_multiplier[] = {40};

//mutex locks
pthread_mutex_t lock[2]; 									//lock[0] -> Q1 ; lock[1] -> Q2
int mutex_lock_error[2];

//semaphores
sem_t sLock;
sem_t sLock2;
int s_status;

//Function declaration
void thread_create();

void* onPeriodicActive(void * id);
void* onReceiverActive(void * id);
void* onAperiodicActive1(void * id);
void* onAperiodicActive2(void * id);

long double calcPI();
unsigned long long rdtsc();

//Structure definition
struct message{
	long double buff; 									//to store value of pi
	int m_id;										//to keep track of message_id
	unsigned long long enqueue_time, dequeue_time; 						//to keep track of enqueue dequeue time
	unsigned long long sender_id;								//to keep track of sender thread's id
	
};

int main()
{
	
	int j = 0;
	
	fdm = open(EVENT_FILE_NAME, O_RDONLY);							//opening mouse-event file
	if (fdm < 0)
	{
	    printf("failed to open input device %s: %d\n", EVENT_FILE_NAME, errno);
	    
	}
	
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
	
	while(read(fdm,&event,sizeof(event)) && termination_Flag != 1){
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

	//Destroying semaphores
	printf("Closing mouse-event file...\n");
	close(fdm);

	return 0;
}

//Thread Create Function
void thread_create(){	
	int j;

	int *id;
	id = (int*)malloc(sizeof(int));								//To keep track of thread #

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

//Receiver function
void* onReceiverActive(void * id){
	int res1, res2,i, sum = 0;
	float mean;
	float difference = 0;                                        
        float dsquares = 0;
	struct timespec next, period;
	struct message * object;
	//Average computation variables
	int count = 0;
	int time_array[1000];
	object = (struct message *)malloc(sizeof(struct message));	//Creating a temporary message object for reading messages from Queues 

	clock_gettime(CLOCK_MONOTONIC, &next);
	period.tv_nsec = Base_period * r_period_multiplier[0];		//Base multiplier of receiver thread
	
	//Iterate till both queues are empty and termination flag is set to 1
	while(1){
	
		if(termination_Flag == 1 && res1 < 0 && res2 < 0){
			break;
		}

		printf("Entered receiver routine\n");		

		while(1){					
			if(res1<0){
				//printf("here1");
				break;
			}
			pthread_mutex_lock(&lock[0]);						//requesting MUTEX LOCK for Q1 for mutually exclusive execution of Dequeue operation

			fd[0] = open("/dev/dataqueue1", O_RDWR);						/* open devices */
			if (fd[0] < 0 ){
				printf("Can not open 1st device file.\n");		
				//return -1;
			}
			if(fd[0] > -1){
				printf("\nNow running receiver thread #1 with thread_id = %lu dequeuing from Q1\n",pthread_self());		

				res1 = read(fd[0],object, sizeof(struct message));			//Reading from Q1
				if(res1 < 0){
					printf("unsuccesful in reading from dataqueue1\n");
					errno = 22;//ENIVAL = 22
					//return -1;
				}
				object->dequeue_time = rdtsc();
				close(fd[0]);
				printf("Closing device files...\n");
			}
			pthread_mutex_unlock(&lock[0]);						//Unlocking MUTEX LOCK for Q1

			printf("Object read from Q1\n");
			printf("object->buff = %LF\n",object->buff);
			printf("object->m_id = %d\n",object->m_id);
			printf("object->enqueue_time = %lld\n",object->enqueue_time);
			printf("object->dequeue_time = %lld\n",object->dequeue_time);
			printf("object->sender_id = %lld\n\n",object->sender_id);
			
			time_array[count] = object->dequeue_time - object->enqueue_time;
			count++;
		}
		//printf("here2");
		res2=0;	

		while(1){
			if(res2<0){
				break;
			}

			pthread_mutex_lock(&lock[1]);						//requesting MUTEX LOCK for Q2 for mutually exclusive execution of Dequeue operation

			fd[1] = open("/dev/dataqueue2", O_RDWR);				/* open devices */
			if (fd[1] < 0 ){
				printf("Can not open 2nd device file.\n");		
				//return -1;
			}					
			if(fd[1] > -1){
				printf("\nNow running receiver thread #1 with thread_id = %lu dequeuing from Q2\n",pthread_self());
			
				res2 = read(fd[1],object, sizeof(struct message));			//Reading from Q2
				if(res2 < 0){
					printf("unsuccesful in reading from dataqueue2\n");
					errno = 22; //ENIVAL = 22
					//return -1;
				}
				object->dequeue_time = rdtsc();
				close(fd[1]);
				printf("Object read from Q2\n");
				printf("object->buff = %LF\n",object->buff);
				printf("object->m_id = %d\n",object->m_id);
				printf("object->enqueue_time = %lld\n",object->enqueue_time);
				printf("object->dequeue_time = %lld\n",object->dequeue_time);
				printf("object->sender_id = %lld\n\n",object->sender_id);
			}
			pthread_mutex_unlock(&lock[1]);						//Unlocking MUTEX LOCK for Q1
			
			time_array[count] = object->dequeue_time - object->enqueue_time;
			count++;
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

	for(i=0;i<count;i++){
		sum = sum + time_array[i];
	}
	mean = (float)sum/(float)count;

	printf("Total number of messages received and dequeued are : %d.\n", count);   
	printf("Mean time spent on queue is %.4f s.\n", mean/(float)CPU_FREQ); 

	for(i=0; i < count; i++)
	{
	    difference =(time_array[i]/(float)CPU_FREQ) - mean/(float)CPU_FREQ;    
	    dsquares += difference*difference;
	}
	dsquares = dsquares/ (float)count;
	dsquares = sqrt(dsquares);   
	printf("Standard Deviation of Queueing time is %.4f s\n", dsquares);
	
	return 0;
}


//Periodic thread function
void* onPeriodicActive(void * id){
	int n = *(int*)id;
	long double pi;
	struct timespec next, period;

	struct message * object;
	object = (struct message *)malloc(sizeof(struct message));	//Creating a temporary message object for writting messages to Queues 

	clock_gettime(CLOCK_MONOTONIC, &next);

	period.tv_nsec = Base_period * p_period_multiplier[n];

	while(termination_Flag != 1){
		//if(termination_Flag != 1){}
		pi = calcPI();

		//If periodic thread #0 and #1 are active enqueue to Queue1
		if( n == 0 || n == 1){			
			pthread_mutex_lock(&lock[0]);				//requesting MUTEX LOCK for Q1 for mutually exclusive execution of Enqueue operation
	
			fd[0] = open("/dev/dataqueue1", O_RDWR);						
	
			if (fd[0] < 0 ){
				printf("Can not open 1st device file.\n");		
				//return -1;
			}
			if(fd[0] > -1){
				printf("\nNow running periodic thread #%d with thread_id = %lu\n",n,pthread_self());										
				sleep(1);
			
				object->buff = pi;				
				message_id = message_id + 1;
				object->m_id = message_id;
				object->enqueue_time = rdtsc();
				object->sender_id = pthread_self();
			
				res = write(fd[0],object,sizeof(struct message));	//Writing to datqueue1
				if(res < 0){
					printf("message dropped from dataqueue1\n");
					errno = 22;//ENIVAL = 22
				}
				close(fd[0]);
				if(res > -1)
					printf("Object wrote to Q1\n");
				printf("Closing device files...\n");
			}
			pthread_mutex_unlock(&lock[0]);				//Unlocking MUTEX LOCK for Q1
			
		}

		//If periodic thread #2 and #3 are active enqueue to Queue2
		else if(n==2 || n== 3){			

			pthread_mutex_lock(&lock[1]);				//requesting MUTEX LOCK for Q2 for mutually exclusive execution of Enqueue operation
	
			fd[1] = open("/dev/dataqueue2", O_RDWR);						/* open devices */
			
			if (fd[1] < 0 ){
				printf("Can not open 2nd device file.\n");		
				//return -1;
			}
			if(fd[1] > -1){
				printf("\nNow running periodic thread #%d with thread_id = %lu\n",n,pthread_self());
				sleep(1);
				object->buff = pi;
				message_id = message_id + 1;
				object->m_id = message_id;
				object->enqueue_time = rdtsc();
				object->sender_id = pthread_self();
			
				res = write(fd[1],object,sizeof(struct message));	//Writing to datqueue2
				if(res < 0){
					printf("message dropped from dataqueue2\n");
					errno = 22;//ENIVAL = 22
				}	
	
				close(fd[1]);
				if(res > -1)
					printf("Object wrote to Q2n");
				printf("Closing device files...\n");	
			}		
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
	struct message * object;
	object = (struct message *)malloc(sizeof(struct message));	//Creating a temporary message object for writting messages to Queues 

	while(termination_Flag != 1){
		//if(termination_Flag != 1){}
		sem_wait(&sLock2);					//requesting semaphore lock for Q2 for sequential execution of RIGHT CLICK event
			
		pi = calcPI();

		pthread_mutex_lock(&lock[1]);				//requesting MUTEX LOCK for Q2 for mutually exclusive execution of Enqueue operation
	
		fd[1] = open("/dev/dataqueue2", O_RDWR);						/* open devices */
		if (fd[1] < 0 ){
			printf("Can not open 2nd device file.\n");		
			//return -1;
		}			
		if(fd[1] > -1){
			printf("\nNow running aperiodic thread #2 with thread_id = %lu\n",pthread_self());
			sleep(1);
			object->buff = pi;
			message_id = message_id + 1;
			object->m_id = message_id;
			object->enqueue_time = rdtsc();
			object->sender_id = pthread_self();
		
			res = write(fd[1],object,sizeof(struct message));	//Writing to datqueue2
			if(res < 0){
				printf("message dropped from dataqueue2\n");
				errno = 22;//ENIVAL = 22
			}	
	
			close(fd[1]);	
			if(res > -1)
				printf("Object wrote to Q2\n");
			printf("Closing device files...\n");
		}		
		pthread_mutex_unlock(&lock[1]);				//Unlocking MUTEX LOCK for Q2
		
	}
	return 0;	
}

//Aperiodic thread 1 function implementing "LEFT CLICK" functionality	
void* onAperiodicActive1(void * id){
	//int n = *(int*)id;
	long double pi;
	struct message * object;
	object = (struct message *)malloc(sizeof(struct message));	//Creating a temporary message object for writting messages to Queues 

	while(termination_Flag != 1){		
		//if(termination_Flag != 1){}
		sem_wait(&sLock);					//requesting semaphore lock for Q1 for sequential execution of LEFT CLICK event
		
		prev = (event.time).tv_sec;
		pi = calcPI();

		pthread_mutex_lock(&lock[0]);				//requesting MUTEX LOCK for Q1 for mutually exclusive execution of Enqueue operation
	
		fd[0] = open("/dev/dataqueue1", O_RDWR);						
	
		if (fd[0] < 0 ){
			printf("Can not open 1st device file.\n");		
			//return -1;
		}

		if(fd[0] > -1){
			printf("\nNow running periodic thread #1 with thread_id = %lu\n",pthread_self());										
			sleep(1);
			
			object->buff = pi;
			message_id = message_id + 1;
			object->m_id = message_id;
			object->enqueue_time = rdtsc();
			object->sender_id = pthread_self();
		
			res = write(fd[0],object,sizeof(struct message));	//Writing to datqueue1
			if(res < 0){
				printf("message dropped from dataqueue1\n");
				errno = 22;//ENIVAL = 22
			}
			close(fd[0]);
			if(res > -1)
				printf("Object wrote to Q1\n");
			printf("Closing device files...\n");
		}
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

// taken from -> https://www.mcs.anl.gov/~kazutomo/rdtsc.html
unsigned long long rdtsc(){
	#if defined(__x86_64__)
	//static __inline__ unsigned long long rdtsc(void)
	//{
	  unsigned hi, lo;
	
	  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	
	  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
	 #endif	
}

