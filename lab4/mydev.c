#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#define DEVICE_NAME "mydev"
#define DEVICE_COUNT 1
#define MAJOR_NUM 256
#define MINOR_NUM 0
#define BUF_LEN 20

static struct my_device_data{
    struct cdev cdev;
    char *kernel_buffer;
};
static struct my_device_data *global_my_data;

dev_t dev_num = MKDEV(MAJOR_NUM, MINOR_NUM);

unsigned short seg_for_c[27] = {
    0b1111001100010001, // A
    0b0000011100000101, // b
    0b1100111100000000, // C
    0b0000011001000101, // d
    0b1000011100000001, // E
    0b1000001100000001, // F
    0b1001111100010000, // G
    0b0011001100010001, // H
    0b1100110001000100, // I
    0b1100010001000100, // J
    0b0000000001101100, // K
    0b0000111100000000, // L
    0b0011001110100000, // M
    0b0011001110001000, // N
    0b1111111100000000, // O
    0b1000001101000001, // P
    0b0111000001010000, // q
    0b1110001100011001, // R
    0b1101110100010001, // S
    0b1100000001000100, // T
    0b0011111100000000, // U
    0b0000001100100010, // V
    0b0011001100001010, // W
    0b0000000010101010, // X
    0b0000000010100100, // Y
    0b1100110000100010, // Z
    0b0000000000000000  // Null
};

static int device_open(struct inode *inode, struct file *file) {
    struct my_device_data *my_data;
    
    my_data = container_of(inode->i_cdev, struct my_device_data, cdev);

    file->private_data = my_data;
 
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset) {
    char input_char;
    unsigned short seg_value;
    ssize_t bytes_read;
    char seg_buffer[16]={0};
    int i;
    struct my_device_data *my_data;

    my_data = (struct my_device_data *)filp->private_data;
    
    if (*offset >= BUF_LEN) {
        *offset = 0;  // EOF
    }

    input_char = my_data->kernel_buffer[*offset];
    *offset += 1;

    // Convert the input character to uppercase
    if (input_char >= 'a' && input_char <= 'z') {
        input_char -= 32;  // Convert to uppercase
    }

    // Check if the character is within the valid range (A-Z)
    if (input_char < 'A' || input_char > 'Z') {
        input_char = 91;
    }

    // Get the corresponding segment code from seg_for_c
    seg_value = seg_for_c[input_char - 'A'];

    // Write each bit of seg_value into the kernel buffer
    for (i = 0; i < 16; i++) {
        seg_buffer[i] = (seg_value & (1 << (15 - i))) ? '1' : '0';
    }

    bytes_read = copy_to_user(buffer, seg_buffer, length);
    if (bytes_read != 0) {
        printk(KERN_ERR "Failed to send %zd characters to user\n", bytes_read);
        return -EFAULT;
    }

    printk(KERN_INFO "Sent %zu characters to the user\n", length);

    return length;    
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t * offset) {

    struct my_device_data *my_data;

    my_data = (struct my_device_data *)filp->private_data;

    // Only process 1 character at a time
    if (length < 1) {
        return -EINVAL;  // Invalid argument
    }

    if (length > BUF_LEN){
    	length = BUF_LEN;
    }

    // Copy the input from the user space (copy_from_user returns 0 on success)
    if (copy_from_user(my_data->kernel_buffer, buffer, length)) {
        return -EFAULT;  // Bad address
    }

    // Return the number of bytes written
    return length;

}

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

static int __init chardev_init(void) {

    // Allocate memory for my_device_data structure and initialize it to zero
    struct my_device_data *my_data = kzalloc(sizeof(struct my_device_data), GFP_KERNEL);
    global_my_data = my_data;
    if (!my_data) {
        pr_err("Failed to allocate memory for my_device_data\n");
        return -ENOMEM;
    }

    // Allocate memory for the kernel buffer with size BUFF_SIZE
    my_data->kernel_buffer = kzalloc(BUF_LEN, GFP_KERNEL);
    if (!my_data->kernel_buffer) {
        pr_err("Failed to allocate memory for kernel buffer\n");
        // Free previously allocated device_data_ptr in case of failure
        kfree(my_data);
        return -ENOMEM;
    }

    int ret;

    // Allocate device number using register_chrdev_region
    ret = register_chrdev_region(dev_num, DEVICE_COUNT, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to register character device region\n");
        kfree(my_data->kernel_buffer);
        kfree(my_data);
    }

    pr_info("Registered device with major number %d and minor number %d\n", MAJOR(dev_num), MINOR(dev_num));

    // 3. Initialize the cdev structure and associate it with file operations
    cdev_init(&my_data->cdev, &fops);
    my_data->cdev.owner = THIS_MODULE;

    // 4. Add the cdev to the system
    ret = cdev_add(&my_data->cdev, dev_num, 1);
    if (ret < 0) {
        pr_err("Failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        kfree(my_data->kernel_buffer);
        kfree(my_data);
        return ret;
    }

    pr_info("Driver initialized successfully\n");

    return 0;
}

static void __exit chardev_exit(void) {
    
    struct my_device_data *my_data = global_my_data;

    // Remove cdev and unregister device region
    cdev_del(&my_data->cdev);
    unregister_chrdev_region(dev_num, 1);

    // Free the kernel buffer first
    if (my_data->kernel_buffer) {
        kfree(my_data->kernel_buffer);
	pr_info("The kernel buffer in my_device_data has been release.");
    }
    
    // Then free the my_device_data structure
    if (my_data) {
        kfree(my_data);
	pr_info("The my_device_data struct instance has been release.");
    }

    pr_info("The character device has been successfully removed.");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
