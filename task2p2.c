#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<pthread.h>
#include<linux/input.h>
#include<errno.h>
#include<unistd.h>
#include<sched.h>
#include<signal.h>
#include<syscall.h>
#include<semaphore.h>

//Globals
pthread_t thread_id[2];
int signal_Flag = 0;
sem_t sLock;

void *Thread1(){


	printf("Thread1 with thread id %lu is waiting for semaphore.....\n", pthread_self() );
	printf("For kernelshark Thread1 id %ld\n",syscall(SYS_gettid) );

	sem_wait(&sLock);

	printf("Semaphore acquired by Thread1 with id %lu\n",pthread_self() );

	pthread_exit(NULL);
}
void *Thread2(){

	int count = 0;

	printf("Thread2 with thread id %lu created\n", pthread_self() );
	// show_sched();
	printf("For kernelshark Thread2 id %ld \n",syscall(SYS_gettid) );

	usleep(1000);

	for (int i = 0; i < 1000; i++){

		for (int j = 0; j < 1000; j++)
		{
			for (int k = 0; k < 1000; k++)
			{
				count += 2;
				count -= 1;
			}
		}
	}

	printf("Kill function called !!\n");

	pthread_kill(thread_id[0], SIGUSR1);

	// sem_post(&sLock);

	pthread_exit(NULL);
}

void signal_handler(int signal_number)
{
	if(signal_number == SIGUSR1){
		printf("In signal handler\n");
		sem_post(&sLock);
	}
}

int main(){

	int thread_return, j, error_status[1], s_status;
	struct sigaction signal_handler_action;
	memset (&signal_handler_action, 0, sizeof (signal_handler_action));
	signal_handler_action.sa_handler = &signal_handler;
	if(sigaction(SIGUSR1, &signal_handler_action, NULL) == -1){
		printf("sigaction error occured\n");
	}

	//Initializing semaphore
	s_status = sem_init(&sLock, 0, 0);
	if(s_status == -1){
		printf("Semaphore 1 init error = %d\n",s_status);
	}

	//Thread1
	thread_return = pthread_create( &thread_id[0], NULL, &Thread1, NULL);
	if(thread_return != 0){
		printf("Thread1 thread create error");
	}
	else
	printf("Thread1 thread created\n");

	//Thread2 
	thread_return = pthread_create( &thread_id[1], NULL, &Thread2, NULL);
	if(thread_return != 0){
		printf("Thread2 thread create error");
	}
	else
	printf("Thread2 thread created\n");

	
	thread_return = pthread_join(thread_id[0], NULL);
	if(thread_return != 0){
		printf("thread join error\n");
	}	

	thread_return = pthread_join(thread_id[1], NULL);
	if(thread_return != 0){
		printf("thread join error\n");
	}	
	

	return 0;
}
