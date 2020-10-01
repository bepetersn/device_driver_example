
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
int buff_content_endpos = 0;


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

    char *buff_contents = buffer;
    size_t bytes_written = 0; 
    int num_failed_bytes;

    if(*offset > BUFFER_SIZE) {
        return -EFAULT;
    }

    num_failed_bytes = copy_from_user(buff_contents, user_buf, BUFFER_SIZE);
    if(num_failed_bytes == 0) {
        bytes_written = strlen(buff_contents);
        printk(KERN_NOTICE "Wrote %li bytes: %s\n", bytes_written, buff_contents);
        *offset += bytes_written;
        buff_content_endpos += bytes_written;
    } else {
        printk(KERN_ALERT "Failed to write %d bytes: %s\n", num_failed_bytes, buff_contents);
    }
    return bytes_written;
}

ssize_t chardev_test_read(struct file *f, 
                          char __user *user_buf, 
                          size_t buf_size, loff_t *offset)
{
    int num_bytes_remaining;
    size_t bytes_read = 0;
    int num_failed_bytes;     
    char *buff_contents = buffer;

    // NOTE: user_buf's buf_size > BUFFER_SIZE in practice.
    // If this was not so, we would need to worry about
    // about consecutive calls to this function, but 
    // as it is, we can just give everything to user_buf
    // in one go every time.

    // Take the end of the buffer content, subtract
    // our offset, and if anything remains, we can 
    // give exactly that.
    num_bytes_remaining = buff_content_endpos - ((int) *offset);
    if(num_bytes_remaining <= 0) {
        return 0;
    }
    printk(KERN_WARNING "Buff content endpos: %d, Num bytes remaining: %d\n", buff_content_endpos, num_bytes_remaining);
    num_failed_bytes = copy_to_user(user_buf, buffer, num_bytes_remaining);
    if(num_failed_bytes == 0) {
        bytes_read = strlen(buff_contents);
        printk(KERN_NOTICE "Read %li bytes\n", bytes_read);
        printk(KERN_NOTICE "Contents: %s\n", buff_contents);
        *offset += bytes_read;
        return bytes_read;
    } else {
        printk(KERN_ALERT "Failed to read %d bytes\n", num_failed_bytes);
        return -1;
    }
   
    return bytes_read;
}

loff_t chardev_test_seek(struct file *dev, loff_t offset, int whence)
{
    // - Deal with whence
    // - subtract or add seek if appropriate
    // - or otherwise, set to offset
    // - Deal with out-of-buffer issues
    printk(KERN_NOTICE "In seek");
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
        printk(KERN_ALERT "Buffer not allocated\n");
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

