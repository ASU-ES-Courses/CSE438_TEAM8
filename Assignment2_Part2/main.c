#include<stdio.h>
#include<sys/stat.h> 
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<sys/ioctl.h>
#include<string.h>
#include<malloc.h>
#include<pthread.h>
#include<linux/input.h>
#include<time.h>

#define EVENT_FILE_NAME "/dev/input/event2"
#define CONFIG 1

int terminate=0;
pthread_t  thread_id;

struct values{
	int arr[4];
};

//Mouse Event detection function
void *mouse_click(){

	int  fd;
	struct input_event event;
	fd = open(EVENT_FILE_NAME, O_RDONLY);
	if (fd < 0){
	    printf("failed to open input device %s: %d\n", EVENT_FILE_NAME, errno);	    
	}

	while(read(fd,&event,sizeof(event)) && terminate != 1){
		//If left or right click then set termination flag
		if( (event.code == 272 && event.value == 1 ) || (event.code == 273 && event.value == 1) ){
			printf("Click event detected\n");
			terminate = 1;
			//break;
		}
	}

return NULL;
}

int main(){

	int fd = 0,return_value,i,thread_status = 0,k = 25;
	struct values *object;
	struct timespec req,rem;
	req.tv_sec = 0;
	req.tv_nsec = (1000000000/2);			// = 0.5 sec
	int arr_pattern[7] = {1,2,4,3,5,6,7}; 	//{'R','G','B','RG','RB','GB','RGB'}
	
	object = (struct values *)malloc(sizeof(struct values));
	printf("Enter Integer input values in order [ percentage of PWM, R_IO, G_IO, B_IO ]\n");
	
	for(i=0;i<4;i++){
		scanf("%d",&(object->arr[i]));		
	}
		
	fd = open("/dev/RGBLed", O_RDWR);						
	if (fd < 0 ){
		printf("Can not open RGBLed device file.\n");		
		//return -1;
	}
	else{
		return_value = ioctl(fd,CONFIG,object);
		if(return_value < 0){
			printf("Unable to write to RGBLed Device Invalid input\n");
			errno = 22;		//ENIVAL = 22
		}
		//valid inputs
		else{	
			//Creating a thread to capture mouse click events
			thread_status = pthread_create( &thread_id, NULL, &mouse_click, NULL);
			if(thread_status != 0){
				printf("thread create error status");
			}
			else{	
				while(1){
					if(terminate == 1){
						break;
					}
					//When PWM is not equal to 0
					if(object->arr[0] != 0){

						for(i=0;i<7;i++){

							//for(i=0;i<k;i++){

							/*clock_gettime(CLOCK_MONOTONIC, &req);

							while(1){
								if( (req.tv_nsec - rem.tv_nsec) > (1000000000/2) ){
									break;
								}*/
								write(fd,&arr_pattern[i],sizeof(arr_pattern[i]));
								nanosleep(&req, &rem);
							//Check for termination
							if(terminate == 1){
								break;
							}	
						}
					}
					// When PWM is 0
					else {
						write(fd,0,sizeof(int));	
					}	

				}
			}
		}
		
	}

	close(fd);
	free(object);
	return 0;
}
