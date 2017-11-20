#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
 
pthread_t  thread_id[2];
pthread_mutex_t dist_mutex;

int Final_distance;

void *DistanceSensor(){

	int fd,distance;
	fd = open("/dev/Sensor", O_RDWR);
	if(fd<0){

        printf("Unable to open device\n");
    }

    while(1){

    		write(fd,"trigger",sizeof("trigger"));
    		usleep(10000);
    		read(fd, &distance, sizeof(distance));
    		usleep(10000);
    		pthread_mutex_lock(&dist_mutex);
    		Final_distance = distance*0.017;
    		pthread_mutex_unlock(&dist_mutex);
    		printf("Distance obtained is %d\n",Final_distance);
    		usleep(10000);
    }

	return NULL;
}

void *Display(){


	return NULL;
}

int main(){

	int thread_return;

	pthread_mutex_init(&dist_mutex, NULL);

	//Thread for Distance sensor
	thread_return = pthread_create( &thread_id[1], NULL, &DistanceSensor, NULL);
	if(thread_return != 0){
		printf("Distance Sensor thread create error");
	}
	else
	printf("Distance Sensor thread created\n");

	//Thread for LED Display
	thread_return = pthread_create( &thread_id[0], NULL, &Display, NULL);
	if(thread_return != 0){
		printf("Display thread create error");
	}
	else
	printf("Display thread created\n");
	
	thread_return = pthread_join(thread_id[1], NULL);
	if(thread_return != 0){
		printf("thread join error\n");
	}	

	thread_return = pthread_join(thread_id[0], NULL);
	if(thread_return != 0){
		printf("thread join error\n");
	}	
	
	pthread_mutex_destroy(&dist_mutex);

	return 0;
}
