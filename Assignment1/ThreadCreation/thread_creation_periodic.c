#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#define Base_period 1000

const int p_period_multiplier[] = {12,32,18,28};
int i;
void * msg_function();

pthread_mutex_t lock;
int x = 0;


void main(){
	
	int status1, _S[4], status4;
	pthread_t p_thread[4], a_thread[2];
	
	status1 = pthread_mutex_init(&lock, NULL);
	printf("mutex_lock status = %d\n",status1);

		
	//Thread Creation
	for(i=0;i<4;i++){
		_S[i] = pthread_create( &p_thread[i], NULL, msg_function, NULL);
		printf("thread %d status = %d\n",i,_S[i]);
	}


	//waiting for thread termination
	for(i=0;i<4;i++){
		pthread_join(p_thread[i], NULL);
	}
    	
	status4 = pthread_mutex_destroy(&lock);	
	printf("lock destroy status = %d\n",status4);
	
}



void * msg_function(){	
	
	struct timespec next, period;
	clock_gettime(CLOCK_MONOTONIC, &next);
	int j=0;
	while(j<4){
		next.tv_nsec = 1000;
		period.tv_nsec = Base_period*p_period_multiplier[i];	
		pthread_mutex_lock(&lock);
		x++;
		printf("thread_id = %lu\n",pthread_self());
		printf("value of x = %d\n",x);

		pthread_mutex_unlock(&lock);
		j=j+1;
		next.tv_nsec = next.tv_nsec + period.tv_nsec;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
	}
}




