#include "factory.h"
#include "netlink.h"
#include "iface.h"
#include "debug.h"

#include <pthread.h>

typedef struct {
    int (*create) (void *ctx);
    void* (*init) (void *ctx);
} creator_t;

static creator_t creators[] = {
    [IFACE_NETLINK] = {.create = netlink_create, .init = &netlink_init},
};

#define NUM_CREATORS sizeof(creators)/sizeof(creator_t)

/**
 * @brief 
 *
 * @param[in] idx
 * @param[out] ctx
 *
 * @return 
 */
int create_iface(int idx, struct iface *ctx)
{
    int ret;
    if (idx > NUM_CREATORS-1 || idx < 0)
        return CREATE_NOT_FOUND;
    if (creators[idx].create((void *) ctx)) {
        return 1;
    }
    pthread_create(&ctx->id, NULL, creators[idx].init, (void *) ctx);
    return 0;
}
