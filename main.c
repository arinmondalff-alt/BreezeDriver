#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/pid.h>
#include "protocol.h"

#define DEVICE_NAME "breeze_rw"
static int major_number = -1; // Default -1

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    if (cmd == IOCTL_RW_MEM) {
        k_rw_request req;
        struct task_struct *task;
        if (copy_from_user(&req, (void __user *)arg, sizeof(req))) return -EFAULT;
        task = pid_task(find_vpid(req.pid), PIDTYPE_PID);
        if (!task) return -ESRCH;
        return (long)access_process_vm(task, req.addr, req.buffer, req.size, req.is_write);
    }
    return -EINVAL;
}

static struct file_operations fops = {
    .unlocked_ioctl = my_ioctl,
    .owner = THIS_MODULE,
};

static int __init driver_entry(void) {
    // 0 matlab kernel khud ek free number dega
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    
    if (major_number < 0) {
        printk(KERN_ALERT "Breeze Driver: Registration failed with %d\n", major_number);
        return major_number; // Agar error hai toh driver load nahi hoga (Safe)
    }
    
    printk(KERN_INFO "Breeze Driver Loaded! Major Number: %d\n", major_number);
    return 0;
}

static void __exit driver_exit(void) {
    if (major_number >= 0) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_INFO "Breeze Driver Unloaded\n");
    }
}

module_init(driver_entry);
module_exit(driver_exit);
MODULE_LICENSE("GPL");
