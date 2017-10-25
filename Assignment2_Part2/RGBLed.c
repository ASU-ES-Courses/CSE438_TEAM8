#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<asm/uaccess.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/string.h>
#include <linux/gpio.h>
#include<linux/delay.h>

#include<linux/jiffies.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define DEVICE_NAME "RGBLed"
#define CONFIG 1

MODULE_LICENSE("GPL");              
MODULE_AUTHOR("Achal Shah & Aditi Sonik");      
MODULE_DESCRIPTION("LKM of RGB Led");

//Globals

static dev_t dev_num;																//For allocating device number
struct class *RGBLed_class; 														//For allocating class number
static struct device *RGBLed_device;												//creating object of DEVICE structure

unsigned long on_timer_interval_s = 1; //PWM 50%
//unsigned long off_timer_interval_s = 0.01;
static struct hrtimer hr_timer;
ktime_t ktime_on;//, ktime_off;

int R_GPIO, R_LS, R_MUX;
int G_GPIO, G_LS, G_MUX;
int B_GPIO, B_LS, B_MUX;

int GPIO_PIN[] = {11,12,13,14,6,0,1,38,40,4,10,5,15,7};
int LS_PIN[] = {32,28,34,16,36,18,20,-1,-1,22,26,24,42,30};
int MUX_PIN[] = {-1,45,77,76,-1,66,68,-1,-1,70,74,44,-1,46};

int LedOFF = 0; 																	// To see LED ON or OFF; 0 = OFF
int flag=0;
int pin_flag[3] = {0,0,0}; 															//To see which pins among RGB are enabled

struct values{
	int arr[4];
};
struct RGBLed_dev {
	struct cdev cdev;															// The cdev structure 
	char name[20];		                										// Name of device
	int pattern;																// Type of LED pattern sent to device 
} *RGBLed_dev1;

//logic taken from https://gist.github.com/maggocnx/5946907#file-timertest-c-L15
enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart){              
  	ktime_t currtime , interval;
  	//printk(KERN_ALERT"In callback function");
  	currtime  = ktime_get();
	interval = ktime_set(on_timer_interval_s,0);//timer_interval_ns); 			//on timer
	hrtimer_forward(timer_for_restart, currtime , interval);

  	//if(RGBLed_dev1->pattern != 7){
	  	if(flag == 0){
	  		/*currtime  = ktime_get();
		  	interval = ktime_set(on_timer_interval_s,0);//timer_interval_ns); 			//on timer
		  	hrtimer_forward(timer_for_restart, currtime , interval);*/
		  		
		  	if(pin_flag[0] != 0){
				gpio_set_value(R_GPIO, 1);
			}
			if(pin_flag[1] != 0){
				gpio_set_value(G_GPIO, 1);
			}
			if(pin_flag[2] != 0){
				gpio_set_value(B_GPIO, 1);
			}
		  	flag = 1;
		  	return HRTIMER_RESTART;  
		  	//printk(KERN_ALERT"Exiting if block");
	  	}
	  	else{
	  		/*currtime  = ktime_get();
		  	interval = ktime_set(on_timer_interval_s,0);//timer_interval_ns); 			//off timer
		  	hrtimer_forward(timer_for_restart, currtime , interval);*/
		  	gpio_set_value(R_GPIO, 0);
			gpio_set_value(G_GPIO, 0);
			gpio_set_value(B_GPIO, 0);
		  	flag = 0;
		  	return HRTIMER_NORESTART;
		  	//printk(KERN_ALERT"Exiting else block");
	  	}
	  	
	 	/*return HRTIMER_RESTART;  */
	 /*}
	 else{
	 	return HRTIMER_NORESTART;

	 } */
}

int RGBLed_open(struct inode *inode, struct file *file)
{
	//struct RGBLed_dev *RGBLed;
	printk(KERN_ALERT"Opening.....through %s function\n", __FUNCTION__);

	RGBLed_dev1 = container_of(inode->i_cdev, struct RGBLed_dev, cdev);			// Get the per-device structure that contains this cdev 

	file->private_data = RGBLed_dev1;											

	printk(KERN_ALERT"\n%s is openning \n", RGBLed_dev1->name);
	return 0;
}

