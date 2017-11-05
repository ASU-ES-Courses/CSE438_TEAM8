#include<stdio.h>
#include<sys/stat.h> 
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
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

int main(){

	int fdLed = 0,return_value = 0;
	int fdMOSI, fdCS, fdSCK, len,i;
	char buf[64];

	/*unsigned char RunLeft[8] = {};
	unsigned char WalkLeft[8] = {};
	unsigned char RunRight[8] = {};
	unsigned char WalkLeft[8] = {};*/

	unsigned char sentMsg[2] = {0,0}, receiveMsg[2] = {0,0};
	unsigned char dataInput[] = {
								//0x0C,0x00, //shutdown								
								0x09,0x00,
								0x0A,0x0F,
								//0x0B,0x07,
								0x01,0x00,
								0x02,0x00,
								0x03,0x00,
								0x04,0x00,
								0x05,0x00,
								0x06,0x00,
								0x07,0x00,
								0x08,0x00,
								//0xFF,0x00	//to off all leds
								//0xFF,0x01	//to on all leds
								//0x0C,0x00	//shutdown
							};

	struct spi_ioc_transfer  message_structure = {
		.tx_buf = (unsigned long)sentMsg,
		.rx_buf = (unsigned long)receiveMsg,
		.len = sizeof(sentMsg), //array size
		.cs_change = 1,
		.speed_hz = 500000,
		.bits_per_word = 8,
	};

	fdLed = open(SPIDEV,O_WRONLY);
	if(fdLed<0){
		printf("error in opening spi device\n");
	}

	IOsetup();

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", MOSI_GPIO);
	if(len<0){
		printf("snprintf error\n");
	}	 
	fdMOSI = open(buf, O_WRONLY);

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", CS_GPIO);
	if(len<0){
		printf("snprintf error\n");
	}		
	fdCS = open(buf, O_WRONLY);

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", SCK_GPIO);
	if(len<0){
		printf("snprintf error\n");
	}
	fdSCK = open(buf, O_WRONLY);

	/*write(fdMOSI, "1", 1);
	
	write(fdSCK, "1", 1);*/

	for(i=0;i<20;i++){
		
		sentMsg[0] = dataInput[i]; //MSB
		sentMsg[1] = dataInput[i+1]; //LSB
		return_value = ioctl(fdLed,SPI_IOC_MESSAGE(1),&message_structure);
		if(return_value < 0){
			printf("error in ioctl\n");
		}	
		write(fdCS, "1", 1);
		write(fdCS, "0", 1);
	}
	sleep(5);
	printf("terminating\n");
	IOclose();
	close(fdLed);
	return 0;
}