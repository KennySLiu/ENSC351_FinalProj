#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

static int inline abs(int a, int b){
    return a < b ? a : b;
}

static int kenny_open(struct inode* inode_pointer, struct file* file_pointer) {
  printk(KERN_INFO "Poke open called.");
  return 0;
}


static int kenny_release(struct inode* inode_pointer, struct file* file_pointer) {
  printk(KERN_INFO "Poke release called.");
  return 0;
}


static ssize_t kenny_read(struct file *file, char *data, size_t length, loff_t *offset_in_file){
    printk(KERN_INFO "Poke read called.");
  return 0;
}


static ssize_t kenny_write(struct file *file, const char *data, size_t length, loff_t *offset_in_file){
    static int accumulated_bytes = 0;
    static long int held_ptr_value = 0;

    printk(KERN_INFO "Poke write called");

    /* curr_offset is the offset into the char* data. Each time we loop through the while, we will need 
     * to increment the curr_offset so we grab the newest data. */

    if (accumulated_bytes + length < 9){
        for (int i = 0; i < length; ++i){
            /* Leftshift and accumulate the ptr: */
            /* Multiply 8 by i + accumulated_bytes because (i + accumulated bytes) represents
             * the number of bytes we got so far, which is what we need. */
            held_ptr_value += (data[i] << 8*(i + accumulated_bytes));
        }
        accumulated_bytes += length;

        return 0;
    }
        
    int curr_offset = 0;
    size_t modified_length = length;
    while (modified_length + accumulated_bytes >= 9){ 
        modified_length -= (9 - accumulated_bytes);

        /* Get the pointer value: */
        long int ptr_val = held_ptr_value;
        for (int i = curr_offset; i < curr_offset + 8; ++i){
            /* Leftshift and accumulate the ptr: */
            ptr_val += (data[i] << 8*((i - curr_offset) + accumulated_bytes));
        }
        curr_offset += 8;

        /* Write to the pointer value: */
        char* to_write = (char*) ptr_val;
        *to_write = data[curr_offset++];

        accumulated_bytes = 0;
        held_ptr_value = 0;
    }

    for (int i = curr_offset; i < length; ++i){
        held_ptr_value += (data[i] << 8*(i - curr_offset));
    }
    accumulated_bytes = length - curr_offset;
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

   int major = register_chrdev(0, "Hello", &file_ops);   
   class_stuff = class_create(THIS_MODULE, "hello class");
   device_data = device_create(class_stuff, NULL, MKDEV(major, 0), NULL,
                   "kenny_poke");
  
  printk(KERN_INFO "KENNY_POKE: HI!\n");
  return 0;
}


static void __exit bye(void) {
  printk(KERN_INFO "KENNY_POKE: BYE!\n");
}

module_init(hi);
module_exit(bye);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RL");
MODULE_DESCRIPTION("Kenny's code: whomsting does a gheh. This is the poke driver!");