#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static char peek_location[8];

static int kenny_open(struct inode* inode_pointer, struct file* file_pointer) {
  printk(KERN_INFO "Peek open called.");
  return 0;
}


static int kenny_release(struct inode* inode_pointer, struct file* file_pointer) {
  printk(KERN_INFO "Peek release called.");
  return 0;
}


static ssize_t kenny_read(struct file *file, char *data, size_t length, loff_t *offset_in_file){
    char* ptr_val = NULL;

    printk(KERN_INFO "KENNY: Peek read called");

    if (length != 1){
        printk(KERN_INFO "KENNY DEBUG: Peek read issued with a length not equal to 1. Erroring out.");
        return -EFAULT;
    }

    memcpy(&ptr_val, peek_location, 1);

    printk(KERN_INFO "KENNY DEBUG: Peek read calculated the peek pointer to be = %p.", ptr_val);

    if (copy_to_user(data, ptr_val, 1) != 0){
        printk(KERN_INFO "KENNY DEBUG: Peek read had an issue transferring the data back to user! Erroring out.");
        return -EFAULT;
    }

  return 0;
}


static ssize_t kenny_write(struct file *file, const char *data, size_t length, loff_t *offset_in_file){
    /* For this function, we assume the users will write exactly 8 bytes every time. 
     * The reason for this is as follows: Suppose the user sends 3 bytes, and then 8 bytes. 
     * Which bytes do we overwrite? Do we assume the latter 8 bytes represent the whole pointer? 
     * Do we assume the first five of the 8 bytes are actually the LSBs of the resultant pointer, 
     * while the remaining 3 now become the MSBs? It's too ill-defined. I will dictate that,
     * for my driver, users have to send exactly the 8 bytes they want to read from. */

    printk(KERN_INFO "KENNY: Peek write called.");

    if (copy_from_user(peek_location, data, 8) != 0){
        printk(KERN_INFO "KENNY: Peek write had an issue with copying over the user's data! Erroring.");
        return -EFAULT;
    }

  return 8;
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

   int major = register_chrdev(0, "kenny_peek_Hello", &file_ops);   
   class_stuff = class_create(THIS_MODULE, "kenny peek class");
   device_data = device_create(class_stuff, NULL, MKDEV(major, 0), NULL,
                   "kenny_peek");
  
  printk(KERN_INFO "KENNY_PEEK: HI!\n");
  return 0;
}


static void __exit bye(void) {
  printk(KERN_INFO "KENNY_PEEK: BYE!\n");
}

module_init(hi);
module_exit(bye);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RL");
MODULE_DESCRIPTION("Kenny's code: whomsting does a gheh. This is the peek driver!");
