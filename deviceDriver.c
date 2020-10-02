
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Brian Peterson");
MODULE_LICENSE("GPL");

#define MAJOR_NUMBER 261
#define BUFFER_SIZE 1024
#define DRIVER_NAME "chardev_test"

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
                           size_t buf_size, loff_t *offset)
{

    // TODO: buf -> buff everywhere
    // Point a string at our (void *) buffer, to begin working with it
    char *buff_contents = buffer;
    int success;
    int int_offset = (int) *offset;

    // Don't allow specifying an invalid offset into the buffer
    if(int_offset < 0 || int_offset >= BUFFER_SIZE)                          
    {
        printk(KERN_ALERT "Invalid offset supplied to read\n");
        return -EINVAL;
    }

    // copy_from_user takes args: to, from, n
    // TODO: Remove newline from end
    success = copy_from_user(buff_contents, user_buf, buf_size);
    if(success == 0) {
        printk(KERN_NOTICE "Wrote %li bytes: '%s'\n", buf_size, buff_contents);
        
        // Increment the offset by how much was written to our file 
        *offset += buf_size;
    } else {
        printk(KERN_ALERT "Failed to write");
        return -1;
    }
    // Caller expects this to be how much we wrote
    return buf_size;
}

ssize_t chardev_test_read(struct file *f, 
                          char __user *user_buf, 
                          size_t buf_size, loff_t *offset)
{
    ssize_t bytes_available = 0;
    int success;
    char *buff_contents = buffer;

    // Don't allow specifying an invalid offset into the buffer
    if(*offset < 0 || *offset >= BUFFER_SIZE) 
    {
        printk(KERN_ALERT "Invalid offset supplied to read\n");
        return -EINVAL;
    }

    // Stop reading bytes when the offset reaches the total # of non-NULL bytes
    bytes_available = strlen(buff_contents);
    if(*offset >= bytes_available) {
        return 0;
    }

    // copy_to_user takes args: to, from, n
    success = copy_to_user(user_buf, buffer, bytes_available);
    if(success != 0) 
    {
        printk(KERN_NOTICE "Failed to read\n");
        return -1;
    }
    printk(KERN_NOTICE "Read %li bytes\n", bytes_available);

    // Increment offset to track what we have read so far
    *offset += bytes_available;
    return bytes_available;
}

loff_t chardev_test_seek(struct file *dev, loff_t offset, int whence)
{
    // - Deal with whence
    // - subtract or add seek if appropriate
    // - or otherwise, set to offset
    // - Deal with out-of-buffer issues
    printk(KERN_NOTICE "In seek\n");
    // HINT: maybe use dev->f_pos;
    return 0;
}


int driver_init(void)
{
    static struct file_operations fops = {
	.owner   = THIS_MODULE,
	.open    = chardev_test_open, 
	.release = chardev_test_close,
        .write   = chardev_test_write,
        .read    = chardev_test_read,
        .llseek  = chardev_test_seek
    };
    buffer = kzalloc(BUFFER_SIZE, GFP_KERNEL);
    if(buffer == NULL) {
        printk(KERN_ALERT "Buffer not allocated\n");
        return -1;
    }

    register_chrdev(MAJOR_NUMBER, DRIVER_NAME, &fops);
    printk(KERN_NOTICE "inside %s function\n", __FUNCTION__);
    return 0;
}

void driver_exit(void)
{
    unregister_chrdev(MAJOR_NUMBER, DRIVER_NAME);
    kfree(buffer);
    printk(KERN_NOTICE "inside %s function\n", __FUNCTION__);
}

module_init(driver_init);
module_exit(driver_exit);

