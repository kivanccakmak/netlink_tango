#ifndef _IFACE_H
#define _IFACE_H

#include <sys/queue.h>
#include <pthread.h>

struct iface {
    TAILQ_ENTRY(iface) tailq;
    pthread_t id;
    int fd;
    void* (*init)(void *ctx);
    int (*handle)(void *ctx);
    int (*remove)(void *ctx);
};

enum {
    IFACE_NETLINK,
    IFACE_LAST
};

#endif
