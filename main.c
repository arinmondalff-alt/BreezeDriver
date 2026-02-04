#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/pid.h>
#include <linux/miscdevice.h> // Naya header
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

// Misc Device Structure
static struct miscdevice breeze_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "breeze_rw",
    .fops = &fops,
};

static int __init driver_entry(void) {
    int ret;
    ret = misc_register(&breeze_misc);
    if (ret) {
        printk(KERN_ERR "Breeze Driver: Registration failed!\n");
        return ret;
    }
    printk(KERN_INFO "Breeze Driver Loaded! Misc device registered.\n");
    return 0;
}

static void __exit driver_exit(void) {
    misc_deregister(&breeze_misc);
    printk(KERN_INFO "Breeze Driver Unloaded\n");
}

module_init(driver_entry);
module_exit(driver_exit);
MODULE_LICENSE("GPL");
