#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/device.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include<linux/slab.h>
#include <linux/string.h>

#define DEVICE_NAME "RGBLed"
//#define DEVICE_NAME1 "RGBLed_dev1"			// device-1 name to be created and registered


MODULE_LICENSE("GPL");              
MODULE_AUTHOR("Achal Shah & Aditi Sonik");      
MODULE_DESCRIPTION("LKM of RGB Led");


static dev_t dev_num;					//For allocating mojor & minor number
struct class *RGBLed_class; 				//For allocating class number
static struct device *RGBLed_device;


struct RGBLed_dev {
	struct cdev cdev;				/* The cdev structure */
	char name[20];		                	/* Name of device*/
	
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


ssize_t RGBLed_write(struct file *file, const char *buff, size_t count, loff_t *ppos)
{

	printk(KERN_ALERT"Writing.......through %s function\n", __FUNCTION__);

	return count;									//-1 = failure ; 1 = success
}


struct file_operations RGBLed_fops={
	.owner			= THIS_MODULE,           
	.open			= RGBLed_open,         
	.release		= RGBLed_release,      
	.write	    	= RGBLed_write,
	//.unlocked_ioctl = RGBLed_ioctl,                 
};



int __init RGBLed_module_init(void)
{

	int return1;

	printk(KERN_ALERT"Working on %s function\n", __FUNCTION__);
	
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
	
	printk(KERN_ALERT"Working on %s function\n", __FUNCTION__);

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
















