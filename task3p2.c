#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<pthread.h>
#include<linux/input.h>
#include<errno.h>
#include<unistd.h>
#include<time.h>
#include<signal.h>
#include "siglib.h"

//Globals
struct input_event event;
unsigned long long prev = 0;
static int count = 0;
pthread_t thread_id[5];
sig_atomic_t termination_Flag = 0;
sigset_t set;
int *id;
struct sig_thread  *s1[5];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *threadfunction(){

	printf("Thread created with id %lu\n",pthread_self());
	sleep(3);
		
	while(1){
		if(termination_Flag == 1){
			if(pthread_equal(pthread_self(), s1[0]->threadId)){
				termination_Flag = 0;
				printf("Signal captured by thread %lu\n",pthread_self());
				pthread_mutex_lock(&lock);
				count++;
				//printf("count = %d\n",count);
				pthread_mutex_unlock(&lock);
				break;
			}
			else if(pthread_equal(pthread_self(), s1[1]->threadId)){
				termination_Flag = 0;
				printf("Signal captured by thread %lu\n",pthread_self());
				pthread_mutex_lock(&lock);
				count++;
				//printf("count = %d\n",count);
				pthread_mutex_unlock(&lock);
				break;
			}
			else if(pthread_equal(pthread_self(), s1[2]->threadId)){
				termination_Flag = 0;
				printf("Signal captured by thread %lu\n",pthread_self());
				pthread_mutex_lock(&lock);
				count++;
				//printf("count = %d\n",count);
				pthread_mutex_unlock(&lock);
				break;
			}
		}
		else if(count >= 3 /*&& termination_Flag == 1*/){
			termination_Flag = 0;
			printf("Registered thread exited already\n Terminating unregisterd ones\n");
			printf("Terminated thread with id %lu\n",pthread_self());
			pthread_mutex_lock(&lock);
			count++;
			//printf("count = %d\n",count);
			pthread_mutex_unlock(&lock);
			break;
		}

	}
	//printf("Exited while loop\n");
	pthread_exit(NULL);
}

void signal_action_handler(int signal_number)
{
	if(signal_number == SIGIO){
		termination_Flag = 1;		
	}
}

int main(){

	int thread_return, status,i;
	
	struct sigaction signal_action;
	memset (&signal_action, 0, sizeof (signal_action));
	signal_action.sa_handler = &signal_action_handler;

	if(sigaction(SIGIO, &signal_action, NULL) == -1){
		printf("sigaction error occured\n");
	}

	sigemptyset(&set);
	sigaddset(&set, SIGIO);


	//Thread Creation
	for(i=0;i<5;i++){
		thread_return = pthread_create( &thread_id[i], NULL, &threadfunction, NULL);
		if(thread_return != 0){
			printf("thread%d create error",i);
		}
	}

	//Registering 3 Threads
	printf("Registering 3 threads\n");

	for(i=0;i<3;i++){
		s1[i] = register_thread(thread_id[i]);
	}

	//usleep(100);

	printf("De-Registering 2 threads\n");
	for(i=3;i<5;i++){		
		s1[i] = de_register_thread(thread_id[i]);
	}

	//mask SIGIO for main
	status = pthread_sigmask(SIG_BLOCK, &set, NULL);	
	if(status != 0){
		printf("pthread_sigmask error\n");
	}

	printf("wait...\n");
	//sleep(3);

	while(count<5){
		kill(getpid(),SIGIO);
	}

	// Thread Join	
	for(i=0;i<5;i++){
		thread_return = pthread_join(thread_id[i], NULL);
		if(thread_return != 0){
			printf("thread%d join error\n",i);
		}
	}

	return 0;
}