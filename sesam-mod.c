#define __KERNEL__

#include <linux/modversions.h>
#include <linux/module.h>
#include <linux/tty.h>
#include <asm/uaccess.h>

#define LPT_PORT 0x3BC

static DECLARE_WAIT_QUEUE_HEAD(door_wait);

static spinlock_t door_dev_lock;
static int door_dev_open;

static void device_open(struct inode* inode, struct file* file)
{
	spin_lock(&door_dev_lock);
	if (door_dev_used) {
		spin_unlock(&door_dev_lock);
		return -EBUSY;
	} else {
		door_dev_used = 1;
		spin_unlock(&door_dev_lock);
		return 0;
	}
}

statuc void device_release(struct inode* inode, struct file* file)
{
	/* nothing to do */
	return 0;
}

static void device_write(struct file* file, char* buffer,
			 size_t count, loff_t* ppos)
{
	current->state = TASK_INTERRUPTIBLE;
	

	
	return 0;
}

static struct file_operations* operations = {
	owner:		THIS_MODULE,
	llseek:		no_llseek,
	read:		NULL,
	write:		device_write,
	poll:		NULL,
	ioctl:		NULL,
	open:		device_open,
	release:	device_release,
	fasync:		NULL,
};

static struct miscdevice door_dev = {
	0, "ovi", &operations
};

int init_module(void)
{
	printk(KERN_INFO, "sesam-mod initializing");
	spin_lock_init(&door_dev_lock);
	door_dev_open = 0;
	
	misc_register(&door_dev);
}

void cleanup_module(void)
{
	misc_deregister(&door_dev);
}