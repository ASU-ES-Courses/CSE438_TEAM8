#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>

#include"Gpio_func.h"
#include"Gpio_func.c"


static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}


pthread_t  thread_id;
int timeout = -1;

void PinSetup(void)
{
	int pinexport, dir_status, val_status;
	//Export 
	pinexport = gpio_export(13);
	if(pinexport < 0){
		printf("export error triger gpio\n");		
	}
	//Set Dir 
	dir_status = gpio_set_dir(13, 1);
	if (dir_status < 0){
		printf("\n triger gpio direction set failed\n");
	}
	

	//Export 
	pinexport = gpio_export(34);
	if(pinexport < 0){
		printf("export error triger gpio Level Shifter\n");		
	}
	//Set Dir 
	dir_status = gpio_set_dir(34, 1);
	if (dir_status < 0){
		printf("\n triger gpio Level Shifter direction set failed\n");
	}
	//Set Val 
	val_status = gpio_set_value(34, 0);
	if (val_status < 0){
		printf("\n triger gpio Value set failed\n");
	}

	//Export 
	pinexport = gpio_export(77);
	if(pinexport < 0){
		printf("export error triger gpio mux\n");		
	}
	//Set Val 
	val_status = gpio_set_value(77, 0);
	if (val_status < 0){
		printf("\n triger gpio mux Value set failed\n");
	}

	//Export 
	pinexport = gpio_export(14);
	if(pinexport < 0){
		printf("export error echo gpio\n");		
	}
	//Set Dir 
	dir_status = gpio_set_dir(14, 0);
	if (dir_status < 0){
		printf("\n echo gpio direction set failed\n");
	}


	//Export 
	pinexport = gpio_export(16);
	if(pinexport < 0){
		printf("export error echo gpio Level Shifter\n");		
	}
	//Set Dir 
	dir_status = gpio_set_dir(16, 1);
	if (dir_status < 0){
		printf("\n echo gpio Level Shifter direction set failed\n");
	}
	//Set Val 
	val_status = gpio_set_value(16, 1);
	if (val_status < 0){
		printf("\n echo gpio Level Shifter Value set failed\n");
	}

	//Export 
	pinexport = gpio_export(76);
	if(pinexport < 0){
		printf("export error echo gpio mux\n");		
	}
	//Set Val 
	val_status = gpio_set_value(76, 0);
	if (val_status < 0){
		printf("\n echo gpio mux Value set failed\n");
	}
}

void *distance(){

	int i,val_status,fd,fd1,ret1,ret2;
	unsigned long long rise,fall;
	double dis;
	char c;
	fd = open("/sys/class/gpio/gpio14/edge", O_WRONLY);
	fd1 = open("/sys/class/gpio/gpio14/value", O_RDONLY);
	struct pollfd poll_struct;
	
	//poll_struct.events = POLLPRI;
while(1)
//for (i = 0; i < 7; i++)
{

	write(fd,"rising",6);
	// close(fd1);
	// fd1 = open("/sys/class/gpio/gpio14/value", O_RDONLY);
	//Triggering Pulse
	val_status = gpio_set_value(13, 1);
	if (val_status < 0){
		printf("\n trigger pulse 1 unsuccessful\n");
	}
	usleep(15);
	val_status = gpio_set_value(13, 0);
	if (val_status < 0){
		printf("\n trigger pulse 0 unsuccessful\n");
	}

	poll_struct.fd = fd1;
	poll_struct.events = POLLPRI;
	poll_struct.revents = 0;
	ret1 = poll(&poll_struct, 1, timeout);
	
	if(ret1<0){
		printf("error pollreturn\n" );

	}
//	if (ret1 > 0){
	if (poll_struct.revents & POLLPRI)
	{
	//	printf("revents = %d\n", poll_struct.revents);
		rise = rdtsc();
		read(fd1,&c,1);

//		}
//	}

	write(fd,"falling",7);
	// close(fd1);
	// fd1 = open("/sys/class/gpio/gpio14/value", O_RDONLY);
//	poll_struct.revents = 0;
	// poll_struct.revents = 0;
	ret2 = poll(&poll_struct, 1, timeout);
	if(ret2<0){
	printf("error pollreturn\n" );
	}	
//	if (ret2 > 0){
		if (poll_struct.revents & POLLPRI)
		{
		//	printf("revents = %d\n", poll_struct.revents);		
			fall = rdtsc();
			read(fd1,&c,1);
			dis = ((fall - rise)/400)*0.017;
			printf("Distance is %f cm\n\n",dis);
		}
	}
		
	

	/*printf("rise tsc = %llu\n",rise);
	printf("return for rise poll = %d\n",ret1);
	printf("fall tsc = %llu\n",fall);
	printf("return for fall poll = %d\n",ret2);
 */   

    usleep(200000);

}

return NULL;
}

int main(){
	int thread_status;
	
	PinSetup();


	thread_status = pthread_create(&thread_id, NULL, &distance, NULL);
		if(thread_status != 0){
			printf("thread create error status");
		}


	thread_status = pthread_join(thread_id, NULL);
			if(thread_status != 0){
				printf("thread join error status\n ");
			}
return 0;

}

