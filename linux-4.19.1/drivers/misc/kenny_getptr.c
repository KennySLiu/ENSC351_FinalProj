#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static int kenny_open(struct inode* inode_pointer, struct file* file_pointer) {
  printk(KERN_INFO "Get_ptr open called.");
  return 0;
}


static int kenny_release(struct inode* inode_pointer, struct file* file_pointer) {
  printk(KERN_INFO "Get_ptr release called.");
  return 0;
}


static ssize_t kenny_read(struct file *file, char *data, size_t length, loff_t *offset_in_file){
    void* retptr = NULL;
    char* retptr_ptr = NULL;


    printk(KERN_INFO "KENNY: Get_ptr read called");    

    /* Have to emit an 8-byte value (long int) pointing to a memory area of at least 1024 bytes */
    if (length != 8){
        printk(KERN_INFO "KENNY: Get_ptr call with length not equal to 8. Erroring out");
        return -EINVAL;
    }

    retptr = kmalloc(1024, GFP_USER);
    printk(KERN_INFO "KENNY DEBUG: Get_ptr &retptr value: %p", &retptr);

    retptr_ptr = (char*) &retptr;
    printk(KERN_INFO "KENNY DEBUG: Get_ptr retptr_ptr value: %p", retptr_ptr);

    /* Copy to pointer into the user-buffer */
    if (copy_to_user(data, retptr_ptr, 8) != 0){
        printk(KERN_INFO "KENNY: Get_ptr call couldn't copy data to user! Erroring out");
        return -EFAULT;
    }

    return 8;
}


static ssize_t kenny_write(struct file *file, const char *data, size_t length, loff_t *offset_in_file){
    printk(KERN_INFO "Get_ptr write called.");
    /* Do nothing. */
  return 0;
}


static struct device* device_data;
static struct class* class_stuff;

static struct file_operations file_ops =
{
   .open = kenny_open,
   .release = kenny_release,
   .read = kenny_read,
   .write = kenny_write,
};


static int __init hi(void) {

   int major = register_chrdev(0, "kenny_getptr_Hello", &file_ops);   
   class_stuff = class_create(THIS_MODULE, "kenny getptr hello");
   device_data = device_create(class_stuff, NULL, MKDEV(major, 0), NULL,
                   "kenny_getptr");
  
  printk(KERN_INFO "KENNY_GETPTR: HI!\n");
  return 0;
}


static void __exit bye(void) {
  printk(KERN_INFO "KENNY_GETPTR: BYE!\n");
}

module_init(hi);
module_exit(bye);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RL");
MODULE_DESCRIPTION("Kenny's code: whomsting does a gheh. This is the getptr driver!");
