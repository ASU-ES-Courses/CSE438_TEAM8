#include<stdio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include<errno.h>
#include <sys/ioctl.h>
#include<string.h>


int main(){

	int fd = 0,return_value,i;
	int arr[4] = {-9,-9,-9,-9};						//[PWM,R,G,B] with default "-9" value
	//unsigned long CONFIG = 0;
	int y = 0;
	int arr_pattern[7] = {1,2,3,4,5,6,7}; //{'R','G','B','RG','RB','GB','RGB'}

	printf("Enter Integer input values in order [ percentage of PWM, R_IO, G_IO, B_IO ]\n");
	
	for(i=0;i<4;i++){
		scanf("%d",&arr[i]);		
	}

	
	fd = open("/dev/RGBLed", O_RDWR);						
	if (fd < 0 ){
		printf("Can not open RGBLed device file.\n");		
		//return -1;
	}
	else{
		//return_value = ioctl(fd, arr[1], arr[2], arr[3]);
		return_value = ioctl(fd,CONFIG,y);
		if(return_value < 0){
			printf("Unable to write to RGBLed Device or Invalid input\n");
			errno = 22;//ENIVAL = 22
		}
		printf("return from ioctl = %d\n",return_value);
		
		while(1){
			//When PWM is not equal to 0
			if(arr[0] != 0){
				for(i=0;i<7;i++){
					write(fd,&arr_pattern[i],sizeof(arr_pattern[i]));
				}
			}
			// When PWM is 0
			else 
				write(fd,"0",sizeof(int));
		}
		
	}



	close(fd);

	return 0;
}