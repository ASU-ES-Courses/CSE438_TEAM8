#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
// #include <pthread.h>
#include <poll.h>
#include<linux/input.h>
#include"Gpio_func.h"
#include"Gpio_func.c"

//Macros
#define EVENT_FILE_NAME "/dev/input/event6"

//Globals
int R_GPIO, R_LS, R_MUX;
int G_GPIO, G_LS, G_MUX;
int B_GPIO, B_LS, B_MUX;

//IOSetup function
void IOSetup(void)
{
	int FdExport, SetDir, SetVal;

	//------------GPIO Pins----------------------------//
	//----------LED 1----------------------//
	//Export 
	FdExport = gpio_export(R_GPIO);
	if(FdExport < 0){
		printf("export error LED 1\n");		
	}
	//Set Dir 
	SetDir = gpio_set_dir(R_GPIO, 1);
	if (SetDir < 0){
		printf("\n LED 1 direction set failed\n");
	}

	//----------LED 2----------------------//
	//Export
	FdExport = gpio_export(G_GPIO);
	if(FdExport < 0){
		printf("export error LED 2\n");		
	}
	//Set Dir 
	SetDir = gpio_set_dir(G_GPIO, 1);
	if (SetDir < 0){
		printf("\n LED 2 direction set failed\n");
	}

	//----------LED 3----------------------//
	//Export 
	FdExport = gpio_export(B_GPIO);
	if(FdExport < 0){
		printf("export error LED 3\n");		
	}
	//Set Dir 
	SetDir = gpio_set_dir(B_GPIO, 1);
	if (SetDir < 0){
		printf("\n LED 3 direction set failed\n");
	}
	//-------------------------------------------------//

	//------------LEVEL Shifter Pins-------------------//
	//----------LED 1----------------------//
	if(R_LS != -1){
		//Export 
		FdExport = gpio_export(R_LS);
		if(FdExport < 0){
			printf("export error LED 1 Level Shifter\n");		
		}
		//Set Dir 
		SetDir = gpio_set_dir(R_LS, 1);
		if (SetDir < 0){
			printf("\n LED 1 Level Shifter direction set failed\n");
		}
		//Set Val 
		SetVal = gpio_set_value(R_LS, 1);
		if (SetVal < 0){
			printf("\n LED 1 Value set failed\n");
		}
	}
	//----------LED 2----------------------//
	if(G_LS != -1){
		//Export
		FdExport = gpio_export(G_LS);
		if(FdExport < 0){
			printf("export error LED 2 Level Shifter\n");		
		}
		//Set Dir 
		SetDir = gpio_set_dir(G_LS, 1);
		if (SetDir < 0){
			printf("\n LED 2 Level Shifter direction set failed\n");
		}
		//Set Val 
		SetVal = gpio_set_value(G_LS, 1);
		if (SetVal < 0){
			printf("\n LED 2 Value set failed\n");
		}
	}
	//----------LED 3----------------------//
	if(B_LS != -1){
		//Export 
		FdExport = gpio_export(B_LS);
		if(FdExport < 0){
			printf("export error LED 3 Level Shifter\n");		
		}
		//Set Dir 
		SetDir = gpio_set_dir(B_LS, 1);
		if (SetDir < 0){
			printf("\n LED 3 Level Shifter direction set failed\n");
		}
		//Set Val 
		SetVal = gpio_set_value(B_LS, 1);
		if (SetVal < 0){
			printf("\n LED 3 Value set failed\n");
		}
	}
	//-------------------------------------------------//

	//------------------MUX Pins-----------------------//
	//----------LED 1----------------------//
	if(R_MUX != -1){
		//Export 
		FdExport = gpio_export(R_MUX);
		if(FdExport < 0){
			printf("export error LED 1 MUX pin\n");		
		}
		//Set Dir 
		if(R_MUX < 64 || R_MUX > 79){			
			SetDir = gpio_set_dir(R_MUX, 1);
			if (SetDir < 0){
				printf("\n LED 1 MUX pin direction set failed\n");
			}
		}
		//Set Val 
		SetVal = gpio_set_value(R_MUX, 1);
		if (SetVal < 0){
			printf("\n LED 1 MUX pin Value set failed\n");
		}
	}
	//----------LED 2----------------------//
	if(G_MUX != -1){
		//Export
		FdExport = gpio_export(G_MUX);
		if(FdExport < 0){
			printf("export error LED 2 MUX pin\n");		
		}
		//Set Dir
		if(G_MUX < 64 || G_MUX > 79){			 
			SetDir = gpio_set_dir(G_MUX, 1);
			if (SetDir < 0){
				printf("\n LED 2 MUX pin direction set failed\n");
			}
		}
		//Set Val 
		SetVal = gpio_set_value(G_MUX, 1);
		if (SetVal < 0){
			printf("\n LED 2 MUX pin Value set failed\n");
		}
	}
	//----------LED 3----------------------//
	if(B_MUX != -1){
		//Export 
		FdExport = gpio_export(B_MUX);
		if(FdExport < 0){
			printf("export error LED 3 MUX pin\n");		
		}
		//Set Dir 
		if(B_MUX < 64 || B_MUX > 79){			
			SetDir = gpio_set_dir(B_MUX, 1);
			if (SetDir < 0){
				printf("\n LED 3 MUX pin direction set failed\n");
			}
		}
		//Set Val 
		SetVal = gpio_set_value(B_MUX, 1);
		if (SetVal < 0){
			printf("\n LED 3 MUX pin Value set failed\n");
		}
	}
	//-------------------------------------------------//

}
//Mouse Event detection function
int mouse_click(){

	int status = 0, fd;
	struct input_event event;
	fd = open(EVENT_FILE_NAME, O_RDONLY);
	if (fd < 0){
	    printf("failed to open input device %s: %d\n", EVENT_FILE_NAME, errno);	    
	}

	while(read(fd,&event,sizeof(event))){
		
		if( (event.code == 272 && event.value == 1 ) || (event.code == 273 && event.value == 1) ){
			printf("Click event detected\n");
			status = 1;
			break;
		}
	}
	return status;
}