//Release devices of device driver
int RGBLed_release(struct inode *inode, struct file *file)
{
	struct RGBLed_dev *RGBLed_dev1 = file->private_data;

	printk(KERN_ALERT"Closing.......through %s function\n", __FUNCTION__);

	printk(KERN_ALERT"\n%s is closing\n", RGBLed_dev1->name);
	
	return 0;
}

ssize_t RGBLed_write(struct file *file, const char *user_buff, size_t count, loff_t *ppos)
{
	int status = 0;//,j=0,k=25;
	//printk(KERN_ALERT"Writing.......through %s function\n", __FUNCTION__);	
	
	get_user(RGBLed_dev1->pattern, user_buff);

	//bitwise operation
	pin_flag[0] = RGBLed_dev1->pattern&4; //R
	pin_flag[1] = RGBLed_dev1->pattern&2; //G
	pin_flag[2] = RGBLed_dev1->pattern&1; //B

	//while(j<k){
		hrtimer_start(&hr_timer, ktime_on, HRTIMER_MODE_REL);
	//	hrtimer_start(&hr_timer, ktime_off, HRTIMER_MODE_REL);
		
	//	j++;
	//}

	
	//msleep(50);
	//if(RGBLed_dev1->pattern == 7){
		
	//}
	//printk(KERN_ALERT"HRtimer started\n");
	/*while(j<k){	
		//gpio_set_value(R_GPIO, 1);				
							
		//gpio_set_value(R_GPIO, 0);	
							
		j++;
	}*/
			
	return status;								
}

static long RGBLed_ioctl(struct file * file, unsigned int  x, unsigned long args){
	long status = 0,i;
	struct values *object;
	int array[4];

	printk(KERN_ALERT"Running.. %s function\n", __FUNCTION__);

	object = (struct values *)kmalloc(sizeof(struct values), GFP_KERNEL);
	//default values for PWM and LED pins
	object->arr[0] = -9;
	object->arr[1] = -9;
	object->arr[2] = -9;
	object->arr[3] = -9;
	status = copy_from_user(object,(struct values*)args,sizeof(struct values));
	if(status > 0){
		printk("failure copy_from_user \n");
	}	

	//Check valid inputs or not
	for(i=0;i<4;i++){
		//for checking floating point values
		/*if(object->arr[i] != (int)object->arr[i]){
			break;			
		}*/
		array[i] = (int)object->arr[i];
	}
	
	if( (array[0] < 101 && array[0] >= 0) && array[1] != -9 && array[2] != -9 && array[3] != -9 && 
		(array[1] < 14 && array[1] >= 0 ) && (array[2] < 14 && array[2] >= 0 ) && (array[3] < 14 && array[3] >= 0 ) && 
		(array[1] != array[2]) && (array[2] != array[3]) && (array[1] != array[3] ) ){

		switch(x){
			case CONFIG:
				printk("Configuring device..\n");			

				//Selecting GPIO Pins
				R_GPIO = GPIO_PIN[array[1]];
				G_GPIO = GPIO_PIN[array[2]];
				B_GPIO = GPIO_PIN[array[3]];

				//Selecting Level Shifter Pins
				R_LS = LS_PIN[array[1]];
				G_LS = LS_PIN[array[2]];
				B_LS = LS_PIN[array[3]];

				//Selecting MUX Pins
				R_MUX = MUX_PIN[array[1]];
				G_MUX = MUX_PIN[array[2]];
				B_MUX = MUX_PIN[array[3]];

				//GPIO PINS------------------//
				status =  gpio_direction_output(R_GPIO, LedOFF);   		// Set the gpio to be in output mode and turn off											
				status =  gpio_direction_output(G_GPIO, LedOFF);
				status =  gpio_direction_output(B_GPIO, LedOFF);
				
				//LS PINS--------------------//
				if(R_LS != -1){
					status =  gpio_direction_output(R_LS, LedOFF);
					gpio_set_value(R_LS, 0);
				}

				if(G_LS != -1){
					status =  gpio_direction_output(G_LS, LedOFF);
					gpio_set_value(G_LS, 0);
				}

				if(B_LS != -1){
					status =  gpio_direction_output(B_LS, LedOFF);
					gpio_set_value(B_LS, 0);
				}

				//MUX PINS-----------------------//
				if(R_MUX != -1){
					if(R_MUX < 64 || R_MUX > 79){
						status =  gpio_direction_output(R_MUX, LedOFF);
					}
					gpio_set_value(R_MUX, 0);
				}

				if(G_MUX != -1){
					if(G_MUX < 64 || G_MUX > 79){
						status =  gpio_direction_output(G_MUX, LedOFF);
					}
					gpio_set_value(G_MUX, 0);
				}

				if(B_MUX != -1){
					if(B_MUX < 64 || B_MUX > 79){
						status =  gpio_direction_output(B_MUX, LedOFF);
					}
					gpio_set_value(B_MUX, 0);
				}
				//-----------------------------//
				break;
				
			default:
				printk("No case specified for device\n");
				break;				
		}//End of switch
	}//End of if
	else{
		status = -1;
	}
	kfree(object);
	return status;

}


