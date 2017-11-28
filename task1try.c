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
#include<setjmp.h>

#define EVENT_FILE_NAME "/dev/input/event18"

//Globals
struct input_event event;
unsigned long long prev = 0;
 
pthread_t thread_id[2];
int termination_Flag = 0;

jmp_buf buff;
sig_atomic_t count = 0;

void *MouseClick(){

	int fdm;
	
	fdm = open(EVENT_FILE_NAME, O_RDONLY);							//opening mouse-event file
	if (fdm < 0)
	{
	    printf("failed to open input device %s: %d\n", EVENT_FILE_NAME, errno);
	    
	}

	while(read(fdm,&event,sizeof(event)) && termination_Flag != 1){
		if(event.code == 273 && event.value == 1 ){
			//RIGHT CLICK
				
			if(prev != 0 && ((event.time).tv_sec - prev < 0.5)){
				printf("Double right click encountered\n");
				kill(getpid(), SIGUSR1);		
			}
			else{
				prev = (event.time).tv_sec; 
			}
			
		}
	}
	return NULL;
}
void *Computation(){

	while(count < 1000){
		// if(termination_Flag == 1){
		// 	longjmp(buff, 1);
		// }
		if(setjmp(buff)==1){
			printf("Computation Terminated and the count is %d \n", count);
			exit(0);
		}
		else{
			count += 1;
			usleep(10000);
			// printf("count %d \n", count);
		}
				
		}	
		printf("Computation completed and the count is %d \n", count);
		exit(0);

	return NULL;
}

void Right_click(int signal_number)
{
	termination_Flag = 1;
	longjmp(buff, 1);
}

int main(){

	int thread_return;
	struct sigaction click_detection;
	memset (&click_detection, 0, sizeof (click_detection));
	click_detection.sa_handler = &Right_click;
	if(sigaction(SIGUSR1, &click_detection, NULL) == -1){
		printf("sigaction error occured\n");
	}

	// pthread_mutex_init(&dist_mutex, NULL);

	//Thread to detect Mouse event click
	thread_return = pthread_create( &thread_id[0], NULL, &MouseClick, NULL);
	if(thread_return != 0){
		printf("MouseClick thread create error");
	}
	else
	printf("MouseClick thread created\n");

	//Thread for Computation
	thread_return = pthread_create( &thread_id[1], NULL, &Computation, NULL);
	if(thread_return != 0){
		printf("Computation thread create error");
	}
	else
	printf("Computation thread created\n");

	
	thread_return = pthread_join(thread_id[0], NULL);
	if(thread_return != 0){
		printf("thread join error\n");
	}	

	thread_return = pthread_join(thread_id[1], NULL);
	if(thread_return != 0){
		printf("thread join error\n");
	}	
	
	// pthread_mutex_destroy(&dist_mutex);

	return 0;
}
