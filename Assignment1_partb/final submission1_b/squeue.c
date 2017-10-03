#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/device.h>          
#include <asm/uaccess.h> 
#include <linux/cdev.h>
#include<linux/slab.h>
#include <linux/string.h>

#define DEVICE_NAME "squeue"
#define DEVICE_NAME1 "dataqueue1"			// device-1 name to be created and registered
#define DEVICE_NAME2 "dataqueue2"			// device-2 name to be created and registered

#define MAX_MSGS 10

MODULE_LICENSE("GPL");              
MODULE_AUTHOR("Achal Shah & Aditi Sonik");      
MODULE_DESCRIPTION("LKM of Shared Queue");


static dev_t dev_num;					//For allocating mojor & minor number
struct class *dataqueue_class; 				//For allocating class number
static struct device *dataqueue1_device, *dataqueue2_device;

//Structure definition
struct message{
	long double buff; 				//to store value of pi
	int m_id;					//to keep track of message_id
	unsigned long long enqueue_time, dequeue_time; 	//to keep track of enqueue dequeue time
	unsigned long long sender_id;			//to keep track of sender thread's id
	
};
	
struct mq{ 
				
	struct message *msg[MAX_MSGS];			//pointer array to keep track of messages
	int front;					//front points to front end of the queue
	int rear;					//rear points to last element of the queue
	int maxSize;					//Maximum capacity of queue
	int currentSize;				//keep track of current number of messages in queue
	
};

/* device structure */
struct squeue_dev {
	struct cdev cdev;				/* The cdev structure */
	char name[20];		                	/* Name of device*/
	struct mq q;					//Queue structure

} *dataqueue1, *dataqueue2;

//Open device driver
int squeue_open(struct inode *inode, struct file *file)
{
	struct squeue_dev *dataqueue;
	printk(KERN_ALERT"Opening.....through %s function\n", __FUNCTION__);

	dataqueue = container_of(inode->i_cdev, struct squeue_dev, cdev);			// Get the per-device structure that contains this cdev 

	file->private_data = dataqueue;								// Easy access to cmos_devp from rest of the entry points 

	printk(KERN_ALERT"\n%s is openning \n", dataqueue->name);
	return 0;
}

//Release devices of device driver
int squeue_release(struct inode *inode, struct file *file)
{
	struct squeue_dev *dataqueue = file->private_data;

	printk(KERN_ALERT"Closing.......through %s function\n", __FUNCTION__);

	printk(KERN_ALERT"\n%s is closing\n", dataqueue->name);
	
	return 0;
}

//writting to device of device driver
ssize_t squeue_write(struct file *file, const char *buff, size_t size_message, loff_t *ppos)
{
	int ret = 0;
	struct squeue_dev *dataqueue = file->private_data;

	dataqueue->q.msg[dataqueue->q.rear] = (struct message *)kmalloc(sizeof(struct squeue_dev), GFP_KERNEL);	
	
	printk(KERN_ALERT"Writing.......through %s function\n", __FUNCTION__);

	ret = copy_from_user(dataqueue->q.msg[dataqueue->q.rear],buff,sizeof(struct message));	//Obtain message data from user space
	if(ret > 0){
		printk("failure copy_from_user \n");
	}	

	//Check dataqueue full or not
	if(((dataqueue->q.rear == dataqueue->q.maxSize -1) && (dataqueue->q.front == 0)) || (dataqueue->q.rear == dataqueue->q.front -1)){
		printk(KERN_ALERT"dataqueue1 is full");
		return -1;
	}

	/*if( (dataqueue->q.front == -1) && (dataqueue->q.rear == -1)){
		dataqueue->q.front = 0;
	} */
	
	dataqueue->q.rear = (dataqueue->q.rear + 1) % dataqueue->q.maxSize ;
	dataqueue->q.currentSize++;	
	printk(KERN_ALERT"Written to dataqueue = %s \n", dataqueue->name);

	return size_message;									//-1 = failure ; 1 = success
}

//Reading from device of device driver
ssize_t squeue_read(struct file *file, char *buff , size_t size_message, loff_t *ppos)
{
	int ret = 0;
	struct squeue_dev *dataqueue;

	printk(KERN_ALERT"Reading......through %s function\n", __FUNCTION__);
	
	dataqueue = file->private_data;			

	//funtion to check queue empty or not
	if(dataqueue->q.currentSize == 0/*dataqueue->q.front == -1*/){								// 0 = FALSE ; 1 = TRUE
		return -1;
	}

	ret = copy_to_user(buff,dataqueue->q.msg[dataqueue->q.front], sizeof(struct message));	//pass the message to user space
	if(ret > 0){
		printk("failure copy_to_user \n");
	}
	//resetting circular queue pointers
	if(dataqueue->q.front == dataqueue->q.rear){
		dataqueue->q.front = -1;
		dataqueue->q.rear = -1;
		
	}
	else if(dataqueue->q.front == (dataqueue->q.maxSize)-1){
		dataqueue->q.front = 0;
	}
	else{
		dataqueue->q.front++;
		
	}		
	dataqueue->q.currentSize--;								//decrement current queue size

	printk(KERN_ALERT"read from dataqueue = %s \n", dataqueue->name);
	
	return size_message;

}

