//already included
#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h> //for register chrdev
#include <linux/slab.h> //for kmalloc, kfree
#include <linux/uaccess.h> //for copy to and from user

//not already included
#include <linux/cdev.h> // cdev_init, cdev_add


//In attempt to not taint the kernel
MODULE_AUTHOR("Aloha Churchill");
MODULE_LICENSE("GPL");


#define BUFFER_SIZE 1024

//define device_buffer and other global data structure
char* device_buffer = NULL;

//device cdev structure
struct cdev pa2_char_driver_cdev;

//other global vars
int num_times_opened = 0;
int num_times_closed = 0;


ssize_t pa2_char_driver_read(struct file* pfile, char __user *buffer, size_t length, loff_t *offset){	
	printk(KERN_NOTICE "Inside read function\n");

	//overflow case
	if(*(offset) + length >= BUFFER_SIZE){
		length = (BUFFER_SIZE) - *(offset);
	}

	//device buffer is address of device file and *(offset) is number of bytes to add to that
	//count is number of bytes not read
	int count = copy_to_user(buffer, device_buffer + *(offset), length);

	if(count <  0){
		printk(KERN_NOTICE "Did not successfully read requested data\n");
		return -1;
	}

	if(count > 0){
		printk(KERN_ALERT "Did not read all bytes\n");
		return -1;
	}

	//update offset position
	*(offset) = *(offset) + length;

	printk(KERN_NOTICE "Read %d bytes\n", length);
	return 0;
}

ssize_t pa2_char_driver_write(struct file* pfile, const char __user *buffer, size_t length, loff_t*  offset){

	//check for overflow
	if(*(offset)+length >= BUFFER_SIZE){
		length = (BUFFER_SIZE) - *(offset);
	}

	int count = copy_from_user(device_buffer + *(offset), buffer, length);

	if(count  <  0){
		printk(KERN_NOTICE "Write not successful\n");
		return -1;
	}

	if(count  >  0){
		printk(KERN_NOTICE "Not all bytes written\n");
		return -1;
	}
	

	*(offset) = *(offset) + length;

	printk(KERN_NOTICE "Wrote %d bytes\n", count);
	return length;
}

int pa2_char_driver_open(struct inode *pinode, struct file *pfile){

	printk(KERN_NOTICE "Device Opened\n");
	printk(KERN_NOTICE "Number of times device file  has been opened previously: %d\n", num_times_opened);
	
	num_times_opened += 1;

	return 0;
}

int pa2_char_driver_close(struct inode *pinode, struct file *pfile){
	printk(KERN_NOTICE "Device Closed\n");
	printk(KERN_NOTICE "Number of times device file has been closed previously: %d\n", num_times_closed);
	
	num_times_closed += 1;

	return 0;

}

loff_t pa2_char_driver_seek(struct file *pfile, loff_t offset, int whence){
	printk(KERN_NOTICE "Inside of seek function\n");

	if(whence == 0){
		//overflow cases, reset seek position
		if(offset > BUFFER_SIZE || offset < 0){
			printk(KERN_ALERT "Offset resulted in bounds error\n");
			return -1;
		}
		pfile->f_pos = offset;
	}
	else if(whence  == 1){
		if(pfile->f_pos + offset < 0 || pfile->f_pos + offset > BUFFER_SIZE){
			printk(KERN_ALERT "Offset resulted in bounds error\n");
			return -1;
		}
		pfile->f_pos += offset;
	}
	else if(whence  == 2){
		if(BUFFER_SIZE - offset > BUFFER_SIZE || BUFFER_SIZE - offset < 0){
			printk(KERN_ALERT "Offset resulted in bounds error\n");
			return -1;
		}
		pfile->f_pos = BUFFER_SIZE - offset;
	}
	else{
		printk(KERN_ALERT "Not a valid whence value\n");
		return -1;
	}
	
	return 0;
}

//struct using tagged structure initialization
struct file_operations pa2_char_driver_file_operations = {
	.owner = THIS_MODULE,
	.llseek = pa2_char_driver_seek,
	.read = pa2_char_driver_read,
	.write = pa2_char_driver_write,
	.open = pa2_char_driver_open,
	.release = pa2_char_driver_close
};


static int pa2_char_driver_init(void){
	printk("Init function called\n");
	dev_t device_num = MKDEV(240, 0);

	//allocate memory for character device
	if(register_chrdev(240, "pa2_char_device", &pa2_char_driver_file_operations) != 0){
		//then error occured with allocation
		printk(KERN_ALERT "Error  in registering char device\n");
		return -1;
	}

	//initialize cdev structure
	cdev_init(&pa2_char_driver_cdev, &pa2_char_driver_file_operations);
 
	if(cdev_add(&pa2_char_driver_cdev, device_num, 1) < 0){
		printk(KERN_ALERT "Device not added to system, cdev_add failed\n");
		return -1;
	}

	//allocate space for data buffer
	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);

	if(device_buffer ==  NULL){
		printk(KERN_ALERT "Could not allocate space for data buffer\n");
		return -1;
	}

	//clear data buffer
	memset(device_buffer, '\0', BUFFER_SIZE);
	
	return 0;
}

static void pa2_char_driver_exit(void){
	printk("Exit function called\n");

	//freeing memory
	if(device_buffer != NULL){
		kfree(device_buffer);
	}

	unregister_chrdev(240, "pa2_char_device");

	cdev_del(&pa2_char_driver_cdev);
	
}

//binding functions to call module_init and module_exit
module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);
