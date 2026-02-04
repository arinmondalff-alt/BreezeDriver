#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <linux/types.h>

typedef struct {
    int pid;
    uint64_t addr;
    void* buffer;
    size_t size;
    int is_write;
} k_rw_request;

#define IOCTL_RW_MEM _IOWR('k', 1, k_rw_request)
#endif
