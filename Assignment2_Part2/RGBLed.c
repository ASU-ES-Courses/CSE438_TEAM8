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
#include<linux/ioctl.h>

#define DEVICE_NAME "RGBLed"
#define CONFIG __IO
#define UNCONFIG __IO

MODULE_LICENSE("GPL");              
MODULE_AUTHOR("Achal Shah & Aditi Sonik");      
MODULE_DESCRIPTION("LKM of RGB Led");

//Globals

static dev_t dev_num;							//For allocating device number
struct class *RGBLed_class; 					//For allocating class number
static struct device *RGBLed_device;			//creating object of DEVICE structure

int R_GPIO, R_LS, R_MUX;
int G_GPIO, G_LS, G_MUX;
int B_GPIO, B_LS, B_MUX;

int GPIO_PIN[] = {11,12,13,14,6,0,1,38,40,4,10,5,15,7};
int LS_PIN[] = {32,28,34,16,36,18,20,-1,-1,22,26,24,42,30};
int MUX_PIN[] = {-1,45,77,76,-1,66,68,-1,-1,70,74,44,-1,46};

int LedOFF = 0; // To see LED ON or OFF; 0 = OFF

struct RGBLed_dev {
	struct cdev cdev;							// The cdev structure 
	char name[20];		                		// Name of device
	char pattern;
	
} *RGBLed_dev1;


int RGBLed_open(struct inode *inode, struct file *file)
{
	//struct RGBLed_dev *RGBLed;
	printk(KERN_ALERT"Opening.....through %s function\n", __FUNCTION__);

	RGBLed_dev1 = container_of(inode->i_cdev, struct RGBLed_dev, cdev);			// Get the per-device structure that contains this cdev 

	file->private_data = RGBLed_dev1;								// Easy access to cmos_devp from rest of the entry points 

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
	int status = 0;

	printk(KERN_ALERT"Writing.......through %s function\n", __FUNCTION__);
	
	get_user(RGBLed_dev1->pattern, user_buff);
	switch(RGBLed_dev1->pattern){
		// case number represented in sequence {'R','G','B','RG','RB','GB','RGB'}
		case 1:
			gpio_set_value(R_GPIO, 1);
			msleep(10);
			break;
		case 2:
			gpio_set_value(R_GPIO, 1);
			msleep(10);
			break;
		case 3:
			gpio_set_value(B_GPIO, 1);
			msleep(10);
			break;
		case 4:
			gpio_set_value(R_GPIO, 1);
			gpio_set_value(G_GPIO, 1);
			msleep(10);
			break;
		case 5:
			gpio_set_value(R_GPIO, 1);
			gpio_set_value(B_GPIO, 1);
			msleep(10);
			break;
		case 6:
			gpio_set_value(G_GPIO, 1);
			gpio_set_value(B_GPIO, 1);
			msleep(10);
			break;
		case 7:
			gpio_set_value(R_GPIO, 1);
			gpio_set_value(G_GPIO, 1);
			gpio_set_value(B_GPIO, 1);
			msleep(10);
			break;
		default:
			gpio_set_value(R_GPIO, 0);
			gpio_set_value(G_GPIO, 0);
			gpio_set_value(B_GPIO, 0);
			break;

	}
	gpio_set_value(R_GPIO, 0);
	gpio_set_value(G_GPIO, 0);
	gpio_set_value(B_GPIO, 0);	

	return status;								
}

