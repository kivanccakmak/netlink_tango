#ifndef _IFACE_FACTORY
#define _IFACE_FACTORY

#define CREATE_SUCCESS 0
#define CREATE_FAIL 1
#define CREATE_NOT_FOUND 2

int create_iface(int idx, void *ctx);

#endif