//IOClose function
void IOClose(void)
{
	int FdUnExport;
	FdUnExport = open("/sys/class/gpio/unexport", O_WRONLY);
	if (FdUnExport < 0){
		printf("\n gpio Unexport open failed\n");
	}

	//Red Led
	if(0> write(FdUnExport,&R_GPIO,3))
		printf("error FdUnExport 4\n");
	if(R_LS != -1){
		if(0> write(FdUnExport,&R_LS,2))
			printf("error FdUnExport 22\n");
	}
	if(R_MUX != -1){
		if(0> write(FdUnExport,&R_MUX,2))
			printf("error FdUnExport 70\n");
	}

	//Green Led
	if(0> write(FdUnExport,&G_GPIO,3))
		printf("error FdUnExport 10\n");
	if(G_LS != -1){
		if(0> write(FdUnExport,&G_LS,2))
			printf("error FdUnExport 26\n");
	}
	if(G_MUX != -1){
		if(0> write(FdUnExport,&G_MUX,2))
			printf("error FdUnExport 74\n");
	}

	//Blue Led
	if(0> write(FdUnExport,&B_GPIO,3))
		printf("error FdUnExport 7\n");
	if(B_LS != -1){
		if(0> write(FdUnExport,&B_LS,2))
			printf("error FdUnExport 30\n");
	}
	if(B_MUX != -1){
		if(0> write(FdUnExport,&B_MUX,2))
			printf("error FdUnExport 46\n");
	}

	close(FdUnExport);
}
//MAIN
int main(){
	
	int pwm_on = 0, pwm_off = 0, status = 0,i,j;
	int arr[4] = {0,0,0,0}; //[PWM,R,G,B]
	int LedR, LedG, LedB, len;
	char buf[64];
	// Array index represents IO port no.
	// -1 indicates corresponding pin to that IO port no. is not available.
	int GPIO_PIN[] = {11,12,13,14,6,0,1,38,40,4,10,5,15,7};
	int LS_PIN[] = {32,28,34,16,36,18,20,-1,-1,22,26,24,42,30};
	int MUX_PIN[] = {-1,45,77,76,-1,66,68,-1,-1,70,74,44,-1,46};

	printf("Enter input values in order [ percentage of PWM, R_IO, G_IO, B_IO ]\n");
	for(i=0;i<4;i++){
		scanf("%d",&arr[i]);
	}
	if( (arr[0] <= 100 && arr[0] > 0) && (arr[1] < 14 && arr[1] >= 0 ) && (arr[2] < 14 && arr[2] >= 0 ) && (arr[3] < 14 && arr[3] >= 0 ) ){

		pwm_on = ( 20 * 1000 * arr[0] ) / 100 ; // each pulse width is of 20ms ( 20*1000 micro sec )
		pwm_off = 20000 - pwm_on ;
		//Selecting GPIO Pins
		R_GPIO = GPIO_PIN[arr[1]];
		G_GPIO = GPIO_PIN[arr[2]];
		B_GPIO = GPIO_PIN[arr[3]];

		//Selecting Level Shifter Pins
		R_LS = LS_PIN[arr[1]];
		G_LS = LS_PIN[arr[2]];
		B_LS = LS_PIN[arr[3]];

		//Selecting MUX Pins
		R_MUX = MUX_PIN[arr[1]];
		G_MUX = MUX_PIN[arr[2]];
		B_MUX = MUX_PIN[arr[3]];

		IOSetup();

		len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", R_GPIO);
		if(len<0){
			printf("snprintf error\n");
		}	 
		LedR = open(buf, O_WRONLY);

		len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", G_GPIO);
		if(len<0){
			printf("snprintf error\n");
		}		
		LedG = open("/sys/class/gpio/gpio10/value", O_WRONLY);

		len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", B_GPIO);
		if(len<0){
			printf("snprintf error\n");
		}
		LedB = open("/sys/class/gpio/gpio7/value", O_WRONLY);

		while(1){
			//set when mouse event
			/*			
			if(terminate == 1){
				break;
			}
			*/
			//CASE1
			// loop forever toggling the LEDR every cycle
			for (j=0;j<25;j++) {
				write(LedR,"1",1);
				usleep(pwm_on);
				write(LedR, "0", 1);
				usleep(pwm_off);
			}

			/*			
			if(terminate == 1){
				break;
			}
			*/

			//CASE2	
			// loop forever toggling the LEDG every cycle
			for (j=0;j<25;j++) {
				write(LedG,"1",1);
				usleep(pwm_on);
				write(LedG, "0", 1);
				usleep(pwm_off);
			}

			/*			
			if(terminate == 1){
				break;
			}
			*/

			//CASE3	
			// loop forever toggling the LEDG every cycle
			for (j=0;j<25;j++) {
				write(LedB,"1",1);
				usleep(pwm_on);
				write(LedB, "0", 1);
				usleep(pwm_off);
			}

			/*			
			if(terminate == 1){
				break;
			}
			*/

			//CASE4		
			// loop forever toggling the LEDR And LEDG every cycle
			for (j=0;j<25;j++) {
				write(LedR,"1",1);
				write(LedG,"1",1);
				usleep(pwm_on);
				write(LedR, "0", 1);
				write(LedG, "0", 1);
				usleep(pwm_off);
			}

			/*			
			if(terminate == 1){
				break;
			}
			*/

			//CASE5
			// loop forever toggling the LEDR and LEDB every cycle
			for (j=0;j<25;j++) {
				write(LedR,"1",1);
				write(LedB,"1",1);
				usleep(pwm_on);
				write(LedR, "0", 1);
				write(LedB, "0", 1);
				usleep(pwm_off);
			}

			/*			
			if(terminate == 1){
				break;
			}
			*/

			//CASE6		
			// loop forever toggling the LEDG every cycle
			for (j=0;j<25;j++) {
				write(LedB,"1",1);
				write(LedG,"1",1);
				usleep(pwm_on);
				write(LedB, "0", 1);
				write(LedG, "0", 1);
				usleep(pwm_off);
			}

			/*			
			if(terminate == 1){
				break;
			}
			*/

			//CASE7			
			// loop forever toggling the LEDG every cycle
			for (j=0;j<25;j++) {
				write(LedR,"1",1);
				write(LedG,"1",1);
				write(LedB,"1",1);
				usleep(pwm_on);
				write(LedR, "0", 1);
				write(LedG, "0", 1);
				write(LedB, "0", 1);
				usleep(pwm_off);
			}

			/*			
			if(terminate == 1){
				break;
			}
			*/

		}

		IOClose();

	}
	else{
		printf("Enter correct input values...!!\n");
	}

	return 0;
}