struct file_operations RGBLed_fops={
	.owner			= THIS_MODULE,           
	.open			= RGBLed_open,         
	.release		= RGBLed_release,      
	.write	    	= RGBLed_write,
	.unlocked_ioctl = RGBLed_ioctl,  
	               
};



int __init RGBLed_module_init(void)
{

	int return1;
	/*ktime_t ktime;*/

	printk(KERN_ALERT"Installing RGBLed device driver by %s function\n", __FUNCTION__);
	
	if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME)<0){				//Allocating major|minor numbers to char-device of device driver "RGBLed"
		printk(KERN_ALERT "Device not registered\n");
		return -1;
	}

		RGBLed_class = class_create(THIS_MODULE, "Led");					//Populate sysfs entries; Adding class to our device 
		
		RGBLed_dev1 = kmalloc(sizeof(struct RGBLed_dev), GFP_KERNEL);		//Allocating memory to RGBLed_dev1
		if (!RGBLed_dev1) {
			printk(KERN_ALERT"Bad Kmalloc\n"); return -ENOMEM;
		}


		cdev_init(&RGBLed_dev1->cdev, &RGBLed_fops);						//Perform device initialization by connecting the file operations with the cdev 
		RGBLed_dev1->cdev.owner = THIS_MODULE;

		return1 = cdev_add(&RGBLed_dev1->cdev, (dev_num), 1);				// Add the major/minor number of device 1 to the cdev's List 
		if (return1 < 0) {
			printk(KERN_ALERT"Bad cdev_add\n");
			return return1;
		}

		RGBLed_device = device_create(RGBLed_class, NULL, MKDEV(MAJOR(dev_num), 0), NULL, DEVICE_NAME);

		strcpy(RGBLed_dev1->name,DEVICE_NAME);

		printk("initializing the timer\n");
		
		ktime_on = ktime_set(on_timer_interval_s,0);// timer_interval_ns); //(sec,Nsec)
		//ktime_off = ktime_set(off_timer_interval_s,0);// timer_interval_ns); //(sec,Nsec)
		hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
		hr_timer.function = &timer_callback;

		printk("END: Timer initialization\n");


		printk(KERN_ALERT"RGBLed driver installed by %s\n", __FUNCTION__);

		return 0;
}


void __exit RGBLed_module_exit(void)
{	
	int ret;

	printk(KERN_ALERT"Uninstalling RGBLed device driver by %s function\n", __FUNCTION__);

	/* Release the major number */
	unregister_chrdev_region((dev_num), 1);

	device_destroy(RGBLed_class, MKDEV(MAJOR(dev_num), 0));

	cdev_del(&RGBLed_dev1->cdev);
	
	//free memory 	
	kfree(RGBLed_dev1);

	class_destroy(RGBLed_class);

	printk("HR Timer module uninstalling\n");
	ret = hrtimer_cancel( &hr_timer );
  	if (ret) printk("The timer was still in use...\n");

	printk(KERN_ALERT"RGBLed driver removed.\n");
}


module_init(RGBLed_module_init);
module_exit(RGBLed_module_exit);















