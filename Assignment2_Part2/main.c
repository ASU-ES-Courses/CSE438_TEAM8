#include<stdio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include<errno.h>
#include <sys/ioctl.h>
#include<string.h>
#include<malloc.h>

#define CONFIG 1
#define UNCONFIG 0

struct values{
	int arr[4];
	//int cmd;
};
int main(){

	int fd = 0,return_value,i;
	struct values *object;
	int y = 3;
	int arr_pattern[7] = {1,2,4,3,5,6,7}; //{'R','G','B','RG','RB','GB','RGB'}
	/*

	  color	  LS_bit  case
	   _#_	   _BGR_   _#_
		R 		001		1
		G 		010		2
		B 		100		4
		RG 		011		3
		RB 		101		5
		GB 		110		6
		RGB 	111		7

	*/
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
		//object->cmd = CONFIG;
		return_value = ioctl(fd,CONFIG,object);
		printf("return = %d\n",return_value);
		//invalid inputs
		if(return_value < 0){
			printf("Unable to write to RGBLed Device or Invalid input\n");
			errno = 22;//ENIVAL = 22
		}
		//valid inputs
		else{		
			while(y>0){
				//When PWM is not equal to 0
				if(object->arr[0] != 0){
					for(i=0;i<7;i++){
						write(fd,&arr_pattern[i],sizeof(arr_pattern[i]));
					}
				}
				// When PWM is 0
				else 
					write(fd,"0",sizeof(int));
				y--;			
			}
		}
		
	}

	close(fd);
	free(object);
	return 0;
}