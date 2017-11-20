#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/param.h>
#include <linux/list.h>
#include <linux/semaphore.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/delay.h>	
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/errno.h>

#define DEVICE_NAME "Sensor"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Achal Shah & Aditi Sonik");
MODULE_DESCRIPTION("LKM of Sensor");

//Globals
static dev_t dev_num;							//For allocating device number
struct class *sensor_class; 					//For allocating class number

struct sensor_dev {
	struct cdev cdev;							// The cdev structure 
	char name[20];		                		// Name of device
} *sensor_dev1;

int TRIG_GPIO = 13, TRIG_LS = 34, TRIG_MUX = 77;
int ECHO_GPIO = 14, ECHO_LS = 16, ECHO_MUX = 76;

unsigned int irq1;
int BUSY = 0;
int EDGE = 0;  	// EDGE = 0 ---> Rising edge
				// EDGE = 1 ---> Falling edge
// double CurrDistance;
unsigned long long rise, fall, CurrDistance;


static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

static irqreturn_t sensor_interrupt_handler(unsigned int irq, void *dev_id){		
	if(EDGE==0){

		rise = rdtsc();
		// printk(KERN_ALERT"Rising\n");
		irq_set_irq_type(irq1,IRQF_TRIGGER_FALLING);
		BUSY = 1;
		EDGE = 1;
	}
	else{

		fall = rdtsc();
		// printk(KERN_ALERT"Falling\n");
		irq_set_irq_type(irq1,IRQF_TRIGGER_RISING);
		EDGE = 0;
		BUSY = 0;
	}
	
	return IRQ_HANDLED;
}
	

int sensor_open(struct inode *inode, struct file *file)
{
	
	printk(KERN_ALERT"Opening.....through %s function\n", __FUNCTION__);

	sensor_dev1 = container_of(inode->i_cdev, struct sensor_dev, cdev);			// Get the per-device structure that contains this cdev 

	file->private_data = sensor_dev1;								// Easy access to cmos_devp from rest of the entry points 

	printk(KERN_ALERT"\n%s is opened \n", sensor_dev1->name);

	return 0;
}


//Release devices of device driver
int sensor_release(struct inode *inode, struct file *file)
{
	struct sensor_dev *sensor_dev1 = file->private_data;

	printk(KERN_ALERT"Closing.......through %s function\n", __FUNCTION__);

	printk(KERN_ALERT"\n%s is closing\n", sensor_dev1->name);
	
	return 0;
}


ssize_t sensor_write(struct file *file, const char *user_buff, size_t count, loff_t *ppos)
{

	printk(KERN_ALERT"Writing.......through %s function\n", __FUNCTION__	);

	if(BUSY==1){
		return -EBUSY;
	}
		
	gpio_set_value_cansleep(TRIG_GPIO,1);
	udelay(15);
	gpio_set_value_cansleep(TRIG_GPIO,0);
		
	
	return 0;								
}


ssize_t sensor_read(struct file *file, char *buff , size_t count, loff_t *ppos){

	// struct message *object;
	unsigned int value;
	printk(KERN_ALERT"Reading......through %s function\n", __FUNCTION__);


	if (BUSY==1)
	{	
		fall = 0;
		rise = 0;
		return -EBUSY;

	}
	else{

		CurrDistance = fall-rise;
		value = div_u64(CurrDistance,400);
		printk(KERN_ALERT"The distance measured is %d \n",value);
		copy_to_user((void *)buff,(const void *)&value, sizeof(value));
	}

	// put_user(CurrDistance, buff);

	return 0;
}


static struct file_operations Sensor_fops={
	.owner			= THIS_MODULE,           
	.open			= sensor_open,
	.read			= sensor_read,         
	.release		= sensor_release,      
	.write	    	= sensor_write,
	               
};


int __init sensor_module_init(void)
{

	int return1, status;



	printk(KERN_ALERT"Installing Sensor device driver by %s function\n", __FUNCTION__);
	
	if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME)<0){			//Allocating major|minor numbers to char-device of device driver "Sensor"
		printk(KERN_ALERT "Device not registered\n");
		return -1;
	}

		sensor_class = class_create(THIS_MODULE, "Led");			//Populate sysfs entries; Adding class to our device 
		
		sensor_dev1 = kmalloc(sizeof(struct sensor_dev), GFP_KERNEL);		//Allocating memory to sensor_dev1
		if (!sensor_dev1) {
			printk(KERN_ALERT"Bad Kmalloc\n"); return -ENOMEM;
		}


		cdev_init(&sensor_dev1->cdev, &Sensor_fops);				//Perform device initialization by connecting the file operations with the cdev 
		sensor_dev1->cdev.owner = THIS_MODULE;

		return1 = cdev_add(&sensor_dev1->cdev, (dev_num), 1);			// Add the major/minor number of device 1 to the cdev's List 
		if (return1 < 0) {
			printk(KERN_ALERT"Bad cdev_add\n");
			return return1;
		}

		device_create(sensor_class, NULL, MKDEV(MAJOR(dev_num), 0), NULL, DEVICE_NAME);

		strcpy(sensor_dev1->name,DEVICE_NAME);

		printk(KERN_ALERT"Sensor driver installed by %s\n", __FUNCTION__);

	//-----------------------------GPIO SETUP------------------------------//
	//----------GPIO PINS------------//

	gpio_request(TRIG_GPIO, "label");
	gpio_request(ECHO_GPIO, "label");

	gpio_direction_output(TRIG_GPIO,0);  
	gpio_direction_input(ECHO_GPIO);


	// //-----------LS PINS---------------//

	gpio_request(TRIG_LS, "label");
	gpio_request(ECHO_LS, "label");

	gpio_direction_output(TRIG_LS,0);
	gpio_direction_output(ECHO_LS,1);  


	// //---------MUX PINS--------------//

	gpio_request(TRIG_MUX, "label");
	gpio_request(ECHO_MUX, "label");


	//------------IRQ SETUP-------------//	


	irq1 = gpio_to_irq(ECHO_GPIO);
	if(irq1<0){
		printk(KERN_ALERT"IRQ number cannot be obtained\n");
	}

	printk(KERN_ALERT"IRQ number obtained %u\n",irq1);

	BUSY = 0;

	fall = 0;
	rise = 0;

	status = request_irq(irq1, (irq_handler_t)sensor_interrupt_handler, IRQF_TRIGGER_RISING,"sensor_interrupt_handler", sensor_dev1);
	if(status != 0){
		printk(KERN_ALERT"Request IRQ fail");
	}

	EDGE = 0;


		return 0;
}


void __exit sensor_module_exit(void)
{	
	// int ret;

	printk(KERN_ALERT"Uninstalling Sensor device driver by %s function\n", __FUNCTION__);

	/* Release the major number */
	unregister_chrdev_region((dev_num), 1);

	device_destroy(sensor_class, MKDEV(MAJOR(dev_num), 0));

	cdev_del(&sensor_dev1->cdev);
	
	//free memory 	
	kfree(sensor_dev1);

	class_destroy(sensor_class);

	free_irq(irq1, (void *)sensor_dev1);

	gpio_free(TRIG_GPIO);
	gpio_free(ECHO_GPIO);
	gpio_free(TRIG_LS);
	gpio_free(ECHO_LS);

	gpio_free(TRIG_MUX);
	gpio_free(ECHO_MUX);

	 
	printk(KERN_ALERT"Sensor driver removed.\n");
}

module_init(sensor_module_init);
module_exit(sensor_module_exit);