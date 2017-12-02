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

//Globals
pthread_t thread_id[2];
pthread_attr_t tattr[2];
struct sched_param param[2];
int signal_Flag = 0;

void *Low_priority_thread(){


	printf("Low_priority_thread with thread id %lu created\n", pthread_self() );

	printf("For kernelshark Low_priority_thread id %ld\n",syscall(SYS_gettid) );

	usleep(10000);		// so that it can switch to main and High_priority_thread can be created 

	while(signal_Flag == 0){
	}

	printf("Signal recieved by Low_priority_thread with id %lu\n",pthread_self() );

	pthread_exit(NULL);
}
void *High_priority_thread(){

	int count = 0;

	printf("High_priority_thread with thread id %lu created\n", pthread_self() );

	printf("For kernelshark High_priority_thread id %ld \n",syscall(SYS_gettid) );

	printf("Signal Sent \n");

	pthread_kill(thread_id[0], SIGUSR1);

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

	printf("High priority thread exits\n");

	pthread_exit(NULL);
}

void signal_handler(int signal_number)
{
	if(signal_number == SIGUSR1){
		printf("In signal handler\n");
		signal_Flag = 1;
	}
}

int main(){

	int thread_return, j, error_status[1];
	struct sigaction signal_handler_action;
	memset (&signal_handler_action, 0, sizeof (signal_handler_action));
	signal_handler_action.sa_handler = &signal_handler;
	if(sigaction(SIGUSR1, &signal_handler_action, NULL) == -1){
		printf("sigaction error occured\n");
	}

	//prio = 1(low) and prio = 99(high)
	param[0].sched_priority = 68;
	param[1].sched_priority = 90;

	for(j=0;j<2;j++){
		//thread attribute creation
		error_status[j] = pthread_attr_init(&tattr[j]);
		if(error_status[j] != 0){
			printf("attr_init error %d= %d\n",j,error_status[j]);
		}

		//Thread attribute initialization
		error_status[j] = pthread_attr_setinheritsched(&tattr[j],PTHREAD_EXPLICIT_SCHED);
		if(error_status[j] != 0){
			printf("thread %d setinherit_sched error = %d\n",j,error_status[j]);
		}

		//Thread policy initialization
		error_status[j] = pthread_attr_setschedpolicy(&tattr[j],SCHED_FIFO);
		if(error_status[j] != 0 ){
			printf("thread %d setschedpolicy error = %d\n",j,error_status[j]);
		}
	
		//Thread priority initialization
		error_status[j] = pthread_attr_setschedparam(&tattr[j], &param[j]);
		if(error_status[j] != 0){
			printf("thread %d setschedparam error = %d\n", j,error_status[j]);
		}
	}	

	//Low_priority_thread
	thread_return = pthread_create( &thread_id[0], &tattr[0], &Low_priority_thread, NULL);
	if(thread_return != 0){
		printf("Low_priority_thread thread create error");
	}
	// else
	// printf("Low_priority_thread thread created\n");

	//High_priority_thread 
	thread_return = pthread_create( &thread_id[1], &tattr[1], &High_priority_thread, NULL);
	if(thread_return != 0){
		printf("High_priority_thread thread create error");
	}
	// else
	// printf("High_priority_thread thread created\n");

	
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
