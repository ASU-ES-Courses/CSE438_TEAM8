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
// #include<setjmp.h>

// #define EVENT_FILE_NAME "/dev/input/event18"

//Globals
struct input_event event;
unsigned long long prev = 0;
 
pthread_t thread_id[3];
sig_atomic_t termination_Flag = 0,count=0;
sigset_t set;
int signal_number;

// void *MouseClick(){

// 	// int fdm;
	
// 	// fdm = open(EVENT_FILE_NAME, O_RDONLY);							//opening mouse-event file
// 	// if (fdm < 0)
// 	// {
// 	//     printf("failed to open input device %s: %d\n", EVENT_FILE_NAME, errno);
	    
// 	// }

// 	// while(read(fdm,&event,sizeof(event)) && termination_Flag != 1){
// 	// 	if(event.code == 273 && event.value == 1 ){
// 	// 		//RIGHT CLICK
				
// 	// 		if(prev != 0 && ((event.time).tv_sec - prev < 0.5)){
// 	// 			printf("Double right click encountered\n");
// 	// 			kill(getpid(), SIGUSR1);		
// 	// 		}
// 	// 		else{
// 	// 			prev = (event.time).tv_sec; 
// 	// 		}
			
// 	// 	}
// 	// }

// 	return NULL;
// }

void *threadfunction(){

	// int status;
	printf("threadwith id %lu waiting for SIGIO\n",pthread_self());
	
	while(termination_Flag==0){}
		// status = sigwait(&set, &signal_number);
		// sleep(5);
		// if(status != 0){
		// 	printf("sigwait failed\n");
		// }
		// if(termination_Flag==1){	
			// usleep(100);
			count ++;
			printf("thread with id %lu received the SIGIO signal\n",pthread_self());
			termination_Flag = 0;
		// }
		// else{
			// usleep(100);
		// }
	// termination_Flag = 1;
	pthread_exit(NULL);
}

void signal_action_handler(int signal_number)
{
	// termination_Flag = 1;
	if(signal_number == SIGIO){
		if(termination_Flag==0){
			printf("Executing the signal handler\n");
			termination_Flag = 1;
		}
	}
}

int main(){

	int thread_return, status;
	struct sigaction signal_action;
	memset (&signal_action, 0, sizeof (signal_action));
	signal_action.sa_handler = &signal_action_handler;
	if(sigaction(SIGIO, &signal_action, NULL) == -1){
		printf("sigaction error occured\n");
	}

	sigemptyset(&set);
	sigaddset(&set, SIGIO);

	//Thread1
	thread_return = pthread_create( &thread_id[0], NULL, &threadfunction, NULL);
	if(thread_return != 0){
		printf("thread1 create error");
	}
	else
	// printf("thread1 created\n");

	//Thread2
	thread_return = pthread_create( &thread_id[1], NULL, &threadfunction, NULL);
	if(thread_return != 0){
		printf("thread2 create error");
	}
	else
	// printf("thread2 created\n");
	
	//Thread3
	thread_return = pthread_create( &thread_id[2], NULL, &threadfunction, NULL);
	if(thread_return != 0){
		printf("thread3 create error");
	}
	else
	// printf("thread3 created\n");


	status = pthread_sigmask(SIG_BLOCK, &set, NULL);	
	if(status != 0){
		printf("pthread_sigmask error\n");
	}

	sleep(1);

	while (count < 3){
		if(termination_Flag==0){
			kill(getpid(),SIGIO);
		}
		else{

			usleep(100);

		}
	}

	// sleep(2);
		
	thread_return = pthread_join(thread_id[0], NULL);
	if(thread_return != 0){
		printf("thread1 join error\n");
	}	

	thread_return = pthread_join(thread_id[1], NULL);
	if(thread_return != 0){
		printf("thread2 join error\n");
	}	
	

	thread_return = pthread_join(thread_id[2], NULL);
	if(thread_return != 0){
		printf("thread3 join error\n");
	}

	return 0;
}