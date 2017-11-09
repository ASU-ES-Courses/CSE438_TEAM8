#include<stdio.h>
#include<sys/stat.h> 
#include<fcntl.h>
#include<unistd.h>
//#include<sys/ioctl.h>
#include<pthread.h>
//#include<linux/spi/spidev.h>

#include <poll.h>

#include"Gpio_func.h"
#include"Gpio_func.c"

//Macros

//Globals
int TRIG_GPIO = 13, TRIG_LS = 34, TRIG_MUX = 77;
int ECHO_GPIO = 14, ECHO_LS = 16, ECHO_MUX = 76;
	/*					GPIO	LS 		PULL_UP		MUX
	IO2 -> TRIG		| 	13		34(L) 	35(L)		77(L)
					|	61		-		"			"
	IO3 -> ECHO 	| 	14		16(H) 	17(L)		76(L)
					|	62		-		"			"	 64(L)
	
	*/
	/*						GPIO	LS 		PULL_UP		MUX
	IO11 -> MOSI		| GPIO 5	24(L) 	25(L)		44(H)
	IO12 -> MISO/CS 	| GPIO 15	42(L) 	43(L)		-
	IO13 -> SCK 		| GPIO 07	30(L) 	31(L)		46(H)
	*/
pthread_t  thread_id[2];
int timeout = 1000;
double CurrDistance, PrevDistance = 0;
char Direction = 'N'; 	// Direction = 'N' || NONE
						// Direction = 'L' || LEFT
						// Direction = 'R' || RIGHT

//rdtsc function definition
//Reference -> https://www.mcs.anl.gov/~kazutomo/rdtsc.html
static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

void IOsetup(void){

	int FdExport, SetDir, SetVal;
	
	//---------------GPIO PINS---------------------//
	FdExport = gpio_export(TRIG_GPIO);
	if(FdExport < 0){
		printf("TRIG_GPIO export error\n");		
	}
	SetDir = gpio_set_dir(TRIG_GPIO, 1);
	if (SetDir < 0){
		printf("\n TRIG_GPIO direction set failed\n");
	}
	
	FdExport = gpio_export(ECHO_GPIO);
	if(FdExport < 0){
		printf("ECHO_GPIO export error\n");		
	}	
	SetDir = gpio_set_dir(ECHO_GPIO, 0);
	if (SetDir < 0){
		printf("\n ECHO_GPIO direction set failed\n");
	}

	//---------------LS PINS-----------------------//
	FdExport = gpio_export(TRIG_LS);
	if(FdExport < 0){
		printf("TRIG_LS export error\n");		
	}	
	SetDir = gpio_set_dir(TRIG_LS, 1);
	if (SetDir < 0){
		printf("\n TRIG_LS direction set failed\n");
	}
	SetVal = gpio_set_value(TRIG_LS, 0);
	if (SetVal < 0){
		printf("\n TRIG_LS Value set failed\n");
	}

	FdExport = gpio_export(ECHO_LS);
	if(FdExport < 0){
		printf("ECHO_LS export error\n");		
	}	
	SetDir = gpio_set_dir(ECHO_LS, 1);
	if (SetDir < 0){
		printf("\n ECHO_LS direction set failed\n");
	}
	SetVal = gpio_set_value(ECHO_LS, 1);
	if (SetVal < 0){
		printf("\n ECHO_LS Value set failed\n");
	}

	//--------------MUX PINS-----------------------//

	FdExport = gpio_export(TRIG_MUX);
	if(FdExport < 0){
		printf("TRIG_MUX export error\n");		
	}	
	
	SetVal = gpio_set_value(TRIG_MUX, 0);
	if (SetVal < 0){
		printf("\n TRIG_MUX Value set failed\n");
	}

	FdExport = gpio_export(ECHO_MUX);
	if(FdExport < 0){
		printf("ECHO_MUX export error\n");		
	}	
	
	SetVal = gpio_set_value(ECHO_MUX, 0);
	if (SetVal < 0){
		printf("\n ECHO_MUX Value set failed\n");
	}
}

