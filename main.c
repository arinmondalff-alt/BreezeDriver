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

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    if (cmd == IOCTL_RW_MEM) {
        k_rw_request req;
        struct task_struct *task; // Declaration ko upar kar diya

        if (copy_from_user(&req, (void __user *)arg, sizeof(req))) {
            return -EFAULT;
        }

        task = pid_task(find_vpid(req.pid), PIDTYPE_PID); // Ab kaam shuru
        if (!task) {
            return -ESRCH;
        }

        return (long)access_process_vm(task, req.addr, req.buffer, req.size, req.is_write);
    }
    return -EINVAL;
}

static struct file_operations fops = {
    .unlocked_ioctl = my_ioctl,
    .owner = THIS_MODULE,
};

static int __init driver_entry(void) {
    register_chrdev(100, DEVICE_NAME, &fops);
    printk(KERN_INFO "Breeze Driver Loaded\n");
    return 0;
}

static void __exit driver_exit(void) {
    unregister_chrdev(100, DEVICE_NAME);
}

module_init(driver_entry);
module_exit(driver_exit);
MODULE_LICENSE("GPL");
