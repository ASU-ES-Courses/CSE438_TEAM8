#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

void * msg_function();

pthread_mutex_t lock;
int x = 0;


void main(){
	
	int status1, _S[4], status4, i;
	pthread_t p_thread[4], a_thread[2];
	
	status1 = pthread_mutex_init(&lock, NULL);
	printf("mutex_lock status = %d\n",status1);
		
	//Thread Creation
	for(i=0;i<4;i++){
		_S[i] = pthread_create( &p_thread[i], NULL, msg_function, NULL);
		printf("thread %d status = %d\n",i,_S[i]);
		//printf("
	}
	
	
	//waiting for thread termination
	for(i=0;i<4;i++){
		pthread_join(p_thread[i], NULL);
	}
    	
	status4 = pthread_mutex_destroy(&lock);	
	printf("lock destroy status = %d\n",status4);
	
}

void * msg_function(){	
	
	pthread_mutex_lock(&lock);

	x++;
	printf("thread_id = %lu\n",pthread_self());
	printf("value of x = %d\n",x);

	pthread_mutex_unlock(&lock);
}
