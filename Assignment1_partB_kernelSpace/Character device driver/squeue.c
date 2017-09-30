#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
//#include<cdev.h>
//#include<asm/uaccess.h>


MODULE_LICENSE("GPL");

//static dev_t dev_num;

int squeue_open(struct inode *qinode, struct file *qfile){
	printk(KERN_ALERT"Opening.....through %s function\n", __FUNCTION__);
	return 0;
}

ssize_t squeue_read(struct file *qfile, char __user *buff, size_t count, loff_t *ppos){
	printk(KERN_ALERT"Reading......through %s function\n", __FUNCTION__);
	return 0;
}

ssize_t squeue_write(struct file *qfile, const char __user *buff, size_t count, loff_t *ppos){
	printk(KERN_ALERT"Writing.......through %s function\n", __FUNCTION__);
	return count;
}

int squeue_release(struct inode *qinode, struct file *qfile){
	printk(KERN_ALERT"Closing.......through %s function\n", __FUNCTION__);
	return 0;
}

struct file_operations squeue_fops={
	.owner		= THIS_MODULE,           
	.open		= squeue_open,         
	.release	= squeue_release,      
	.write		= squeue_write,        
	.read		= squeue_read,         
};


int __init squeue_module_init(void)
{
	printk(KERN_ALERT"Working on %s function\n", __FUNCTION__);

	register_chrdev(121,"dataqueue1",&squeue_fops);

//	if (alloc_chrdev_region(&dev_num, 0, 1, dataqueue1)<0){
//		printk(KERN_ALERT "Device not registered\n");
//		return -1;
//	}

	return 0;
}

void __exit squeue_module_exit(void)
{	
	printk(KERN_ALERT"Working on %s function\n", __FUNCTION__);
	unregister_chrdev(121, "dataqueue1");
}

module_init(squeue_module_init);
module_exit(squeue_module_exit);














