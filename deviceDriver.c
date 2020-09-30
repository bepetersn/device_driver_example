
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
static void *buffer;

int chardev_test_open(struct inode *dev, struct file* f) 
{
    num_times_opened++;
    printk(KERN_NOTICE "Num times opened: %d\n", num_times_opened); 
    return 0;
}

int chardev_test_close(struct inode *dev , struct file *f)
{
    num_times_closed++;
    printk(KERN_NOTICE "Num times closed: %d\n", num_times_closed);
    return 0;
}

ssize_t chardev_test_write(struct file *f, 
                           const char __user *user_buf, 
                           size_t buf_size, loff_t *pos)
{
    // handle position differently here
    int result = copy_from_user(buffer, user_buf, BUFFER_SIZE);
    if(result != 0) {
        printk(KERN_NOTICE "Wrote %d bytes\n", BUFFER_SIZE);
        *pos += buf_size;
    } else {
        printk(KERN_ALERT "Failed to write");
    }
    return buf_size;
}

ssize_t chardev_test_read(struct file *f, 
                          char __user *user_buf, 
                          size_t buf_size, 
                          loff_t *pos)
{
    int bytes_read = 0;
    int result = copy_to_user(user_buf, buffer, BUFFER_SIZE);
    if(result != 0) {
        printk(KERN_NOTICE "Read %d bytes\n", BUFFER_SIZE);
        bytes_read = buf_size;
    } else {
        printk(KERN_ALERT "Failed to read");
    }

    *pos += buf_size;
   
    return bytes_read;
}

loff_t chardev_test_seek(struct file *dev, loff_t offset, int whence)
{
    // - Deal with whence
    // - subtract or add seek if appropriate
    // - or otherwise, set to offset
    // - Deal with out-of-buffer issues
    return 0;
}


int driver_init(void)
{
    static struct file_operations fops = {
	.owner   = THIS_MODULE,
	.open    = chardev_test_open, 
	.release = chardev_test_close,
        .write   = chardev_test_write,
        .read    = chardev_test_read
    };
    buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if(buffer == NULL) {
        printk(KERN_ALERT "Buffer not allocated");
        return -1;
    }

    register_chrdev(CHARDEV_TEST_MAJOR_NUMBER, "chardev_test", &fops);
    printk(KERN_NOTICE "inside %s function\n", __FUNCTION__);
    return 0;
}



void driver_exit(void)
{
    unregister_chrdev(CHARDEV_TEST_MAJOR_NUMBER, "chardev_test");
    kfree(buffer);
    printk(KERN_NOTICE "inside %s function\n", __FUNCTION__);
}

module_init(driver_init);
module_exit(driver_exit);

