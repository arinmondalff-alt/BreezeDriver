#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/pid.h>
#include <linux/miscdevice.h>
#include "protocol.h"

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

static struct miscdevice breeze_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "breeze_rw",
    .fops = &fops,
};

static int __init driver_entry(void) {
    int ret;
    
    // Debug 1: Init start
    printk(KERN_INFO "Breeze_Debug: Starting driver_entry\n");

    ret = misc_register(&breeze_misc);
    
    if (ret < 0) {
        // Debug 2: Fail
        printk(KERN_ERR "Breeze_Debug: misc_register failed with %d\n", ret);
        return ret;
    }

    // Debug 3: Success
    printk(KERN_INFO "Breeze_Debug: Driver Loaded! /dev/breeze_rw created.\n");
    return 0;
}

static void __exit driver_exit(void) {
    misc_deregister(&breeze_misc);
    printk(KERN_INFO "Breeze_Debug: Driver Unloaded\n");
}

module_init(driver_entry);
module_exit(driver_exit);
MODULE_LICENSE("GPL");