void IOclose(void){
	int FdUnExport;
	
	//--------------GPIO------------------------//
	FdUnExport = gpio_unexport(TRIG_GPIO);
	if(FdUnExport != 0){
		printf("TRIG_GPIO Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(ECHO_GPIO);
	if(FdUnExport != 0){
		printf("ECHO_GPIO Pin UnExport error \n");
	}
	//--------------LS-----------------------//
	FdUnExport = gpio_unexport(TRIG_LS);
	if(FdUnExport != 0){
		printf("TRIG_LS Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(ECHO_LS);
	if(FdUnExport != 0){
		printf("ECHO_LS Pin UnExport error \n");
	}
	//--------------MUX------------------------//
	FdUnExport = gpio_unexport(TRIG_MUX);
	if(FdUnExport != 0){
		printf("TRIG_MUX Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(ECHO_MUX);
	if(FdUnExport != 0){
		printf("ECHO_MUX Pin UnExport error \n");
	}
}

void *DistanceSensor(){

	int fdEchoEdge, fdEchoValue, fdTrigValue;
	int pollReturn;
	unsigned long long rise,fall;	
	int len;
	char buf[64], c; 										// 'c' is a dummy variable
	struct pollfd poll_struct;								//object of poll structure
		
	//Opening Trigger pins' VALUE file
	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", TRIG_GPIO);
	if(len<0){
		printf("snprintf TRIG_GPIO Value error \n");
	}		
	fdTrigValue = open(buf, O_WRONLY);

	//Opening Echo pins' EDGE file
	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/edge", ECHO_GPIO);
	if(len<0){
		printf("snprintf ECHO_GPIO Edge error \n");
	}		
	fdEchoEdge = open(buf, O_WRONLY);

	//Opening Echo pins' VALUE file
	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", ECHO_GPIO);
	if(len<0){
		printf("snprintf ECHO_GPIO Value error \n");
	}		
	fdEchoValue = open(buf, O_RDONLY);

	//initializing poll structure
	poll_struct.fd = fdEchoValue;
	poll_struct.events = POLLPRI;
	poll_struct.revents = 0;

	while(1)
	{			
		lseek(poll_struct.fd, 0, SEEK_SET);					//Resetting file pointer	
		write(fdEchoEdge,"rising",sizeof("rising"));		//Specifying EDGE = "rising" on edge file	
		
		poll_struct.revents = 0;

		//Sending Trigger Pulse
		write(fdTrigValue,"1",sizeof("1"));		
		usleep(15);
		write(fdTrigValue,"0",sizeof("0"));
		
		pollReturn = poll(&poll_struct, 1, timeout);		
		if(pollReturn < 0){
			printf("error pollReturn\n" );
		}
	
		if (poll_struct.revents & POLLPRI)
		{			
			rise = rdtsc();									//Rising Edge detected			
			read(poll_struct.fd,&c,1);						//reading dummy value from file to clear it		

			write(fdEchoEdge,"falling",sizeof("falling"));
			
			lseek(poll_struct.fd, 0, SEEK_SET);				//Resetting file pointer

			poll_struct.revents = 0;
			pollReturn = poll(&poll_struct, 1, timeout);
			if(pollReturn < 0){
				printf("error pollReturn\n" );
			}
			if (poll_struct.revents & POLLPRI)
			{						
				fall = rdtsc();								//Falling Edge detected											
				read(poll_struct.fd,&c,1);					//reading dummy value from file to clear it	
				//apply mutex
				CurrDistance = ((fall - rise)/400)*0.017;
				if(PrevDistance - CurrDistance > 1){
					//Moving close
					Direction = 'R';
				}
				else
					if(PrevDistance - CurrDistance < -1){
					//Moving far
					Direction = 'L';
				}
				PrevDistance = CurrDistance;
				printf("\nDistance is %f cm\n",CurrDistance);
				printf("\nDirection is %c \n\n",Direction);	
				//release mutex						
			}	
			else{
				printf("Falling Edge detection error\n");
			}		
		}
		else{
			printf("Rising Edge detection error\n");
		}		

	    usleep(200000);
	    
	}
	close(fdTrigValue);
	close(fdEchoEdge);
	close(fdEchoValue);
	return NULL;
}

int main(){

	int i;
	int thread_return[2];
	//setting up IO pins
	IOsetup();

	//Thread for LED Display
	/*thread_return[0] = pthread_create( &thread_id[0], NULL, &Display, NULL);
	if(thread_return[0] != 0){
		printf("Display thread create error");
	}
	else
	printf("Display thread created\n");*/

	//Thread for Distance sensor
	thread_return[1] = pthread_create( &thread_id[1], NULL, &DistanceSensor, NULL);
	if(thread_return[1] != 0){
		printf("Distance Sensor thread create error");
	}
	else
	printf("Distance Sensor thread created\n");

	/*for(i=0;i<2;i++){
		thread_return[i] = pthread_join(thread_id[i], NULL);
		if(thread_return[i] != 0){
			printf("thread join error\n");
		}
	}

	thread_return[0] = pthread_join(thread_id[0], NULL);
	if(thread_return[0] != 0){
		printf("thread join error\n");
	}*/
	/*while(1){
		printf("In main\n");
		printf("CurrDistance = %f\n",CurrDistance);
		printf("PrevDistance = %f\n",PrevDistance);
		usleep(200000);
	}*/
	thread_return[1] = pthread_join(thread_id[1], NULL);
	if(thread_return[1] != 0){
		printf("thread join error\n");
	}	

	printf("terminating\n");
	//resetting IO pins
	IOclose();
	
	return 0;
}