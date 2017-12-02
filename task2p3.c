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

void *Thread1(){

	struct timespec timer, remain;
	int ret;

	timer.tv_sec = 10;
	// clock_gettime(CLOCK_MONOTONIC, &timer);
	// clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timer, 0);
	printf("Thread1 with thread id %lu created\n", pthread_self() );
	printf("For kernelshark Thread1 id %ld\n",syscall(SYS_gettid) );

	printf("Thread1 sleeping for 10 seconds\n");

	ret = nanosleep(&timer, &remain);
	if(ret != 0){
		printf("The timer interrupted and the remaining time is %ld seconds \n", remain.tv_sec);
	}
	else{
		printf("The timer completed and the elapsed time is %ld and the remaining time is %ld \n",timer.tv_sec, remain.tv_sec);
	}

	pthread_exit(NULL);
}
void *Thread2(){

	struct timespec timer;
	
	timer.tv_sec = 3;

	printf("Thread2 with thread id %lu created\n", pthread_self() );

	printf("For kernelshark Thread2 id %ld \n",syscall(SYS_gettid) );

	nanosleep(&timer, NULL);

	pthread_kill(thread_id[0], SIGUSR1);

	pthread_exit(NULL);
}

void signal_handler(int signal_number)
{
	if(signal_number == SIGUSR1){
		printf("In signal handler\n");
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

	//Thread1
	thread_return = pthread_create( &thread_id[0], NULL, &Thread1, NULL);
	if(thread_return != 0){
		printf("Thread1 thread create error");
	}
	// else
	// printf("Thread1 thread created\n");

	//Thread2 
	thread_return = pthread_create( &thread_id[1], NULL, &Thread2, NULL);
	if(thread_return != 0){
		printf("Thread2 thread create error");
	}
	// else
	// printf("Thread2 thread created\n");

	
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