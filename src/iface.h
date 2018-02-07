#ifndef _IFACE_H
#define _IFACE_H

struct iface {
    int fd;
    int (*init)(void *ctx);
    int (*handle)(void *ctx);
    int (*remove)(void *ctx);
};

enum {
    IFACE_NETLINK,
    IFACE_LAST
};

#endif
