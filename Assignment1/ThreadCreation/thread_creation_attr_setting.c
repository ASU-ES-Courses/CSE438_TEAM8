#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

void * msg_function();

pthread_mutex_t lock;
int x = 0;


void main(){
	
	int mutex_creation_status, mutex_destroy_status;
	int thread_creation_status[4], thread_destroy_status[4];
	int attr_init_status[4], attr_destroy_status[4];
	int attr_setinherit_status[4], attr_setschedparam_status[4], attr_setschedpolicy_status[4];
	int sig, policy,i; 
	pthread_t p_thread[4];
	pthread_attr_t tattr[4];
	struct sched_param param[4];
	
	param[0].sched_priority = 80;
	param[1].sched_priority = 70;
	param[2].sched_priority = 60;
	param[3].sched_priority = 80;
	
	mutex_creation_status = pthread_mutex_init(&lock, NULL);
	if(mutex_creation_status != 0){
		printf("mutex_lock status = %d\n",mutex_creation_status);
	}
	
	for(i=0;i<4;i++){
		//thread attribute creation
		attr_init_status[i] = pthread_attr_init(&tattr[i]);
		if(attr_init_status[i] != 0){
			printf("attr initialization error %d= %d\n",i,attr_init_status[i]);
		}

		//Thread attribute initialization
		attr_setinherit_status[i] = pthread_attr_setinheritsched(&tattr[i],PTHREAD_EXPLICIT_SCHED);
		if(attr_setinherit_status[i] != 0){
			printf("setinherit sched error %d = %d\n",i,attr_setinherit_status[i]);
		}

		attr_setschedpolicy_status[i] = pthread_attr_setschedpolicy(&tattr[i],SCHED_FIFO);
		if(attr_setschedpolicy_status[i] != 0 ){
			printf("setschedpolicy error %d = %d\n",i,attr_setschedpolicy_status[i]);
		}
		
	
		attr_setschedparam_status[i] = pthread_attr_setschedparam(&tattr[i], &param[i]);
		if(attr_setschedparam_status[i] != 0){
			printf("setschedparam error %d = %d\n", i,attr_setschedparam_status[i]);
		}
	

		//Thread Creation
	
		thread_creation_status[i] = pthread_create( &p_thread[i], NULL, msg_function, NULL);
		printf("thread %d creation status = %d\n",i,thread_creation_status[i]);
	}

	/*
	//Retrive thread attributes
	pthread_getschedparam(p_thread,&policy,&param);
	printf("policy = %d\t priority = %d\n",policy,param.sched_priority);		
	*/
	//waiting for thread termination
	
	for(i=0;i<4;i++){
		pthread_join(p_thread[i], NULL);

		attr_destroy_status[i] = pthread_attr_destroy(&tattr[i]);
		if(attr_destroy_status[i] != 0){
			printf("destroy status of attr %d = %d\n",i,attr_destroy_status[i]);
		}	
  	}

	mutex_destroy_status = pthread_mutex_destroy(&lock);	
	if(mutex_destroy_status != 0){
		printf("lock destroy status = %d\n",mutex_destroy_status);
	}
/*	
	thread_destroy_status = pthread_kill(p_thread,sig);
	if(thread_destroy_status != 0){
		printf("thread destroy error = %d\n",thread_destroy_status);
	}
*/	
}

void * msg_function(){	
	
	pthread_mutex_lock(&lock);

	x++;
	printf("\nNow running thread\n");
	printf("thread_id = %lu\n",pthread_self());
	printf("value of x = %d\n",x);
	printf("thread execution completed\n\n");

	pthread_mutex_unlock(&lock);
}