//define file operations 
struct file_operations squeue_fops={
	.owner		= THIS_MODULE,           
	.open		= squeue_open,         
	.release	= squeue_release,      
	.write		= squeue_write,        
	.read		= squeue_read,         
};

//device driver initialization module
int __init squeue_module_init(void)
{
	int return1,return2;
	int time_since_boot;

	printk(KERN_ALERT"Working on %s function\n", __FUNCTION__);
	
	if (alloc_chrdev_region(&dev_num, 0, 2, DEVICE_NAME)<0){			//Allocating major|minor numbers to char-devices of device driver "squeue"
		printk(KERN_ALERT "Device not registered\n");
		return -1;
	}
			
		dataqueue_class = class_create(THIS_MODULE, "squeueA");			//Populate sysfs entries; Adding class to our device 
		
		dataqueue1 = kmalloc(sizeof(struct squeue_dev), GFP_KERNEL);		//Allocating memory to dataQ1
		if (!dataqueue1) {
			printk(KERN_ALERT"Bad Kmalloc\n"); return -ENOMEM;
		}
		
		printk(KERN_ALERT"starting to initialize attributes of dataqueue1\n");
		
		//Initializing attributes of dataqueue1
		dataqueue1->q.front = -1;
		dataqueue1->q.rear = -1;
		dataqueue1->q.maxSize = MAX_MSGS;					//keep track of queue size
		dataqueue1->q.currentSize = 0;						//keep track of messages currently queue holding
	
		
		dataqueue2 = kmalloc(sizeof(struct squeue_dev), GFP_KERNEL);		//Allocating memory to dataQ2
		if (!dataqueue2) {
			printk(KERN_ALERT"Bad Kmalloc\n"); return -ENOMEM;
		}
		
		printk(KERN_ALERT"starting to initialize attributes of dataqueue2\n");
		//Initializing attributes of dataqueue2
		
		dataqueue2->q.front = 0;
		dataqueue2->q.rear = 0;
		dataqueue2->q.maxSize = MAX_MSGS;					//keep track of queue size
		dataqueue2->q.currentSize = 0;						//keep track of messages currently queue holding
	

		cdev_init(&dataqueue1->cdev, &squeue_fops);				//Perform device initialization by connecting the file operations with the cdev 
		dataqueue1->cdev.owner = THIS_MODULE;

		cdev_init(&dataqueue2->cdev, &squeue_fops);
		dataqueue2->cdev.owner = THIS_MODULE;
		
		return1 = cdev_add(&dataqueue1->cdev, (dev_num), 1);			// Add the major/minor number of device 1 to the cdev's List 
		if (return1 < 0) {
			printk(KERN_ALERT"Bad cdev1\n");
			return return1;
		}

		return2 = cdev_add(&dataqueue2->cdev, (dev_num), 2);			// Add the major/minor number of device 2 to the cdev's List
		if (return2 < 0) {
			printk(KERN_ALERT"Bad cdev2\n");
			return return2;
		}

		dataqueue1_device = device_create(dataqueue_class, NULL, MKDEV(MAJOR(dev_num), 0), NULL, DEVICE_NAME1);
		dataqueue2_device = device_create(dataqueue_class, NULL, MKDEV(MAJOR(dev_num), 1), NULL, DEVICE_NAME2);

		strcpy(dataqueue1->name,DEVICE_NAME1);
		strcpy(dataqueue2->name,DEVICE_NAME2);
				
		printk(KERN_ALERT"Completed %s\n", __FUNCTION__);
	
		time_since_boot=(jiffies-INITIAL_JIFFIES)/HZ;				//since on some systems jiffies is a very huge uninitialized value at boot and saved.
		
	return 0;
}

void __exit squeue_module_exit(void)
{	
	
	printk(KERN_ALERT"Working on %s function\n", __FUNCTION__);

	/* Release the major number */
	unregister_chrdev_region((dev_num), 2);

	device_destroy(dataqueue_class, MKDEV(MAJOR(dev_num), 0));
	device_destroy(dataqueue_class, MKDEV(MAJOR(dev_num), 1));

	cdev_del(&dataqueue1->cdev);
	
	//free memory for dataqueue	
	kfree(dataqueue1);
	
	cdev_del(&dataqueue2->cdev);

	kfree(dataqueue2);
	
	/* Destroy driver_class */
	class_destroy(dataqueue_class);

	printk(KERN_ALERT"squeue driver removed.\n");
}


module_init(squeue_module_init);
module_exit(squeue_module_exit);














