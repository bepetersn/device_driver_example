
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Brian Peterson");
MODULE_LICENSE("GPL");

#define CHARDEV_TEST_MAJOR_NUMBER 261
#define BUFFER_SIZE 1024

int num_times_opened;
int num_times_closed;

int chardev_test_open(struct inode *dev, struct file* f) 
{
    // printk(KERN_ALERT "opened me"); 
    //num_times_opened++;
    return 0;
}

int chardev_test_close(struct inode *dev , struct file *f)
{
    //num_times_closed++;
    //printk(KERN_ALERT "Num times opened: %d", num_times_closed);
    return 0;
}

int driver_init(void)
{

    static struct file_operations fops = {
	.owner   = THIS_MODULE,
	.open    = chardev_test_open, 
	.release = chardev_test_close, 
    };
    register_chrdev(CHARDEV_TEST_MAJOR_NUMBER, "chardev_test", &fops);
    printk(KERN_ALERT "inside %s function\n", __FUNCTION__);
    return 0;
}



void driver_exit(void)
{
    unregister_chrdev(CHARDEV_TEST_MAJOR_NUMBER, "chardev_test");
    // kfree
    printk(KERN_ALERT "inside %s function\n", __FUNCTION__);
}

module_init(driver_init);
module_exit(driver_exit);

