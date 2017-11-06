#include<stdio.h>
#include<sys/stat.h> 
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<pthread.h>
#include<linux/spi/spidev.h>

#include"Gpio_func.h"
#include"Gpio_func.c"

//Macros
#define SPIDEV "/dev/spidev1.0"

//Globals
int MOSI_GPIO = 5, MOSI_LS = 24, MOSI_PULLUP = 25, MOSI_MUX = 44;
int CS_GPIO  = 15, CS_LS = 42, CS_PULLUP = 43;//, CS_MUX;
int SCK_GPIO = 7, SCK_LS = 30, SCK_PULLUP = 31, SCK_MUX = 46;

	/*						GPIO	LS 		PULL_UP		MUX
	IO11 -> MOSI		| GPIO 5	24(L) 	25(L)		44(H)
	IO12 -> MISO/CS 	| GPIO 15	42(L) 	43(L)		-
	IO13 -> SCK 		| GPIO 07	30(L) 	31(L)		46(H)
	*/
pthread_t  thread_id[2];

void IOsetup(void){

	int FdExport, SetDir, SetVal;
	
	//---------------GPIO PINS---------------------//
	FdExport = gpio_export(MOSI_GPIO);
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

	FdExport = gpio_export(CS_GPIO);
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

	FdExport = gpio_export(SCK_GPIO);
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

	//---------------LS PINS-----------------------//
	FdExport = gpio_export(MOSI_LS);
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

	FdExport = gpio_export(CS_LS);
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

	FdExport = gpio_export(SCK_LS);
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

	//------------PULLUP PINS----------------------//

	FdExport = gpio_export(MOSI_PULLUP);
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

	FdExport = gpio_export(CS_PULLUP);
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

	FdExport = gpio_export(SCK_PULLUP);
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

	FdExport = gpio_export(MOSI_MUX);
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

	FdExport = gpio_export(SCK_MUX);
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

}

void IOclose(void){
	int FdUnExport;
	
	//--------------GPIO------------------------//
	FdUnExport = gpio_unexport(MOSI_GPIO);
	if(FdUnExport != 0){
		printf("MOSI_GPIO Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(CS_GPIO);
	if(FdUnExport != 0){
		printf("CS_GPIO Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(SCK_GPIO);
	if(FdUnExport != 0){
		printf("SCK_GPIO Pin UnExport error \n");
	}
	//--------------LS-----------------------//
	FdUnExport = gpio_unexport(MOSI_LS);
	if(FdUnExport != 0){
		printf("MOSI_LS Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(CS_LS);
	if(FdUnExport != 0){
		printf("CS_LS Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(SCK_LS);
	if(FdUnExport != 0){
		printf("SCK_LS Pin UnExport error \n");
	}
	//--------------PULLUP---------------------//
	FdUnExport = gpio_unexport(MOSI_PULLUP);
	if(FdUnExport != 0){
		printf("MOSI_PULLUP Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(CS_PULLUP);
	if(FdUnExport != 0){
		printf("CS_PULLUP Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(SCK_PULLUP);
	if(FdUnExport != 0){
		printf("SCK_PULLUP Pin UnExport error \n");
	}
	//--------------MUX------------------------//
	FdUnExport = gpio_unexport(MOSI_MUX);
	if(FdUnExport != 0){
		printf("MOSI_MUX Pin UnExport error \n");
	}
	FdUnExport = gpio_unexport(SCK_MUX);
	if(FdUnExport != 0){
		printf("SCK_MUX Pin UnExport error \n");
	}
}

void *Display(){

	int fdLed = 0,return_value = 0;
	int fdCS, len,i;
	char buf[64];

	unsigned char sentMsg[2] = {0,0}, receiveMsg[2] = {0,0};

	unsigned char StillRight[] = {
								0x0F,0x00,	//Display test
								0x0C,0x01,	//Shutdown					
								0x09,0x00,	//Decode
								0x0A,0x02,	//Intensity
								0x0B,0x07,	//Scan Limit
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
								0x0F,0x00,	//Display test
								0x0C,0x01,	//Shutdown					
								0x09,0x00,	//Decode
								0x0A,0x02,	//Intensity
								0x0B,0x07,	//Scan Limit
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
								0x0F,0x00,	//Display test
								0x0C,0x01,	//Shutdown					
								0x09,0x00,	//Decode
								0x0A,0x02,	//Intensity
								0x0B,0x07,	//Scan Limit
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
								0x0F,0x00,	//Display test
								0x0C,0x01,	//Shutdown					
								0x09,0x00,	//Decode
								0x0A,0x02,	//Intensity
								0x0B,0x07,	//Scan Limit
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

	for(i=0;i<26;i = i+2){
		
		sentMsg[0] = StillRight[i]; 					//MSB
		sentMsg[1] = StillRight[i+1]; 					//LSB
		return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
		if(return_value < 0){
			printf("error in ioctl\n");
		}	
		write(fdCS, "1", 1);							//setting clock high will push data from shift register
		write(fdCS, "0", 1);							//setting clock low will make shift register ready to receive input
	}
	sleep(5);

	for(i=0;i<26;i = i+2){
		
		sentMsg[0] = RunRight[i]; 						//MSB
		sentMsg[1] = RunRight[i+1]; 					//LSB
		return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
		if(return_value < 0){
			printf("error in ioctl\n");
		}	
		write(fdCS, "1", 1);							//setting clock high will push data from shift register
		write(fdCS, "0", 1);							//setting clock low will make shift register ready to receive input
	}
	sleep(5);

	for(i=0;i<26;i = i+2){
		
		sentMsg[0] = StillLeft[i]; 						//MSB
		sentMsg[1] = StillLeft[i+1]; 					//LSB
		return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
		if(return_value < 0){
			printf("error in ioctl\n");
		}	
		write(fdCS, "1", 1);							//setting clock high will push data from shift register
		write(fdCS, "0", 1);							//setting clock low will make shift register ready to receive input
	}
	sleep(5);

	for(i=0;i<26;i = i+2){
		
		sentMsg[0] = RunLeft[i]; 						//MSB
		sentMsg[1] = RunLeft[i+1]; 						//LSB
		return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
		if(return_value < 0){
			printf("error in ioctl\n");
		}	
		write(fdCS, "1", 1);							//setting clock high will push data from shift register
		write(fdCS, "0", 1);							//setting clock low will make shift register ready to receive input
	}
	sleep(5);

	close(fdLed);

return NULL;
}

int main(){

	int i;
	int thread_return[2];
	//setting up IO pins
	IOsetup();

	//Thread for LED Display
	thread_return[0] = pthread_create( &thread_id[0], NULL, &Display, NULL);
	if(thread_return[0] != 0){
		printf("Display thread create error");
	}
	else
	printf("Display thread created\n");

/*	//Thread for Distance sensor
	thread_return[1] = pthread_create( &thread_id[1], NULL, &DistanceSensor, NULL);
	if(thread_return[1] != 0){
		printf("Distance Sensor thread create error");
	}
	else
	printf("Distance Sensor thread created\n");

	for(i=0;i<2;i++){
		thread_return[i] = pthread_join(thread_id[i], NULL);
		if(thread_return[i] != 0){
			printf("thread join error\n");
		}
	}
*/
	thread_return[0] = pthread_join(thread_id[0], NULL);
	if(thread_return[0] != 0){
		printf("thread join error\n");
	}	

	printf("terminating\n");
	//resetting IO pins
	IOclose();
	
	return 0;
}