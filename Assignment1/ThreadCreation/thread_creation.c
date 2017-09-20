#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#define Base_period 1000

void * msg_function();

//Global variable declaration
int x = 0, i;
int error_status, error_status2[4];

pthread_mutex_t lock;
pthread_t p_thread[4];
pthread_attr_t tattr[4];
struct sched_param param[4];

const int p_period_multiplier[] = {12,32,18,28};

//Function declaration
void periodic_thread_create();

//Main
void main(){	
		
	//Initializing mutex lock
	error_status = pthread_mutex_init(&lock, NULL);
	if(error_status != 0){
		printf("mutex_init error status = %d\n",error_status);
	}
	
	//Creating Periodic Sender threads
	periodic_thread_create();

	//waiting for thread termination
	
	for(i=0;i<4;i++){
		error_status2[i] = pthread_join(p_thread[i], NULL);
		if(error_status2[i] != 0){
			printf("thread %d join error status = %d\n",i,error_status2[i]);
		}

		error_status2[i] = pthread_attr_destroy(&tattr[i]);
		if(error_status2[i] != 0){
			printf("thread %d attr_destroy error status = %d\n",i,error_status2[i]);
		}	
  	}

	//Destroying mutex lock
	error_status = pthread_mutex_destroy(&lock);	
	if(error_status != 0){
		printf("mutex_destroy error status = %d\n",error_status);
	}
	
}
void periodic_thread_create(){
	
	param[0].sched_priority = 80;
	param[1].sched_priority = 70;
	param[2].sched_priority = 60;
	param[3].sched_priority = 80;

	for(i=0;i<4;i++){
		//thread attribute creation
		error_status2[i] = pthread_attr_init(&tattr[i]);
		if(error_status2[i] != 0){
			printf("attr_init error %d= %d\n",i,error_status2[i]);
		}

		//Thread attribute initialization
		error_status2[i] = pthread_attr_setinheritsched(&tattr[i],PTHREAD_EXPLICIT_SCHED);
		if(error_status2[i] != 0){
			printf("thread %d setinherit_sched error = %d\n",i,error_status2[i]);
		}

		error_status2[i] = pthread_attr_setschedpolicy(&tattr[i],SCHED_FIFO);
		if(error_status2[i] != 0 ){
			printf("thread %d setschedpolicy error = %d\n",i,error_status2[i]);
		}
		
	
		error_status2[i] = pthread_attr_setschedparam(&tattr[i], &param[i]);
		if(error_status2[i] != 0){
			printf("thread %d setschedparam error = %d\n", i,error_status2[i]);
		}
	

		//Thread Creation
	
		error_status2[i] = pthread_create( &p_thread[i], NULL, msg_function, NULL);
		if(error_status2[i] != 0){
			printf("thread %d create error status = %d\n",i,error_status2[i]);
		}
	}

	
}

void * msg_function(){	
	struct timespec next, period;
	clock_gettime(CLOCK_MONOTONIC, &next);
	int j=0;

	//Modify loop to infinite and terminate with event 
	while(j<4){ 
	
		next.tv_nsec = 1000;
		period.tv_nsec = Base_period*p_period_multiplier[i];

		pthread_mutex_lock(&lock);
		x++;
		printf("\nNow running thread\n");
		printf("thread_id = %lu\n",pthread_self());
		printf("value of x = %d\n",x);
		printf("tv_nsec = %ld\n",next.tv_nsec);
		printf("tv_sec = %ld\n",next.tv_sec);
		printf("thread execution completed\n\n");

		pthread_mutex_unlock(&lock);
		j=j+1;
		next.tv_nsec = next.tv_nsec + period.tv_nsec;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
	}
}