//int RGBLed_ioctl(struct inode * inode, struct file * file, unsigned long _r, unsigned int _g, unsigned int _b){
int RGBLed_ioctl(struct inode * inode, struct file * file, unsigned long x, unsigned int y){
	int status = 0,_r,_g,_b;
	
	printk(KERN_ALERT"Configuring.......through %s function\n", __FUNCTION__);
	switch(x){

		case CONFIG:
			_r = 9;
			_g = 10;
			_b = 13;
			//Check valid inputs or not

			//Selecting GPIO Pins
			R_GPIO = GPIO_PIN[_r];
			G_GPIO = GPIO_PIN[_g];
			B_GPIO = GPIO_PIN[_b];

			//Selecting Level Shifter Pins
			R_LS = LS_PIN[_r];
			G_LS = LS_PIN[_g];
			B_LS = LS_PIN[_b];

			//Selecting MUX Pins
			R_MUX = MUX_PIN[_r];
			G_MUX = MUX_PIN[_g];
			B_MUX = MUX_PIN[_b];

			//GPIO PINS------------------//
			status =  gpio_direction_output(R_GPIO, LedOFF);   		// Set the gpio to be in output mode and turn off
			status =  gpio_export(R_GPIO, false);					//causes gpio pin to appear in /sys/class/gpio & the second argument prevents the direction from being changed
			
			status =  gpio_direction_output(G_GPIO, LedOFF);
			status =  gpio_export(G_GPIO, false);

			status =  gpio_direction_output(B_GPIO, LedOFF);
			status =  gpio_export(B_GPIO, false);
			
			//LS PINS--------------------//
			if(R_LS != -1){
				status =  gpio_direction_output(R_LS, LedOFF);
				status =  gpio_export(R_LS, false);
				gpio_set_value(R_LS, 0);
			}

			if(G_LS != -1){
				status =  gpio_direction_output(G_LS, LedOFF);
				status =  gpio_export(G_LS, false);
				gpio_set_value(G_LS, 0);
			}

			if(B_LS != -1){
				status =  gpio_direction_output(B_LS, LedOFF);
				status =  gpio_export(B_LS, false);
				gpio_set_value(B_LS, 0);
			}

			//MUX PINS-----------------------//
			if(R_MUX != -1){
				if(R_MUX < 64 || R_MUX > 79){
					status =  gpio_direction_output(R_MUX, LedOFF);
				}
				status =  gpio_export(R_MUX, false);
				gpio_set_value(R_MUX, 0);
			}

			if(G_MUX != -1){
				if(G_MUX < 64 || G_MUX > 79){
					status =  gpio_direction_output(G_MUX, LedOFF);
				}
				status =  gpio_export(G_MUX, false);
				gpio_set_value(G_MUX, 0);
			}

			if(B_MUX != -1){
				if(B_MUX < 64 || B_MUX > 79){
					status =  gpio_direction_output(B_MUX, LedOFF);
				}
				status =  gpio_export(B_MUX, false);
				gpio_set_value(B_MUX, 0);
			}
			//-----------------------------//
			break;
		case UNCONFIG:

			break;
		printk(KERN_ALERT"Configured RGBLed\n", __FUNCTION__);
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

	printk(KERN_ALERT"Installing RGBLed device driver by %s function\n", __FUNCTION__);
	
	if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME)<0){			//Allocating major|minor numbers to char-device of device driver "RGBLed"
		printk(KERN_ALERT "Device not registered\n");
		return -1;
	}

		RGBLed_class = class_create(THIS_MODULE, "Led");			//Populate sysfs entries; Adding class to our device 
		
		RGBLed_dev1 = kmalloc(sizeof(struct RGBLed_dev), GFP_KERNEL);		//Allocating memory to RGBLed_dev1
		if (!RGBLed_dev1) {
			printk(KERN_ALERT"Bad Kmalloc\n"); return -ENOMEM;
		}


		cdev_init(&RGBLed_dev1->cdev, &RGBLed_fops);				//Perform device initialization by connecting the file operations with the cdev 
		RGBLed_dev1->cdev.owner = THIS_MODULE;

		return1 = cdev_add(&RGBLed_dev1->cdev, (dev_num), 1);			// Add the major/minor number of device 1 to the cdev's List 
		if (return1 < 0) {
			printk(KERN_ALERT"Bad cdev_add\n");
			return return1;
		}

		RGBLed_device = device_create(RGBLed_class, NULL, MKDEV(MAJOR(dev_num), 0), NULL, DEVICE_NAME);

		strcpy(RGBLed_dev1->name,DEVICE_NAME);

		printk(KERN_ALERT"RGBLed driver installed by %s\n", __FUNCTION__);

		return 0;
}


void __exit RGBLed_module_exit(void)
{	
	
	printk(KERN_ALERT"Uninstalling RGBLed device driver by %s function\n", __FUNCTION__);

	/* Release the major number */
	unregister_chrdev_region((dev_num), 1);

	device_destroy(RGBLed_class, MKDEV(MAJOR(dev_num), 0));

	cdev_del(&RGBLed_dev1->cdev);
	
	//free memory 	
	kfree(RGBLed_dev1);

	class_destroy(RGBLed_class);

	printk(KERN_ALERT"RGBLed driver removed.\n");
}


module_init(RGBLed_module_init);
module_exit(RGBLed_module_exit);















