#include<stdio.h>
#include<sys/stat.h> 
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<pthread.h>
#include<linux/spi/spidev.h>
#include <poll.h>

#include"Gpio_func.h"
#include"Gpio_func.c"

//Macros
#define SPIDEV "/dev/spidev1.0"

//Globals
int MOSI_GPIO = 5, MOSI_LS = 24, MOSI_PULLUP = 25, MOSI_MUX = 44;
int CS_GPIO  = 15, CS_LS = 42, CS_PULLUP = 43;//, CS_MUX;
int SCK_GPIO = 7, SCK_LS = 30, SCK_PULLUP = 31, SCK_MUX = 46;

int TRIG_GPIO = 13, TRIG_LS = 34, TRIG_MUX = 77;
int ECHO_GPIO = 14, ECHO_LS = 16, ECHO_MUX = 76;

pthread_t  thread_id[2];
pthread_mutex_t MutexLock = PTHREAD_MUTEX_INITIALIZER;

int timeout = 1000;
double CurrDistance, PrevDistance = 0;
char Direction = 'R'; 	// Direction = 'L' || LEFT
						// Direction = 'R' || RIGHT

int RunTime = 45;		//RunTime for program 45 seconds
int TerminationFlag = 0;

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
	FdExport = gpio_export(MOSI_GPIO);							//MOSI GPIO
	if(FdExport < 0){
		printf("MOSI_GPIO export error\n");		
	}
	SetDir = gpio_set_dir(MOSI_GPIO, 1);
	if (SetDir < 0){
		printf("\n MOSI_GPIO direction set failed\n");
	}
	SetVal = gpio_set_value(MOSI_GPIO, 1);
	if (SetVal < 0){
		printf("\n MOSI_GPIO Value set failed\n");
	}

	FdExport = gpio_export(CS_GPIO);							//CHIP SELECT GPIO
	if(FdExport < 0){
		printf("CS_GPIO export error\n");		
	}	
	SetDir = gpio_set_dir(CS_GPIO, 1);
	if (SetDir < 0){
		printf("\n CS_GPIO direction set failed\n");
	}
	SetVal = gpio_set_value(CS_GPIO, 0);
	if (SetVal < 0){
		printf("\n CS_GPIO Value set failed\n");
	}

	FdExport = gpio_export(SCK_GPIO);							//CLOCK GPIO
	if(FdExport < 0){
		printf("SCK_GPIO export error\n");		
	}	
	SetDir = gpio_set_dir(SCK_GPIO, 1);
	if (SetDir < 0){
		printf("\n SCK_GPIO direction set failed\n");
	}
	SetVal = gpio_set_value(SCK_GPIO, 1);
	if (SetVal < 0){
		printf("\n SCK_GPIO Value set failed\n");
	}

	FdExport = gpio_export(TRIG_GPIO);							//TRIGGER GPIO
	if(FdExport < 0){
		printf("TRIG_GPIO export error\n");		
	}
	SetDir = gpio_set_dir(TRIG_GPIO, 1);
	if (SetDir < 0){
		printf("\n TRIG_GPIO direction set failed\n");
	}
	
	FdExport = gpio_export(ECHO_GPIO);							//ECHO GPIO
	if(FdExport < 0){
		printf("ECHO_GPIO export error\n");		
	}	
	SetDir = gpio_set_dir(ECHO_GPIO, 0);
	if (SetDir < 0){
		printf("\n ECHO_GPIO direction set failed\n");
	}
	//---------------LS PINS-----------------------//
	FdExport = gpio_export(MOSI_LS);							//MOSI LEVEL SHIFTER
	if(FdExport < 0){
		printf("MOSI_LS export error\n");		
	}	
	SetDir = gpio_set_dir(MOSI_LS, 1);
	if (SetDir < 0){
		printf("\n MOSI_LS direction set failed\n");
	}
	SetVal = gpio_set_value(MOSI_LS, 0);
	if (SetVal < 0){
		printf("\n MOSI_LS Value set failed\n");
	}

	FdExport = gpio_export(CS_LS);								//CHIP SELECT LEVEL SHIFTER
	if(FdExport < 0){
		printf("CS_LS export error\n");		
	}	
	SetDir = gpio_set_dir(CS_LS, 1);
	if (SetDir < 0){
		printf("\n CS_LS direction set failed\n");
	}
	SetVal = gpio_set_value(CS_LS, 0);
	if (SetVal < 0){
		printf("\n CS_LS Value set failed\n");
	}

	FdExport = gpio_export(SCK_LS);								//CLOCK LEVEL SHIFTER
	if(FdExport < 0){
		printf("SCK_LS export error\n");		
	}	
	SetDir = gpio_set_dir(SCK_LS, 1);
	if (SetDir < 0){
		printf("\n SCK_LS direction set failed\n");
	}
	SetVal = gpio_set_value(SCK_LS, 0);
	if (SetVal < 0){
		printf("\n SCK_LS Value set failed\n");
	}

	FdExport = gpio_export(TRIG_LS);							//TRIGGER LEVEL SHIFTER
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

	FdExport = gpio_export(ECHO_LS);							//ECHO LEVEL SHIFTER
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

	//------------PULLUP PINS----------------------//

	FdExport = gpio_export(MOSI_PULLUP);						//MOSI PULLUP
	if(FdExport < 0){
		printf("MOSI_PULLUP export error\n");		
	}	
	SetDir = gpio_set_dir(MOSI_PULLUP, 1);
	if (SetDir < 0){
		printf("\n MOSI_PULLUP direction set failed\n");
	}
	SetVal = gpio_set_value(MOSI_PULLUP, 0);
	if (SetVal < 0){
		printf("\n MOSI_PULLUP Value set failed\n");
	}

	FdExport = gpio_export(CS_PULLUP);							//CHIP SELECT PULLUP
	if(FdExport < 0){
		printf("CS_PULLUP export error\n");		
	}	
	SetDir = gpio_set_dir(CS_PULLUP, 1);
	if (SetDir < 0){
		printf("\n CS_PULLUP direction set failed\n");
	}
	SetVal = gpio_set_value(CS_PULLUP, 0);
	if (SetVal < 0){
		printf("\n CS_PULLUP Value set failed\n");
	}

	FdExport = gpio_export(SCK_PULLUP);							//CLOCK PULLUP
	if(FdExport < 0){
		printf("SCK_PULLUP export error\n");		
	}	
	SetDir = gpio_set_dir(SCK_PULLUP, 1);
	if (SetDir < 0){
		printf("\n SCK_PULLUP direction set failed\n");
	}
	SetVal = gpio_set_value(SCK_PULLUP, 0);
	if (SetVal < 0){
		printf("\n SCK_PULLUP Value set failed\n");
	}

	//--------------MUX PINS-----------------------//

	FdExport = gpio_export(MOSI_MUX);							//MOSI MUX
	if(FdExport < 0){
		printf("MOSI_MUX export error\n");		
	}	
	SetDir = gpio_set_dir(MOSI_MUX, 1);
	if (SetDir < 0){
		printf("\n MOSI_MUX direction set failed\n");
	}
	SetVal = gpio_set_value(MOSI_MUX, 1);
	if (SetVal < 0){
		printf("\n MOSI_MUX Value set failed\n");
	}

	FdExport = gpio_export(SCK_MUX);							//CLOCK MUX
	if(FdExport < 0){
		printf("SCK_MUX export error\n");		
	}	
	SetDir = gpio_set_dir(SCK_MUX, 1);
	if (SetDir < 0){
		printf("\n SCK_MUX direction set failed\n");
	}
	SetVal = gpio_set_value(SCK_MUX, 1);
	if (SetVal < 0){
		printf("\n SCK_MUX Value set failed\n");
	}

	FdExport = gpio_export(TRIG_MUX);							//TRIGGER MUX
	if(FdExport < 0){
		printf("TRIG_MUX export error\n");		
	}	
	
	SetVal = gpio_set_value(TRIG_MUX, 0);
	if (SetVal < 0){
		printf("\n TRIG_MUX Value set failed\n");
	}

	FdExport = gpio_export(ECHO_MUX);							//ECHO MUX
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
	FdUnExport = gpio_unexport(MOSI_GPIO);						//MOSI GPIO
	if(FdUnExport != 0){
		printf("MOSI_GPIO Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(CS_GPIO);						//CHIP SELECT GPIO
	if(FdUnExport != 0){
		printf("CS_GPIO Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(SCK_GPIO);						//CLOCK GPIO
	if(FdUnExport != 0){
		printf("SCK_GPIO Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(TRIG_GPIO);						//TRIGGER GPIO
	if(FdUnExport != 0){
		printf("TRIG_GPIO Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(ECHO_GPIO);						//ECHO GPIO
	if(FdUnExport != 0){
		printf("ECHO_GPIO Pin UnExport error \n");
	}
	//--------------LS-----------------------//
	FdUnExport = gpio_unexport(MOSI_LS);						//MOSI LEVEL SHIFTER
	if(FdUnExport != 0){
		printf("MOSI_LS Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(CS_LS);							//CHIP SELECT LEVEL SHIFTER
	if(FdUnExport != 0){
		printf("CS_LS Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(SCK_LS);							//CLOCK LEVEL SHIFTER
	if(FdUnExport != 0){
		printf("SCK_LS Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(TRIG_LS);						//TRIGGER LEVEL SHIFTER
	if(FdUnExport != 0){
		printf("TRIG_LS Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(ECHO_LS);						//ECHO LEVEL SHIFTER
	if(FdUnExport != 0){
		printf("ECHO_LS Pin UnExport error \n");
	}
	//--------------PULLUP---------------------//
	FdUnExport = gpio_unexport(MOSI_PULLUP);					//MOSI PULLUP
	if(FdUnExport != 0){
		printf("MOSI_PULLUP Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(CS_PULLUP);						//CHIP SELECT PULLUP
	if(FdUnExport != 0){
		printf("CS_PULLUP Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(SCK_PULLUP);						//CLOCK PULLUP
	if(FdUnExport != 0){
		printf("SCK_PULLUP Pin UnExport error \n");
	}
	//--------------MUX------------------------//
	FdUnExport = gpio_unexport(MOSI_MUX);						//MOSI MUX
	if(FdUnExport != 0){
		printf("MOSI_MUX Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(SCK_MUX);						//CHIP SELECT MUX
	if(FdUnExport != 0){
		printf("SCK_MUX Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(TRIG_MUX);						//TRIGGER MUX
	if(FdUnExport != 0){
		printf("TRIG_MUX Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(ECHO_MUX);						//ECHO MUX
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
		
	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", TRIG_GPIO);
	if(len<0){
		printf("snprintf TRIG_GPIO Value error \n");
	}		
	fdTrigValue = open(buf, O_WRONLY);						//Opening Trigger pins' VALUE file

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/edge", ECHO_GPIO);
	if(len<0){
		printf("snprintf ECHO_GPIO Edge error \n");
	}		
	fdEchoEdge = open(buf, O_WRONLY);						//Opening Echo pins' EDGE file

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", ECHO_GPIO);
	if(len<0){
		printf("snprintf ECHO_GPIO Value error \n");
	}		
	fdEchoValue = open(buf, O_RDONLY);						//Opening Echo pins' VALUE file

	poll_struct.fd = fdEchoValue;							//Initializing poll structure
	poll_struct.events = POLLPRI;
	poll_struct.revents = 0;

	while(1)
	{		
		if(TerminationFlag == 1){							//Checking 'TeriminationFlag' set or not
			break;
		}	
		lseek(poll_struct.fd, 0, SEEK_SET);					//Resetting file pointer	
		write(fdEchoEdge,"rising",sizeof("rising"));		
		
		poll_struct.revents = 0;

		write(fdTrigValue,"1",sizeof("1"));					//Sending Trigger Pulse
		usleep(15);
		write(fdTrigValue,"0",sizeof("0"));
		
		pollReturn = poll(&poll_struct, 1, timeout);		//Capturing POLL event for 'Rising' Edge
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
			pollReturn = poll(&poll_struct, 1, timeout);	//Capturing POLL event for 'Falling' Edge
			if(pollReturn < 0){
				printf("error pollReturn\n" );
			}
			if (poll_struct.revents & POLLPRI)
			{						
				fall = rdtsc();								//Falling Edge detected											
				read(poll_struct.fd,&c,1);					//reading dummy value from file to clear it	
				
				pthread_mutex_lock(&MutexLock);
				CurrDistance = ((fall - rise)/400)*0.017;	//Computing the current distance value
				pthread_mutex_unlock(&MutexLock);

				/*printf("\nDistance is %f cm\n",CurrDistance);
				printf("\nDirection is %c \n\n",Direction);	*/			
			}	
			else{
				printf("Falling Edge detection error\n");
			}		
		}
		else{
			printf("Rising Edge detection error\n");
		}		
	    usleep(1000000);
	    
	    
	}

	close(fdTrigValue);
	close(fdEchoEdge);
	close(fdEchoValue);
	return NULL;
}

void *Display(){

	int fdLed = 0,return_value = 0;
	int fdCS, len,i;
	char buf[64];
	int SleepTime = 0;

	unsigned char sentMsg[2] = {0,0}, receiveMsg[2] = {0,0};

	unsigned char ResetRegisters[] = {
								0x0F,0x00,	//Display test
								0x0C,0x01,	//Shutdown					
								0x09,0x00,	//Decode
								0x0A,0x02,	//Intensity
								0x0B,0x07	//Scan Limit
							};

	unsigned char StillRight[] = {
								0x01,0x06,
								0x02,0x06,
								0x03,0x06,
								0x04,0x83,
								0x05,0x7F,
								0x06,0x24,
								0x07,0x22,
								0x08,0x63
							};

	unsigned char StillLeft[] = {
								0x01,0x60,
								0x02,0x60,
								0x03,0x60,
								0x04,0xC1,
								0x05,0xFE,
								0x06,0x24,
								0x07,0x44,
								0x08,0xC6
							};

	unsigned char RunRight[] = {
								0x01,0x06,
								0x02,0x06,
								0x03,0x06,
								0x04,0x03,
								0x05,0x7F,
								0x06,0xA2,
								0x07,0x32,
								0x08,0x16
							};

	unsigned char RunLeft[] = {
								0x01,0x60,
								0x02,0x60,	
								0x03,0x60,
								0x04,0xC0,
								0x05,0xFE,
								0x06,0x45,
								0x07,0x4C,
								0x08,0x68
							};

	struct spi_ioc_transfer  message_structure = {
		.tx_buf = (unsigned long)sentMsg,
		.rx_buf = (unsigned long)receiveMsg,
		.len = sizeof(sentMsg), //array size
		.cs_change = 1,
		.speed_hz = 10000000,
		.bits_per_word = 8,
	};

	fdLed = open(SPIDEV,O_WRONLY);
	if(fdLed<0){
		printf("error in opening spi device\n");
	}

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", CS_GPIO);
	if(len<0){
		printf("snprintf error\n");
	}		
	fdCS = open(buf, O_WRONLY);

	//Resetting all the registers
	for(i=0;i<10;i = i+2){
		
		sentMsg[0] = ResetRegisters[i]; 						//MSB
		sentMsg[1] = ResetRegisters[i+1]; 						//LSB
		return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
		if(return_value < 0){
			printf("error in ioctl\n");
		}	
		write(fdCS, "1", 1);									//setting clock high will push data from shift register
		write(fdCS, "0", 1);									//setting clock low will make shift register ready to receive input
	}

	while(1){
		if(TerminationFlag == 1){								//Checking 'TeriminationFlag' set or not
			break;
		}

		pthread_mutex_lock(&MutexLock);							//Computing the 'Direction' of person moving
		if(PrevDistance - CurrDistance > 1){					//Moving close to sensor		
			Direction = 'R';
		}
		else
			if(PrevDistance - CurrDistance < -1){				//Moving far from sensor		
			Direction = 'L';
		}
		PrevDistance = CurrDistance;
		pthread_mutex_unlock(&MutexLock);

		if(Direction == 'R'){
			SleepTime = 5000*(int)PrevDistance;
			if(SleepTime > 1000000){							//If 'SleepTime' computed is greater than sensor sleep time reset it to smaller value
				SleepTime = 900000;
			}
			for(i=0;i<16;i = i+2){
				
				sentMsg[0] = StillRight[i]; 					//MSB
				sentMsg[1] = StillRight[i+1]; 					//LSB
				return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
				if(return_value < 0){
					printf("error in ioctl\n");
				}	
				write(fdCS, "1", 1);							//setting clock high will push data from shift register
				write(fdCS, "0", 1);							//setting clock low will make shift register ready to receive input
			}
			
			usleep(SleepTime);

			for(i=0;i<16;i = i+2){
				
				sentMsg[0] = RunRight[i]; 						//MSB
				sentMsg[1] = RunRight[i+1]; 					//LSB
				return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
				if(return_value < 0){
					printf("error in ioctl\n");
				}	
				write(fdCS, "1", 1);							//setting clock high will push data from shift register
				write(fdCS, "0", 1);							//setting clock low will make shift register ready to receive input
			}
			
			usleep(SleepTime);
		}
		else if(Direction == 'L'){
			SleepTime = 8000*(int)PrevDistance;
			if(SleepTime > 1000000){							//If 'SleepTime' computed is greater than sensor sleep time reset it to smaller value
				SleepTime = 900000;
			}
			for(i=0;i<16;i = i+2){
				
				sentMsg[0] = StillLeft[i]; 						//MSB
				sentMsg[1] = StillLeft[i+1]; 					//LSB
				return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
				if(return_value < 0){
					printf("error in ioctl\n");
				}	
				write(fdCS, "1", 1);							//setting clock high will push data from shift register
				write(fdCS, "0", 1);							//setting clock low will make shift register ready to receive input
			}
			
			usleep(SleepTime);

			for(i=0;i<16;i = i+2){
				
				sentMsg[0] = RunLeft[i]; 						//MSB
				sentMsg[1] = RunLeft[i+1]; 						//LSB
				return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
				if(return_value < 0){
					printf("error in ioctl\n");
				}	
				write(fdCS, "1", 1);							//setting clock high will push data from shift register
				write(fdCS, "0", 1);							//setting clock low will make shift register ready to receive input
			}
			
			usleep(SleepTime);
		}		
	}
	close(fdLed);
	close(fdCS);

	return NULL;
}

int main(){

	int thread_return;
	
	IOsetup();												//setting up IO pins

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

	sleep(RunTime);											//Suspend main program Activity till executing duration

	TerminationFlag = 1;									//As 'RunTime' expires 'TerminationFlag' is set to 1
	
	thread_return = pthread_join(thread_id[1], NULL);
	if(thread_return != 0){
		printf("thread join error\n");
	}	

	thread_return = pthread_join(thread_id[0], NULL);
	if(thread_return != 0){
		printf("thread join error\n");
	}	

	printf("\nRunTime Expired\nTerminating...\n");
	
	IOclose();												//resetting IO pins
	
	return 0;
}