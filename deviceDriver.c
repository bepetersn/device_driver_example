
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


int chardev_test_open(struct inode *filep, struct file* f) 
{
    num_times_opened++;
    printk(KERN_NOTICE "Num times opened: %d\n", num_times_opened); 
    return 0;
}

int chardev_test_close(struct inode *filep , struct file *f)
{
    num_times_closed++;
    printk(KERN_NOTICE "Num times closed: %d\n", num_times_closed);
    return 0;
}

ssize_t chardev_test_write(struct file *f, 
                           const char __user *user_buf, 
                           size_t buf_size, loff_t *offset)
{

    int int_offset = (int) *offset;
    int bytes_available = BUFFER_SIZE - int_offset;
    char *buff_contents = buffer; // Get access to our buffer 
    // Pointer arithmetic lets us start writing at caller's offset
    char *offset_contents = buff_contents + int_offset; 

    if(int_offset > BUFFER_SIZE || int_offset < 0) {
        printk(KERN_ALERT "Invalid offset supplied\n");
        return -1;
    }
    if (bytes_available < buf_size) {
        printk(KERN_ALERT "Invalid length buffer\n"); 
        return -1;
    }
    // copy_from expects a (char *) to write into 
    if(!copy_from_user(offset_contents, user_buf, buf_size)) {
        printk(KERN_NOTICE "Wrote %li bytes: '%s'\n", buf_size, buff_contents);
        *offset += buf_size; // Allow multiple writes per open
        return buf_size; // Caller expects # bytes written
    } else {
        printk(KERN_ALERT "Failed to write");
        return -1;
    }
}

ssize_t chardev_test_read(struct file *f, 
                          char __user *user_buf, 
                          size_t buf_size, loff_t *offset)
{
    int int_offset = (int) *offset;
    int bytes_available = BUFFER_SIZE - int_offset;

    if(int_offset > BUFFER_SIZE || int_offset < 0) {
        printk(KERN_ALERT "Invalid offset supplied\n");
        return -1;
    }
    if (bytes_available < buf_size) {
        printk(KERN_ALERT "Invalid length buffer\n");
        return -1;
    }
    // copy_to expects buffer to be a (void *)
    if(!copy_to_user(user_buf, buffer, buf_size)) {
        printk(KERN_NOTICE "Read %li bytes\n", buf_size);
        *offset += buf_size; // Track what we have read so far
        return buf_size; // Caller expects # bytes read 
    } else {
        printk(KERN_NOTICE "Failed to read\n");
        return -1;
    } 
}

loff_t chardev_test_seek(struct file *filep, loff_t offset, int whence)
{
    loff_t requested;
    loff_t buf_size_loff = (loff_t) BUFFER_SIZE;
    switch(whence) {
      case 0: // SEEK_SET
        requested = offset;
        break;
      case 1: // SEEK_CUR -- incr / decr by offset 
        requested = filep->f_pos + offset;
        break;
      case 2: // SEEK_END -- negative offset from end
        requested = buf_size_loff - offset;
        break; 
      default:
        return -EINVAL;
    }
    // Seeking outside the buffer is not allowed
    if(requested > buf_size_loff || requested < 0) {
        printk(KERN_NOTICE "Invalid offset in seek: %lld\n", requested);
        return -1;
    }
    filep->f_pos = requested;
    printk(KERN_NOTICE "Seeked to %lld\n", requested);
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

