#include "factory.h"
#include "netlink.h"
#include "iface.h"
#include "debug.h"

typedef struct {
    int idx;
    int (*create) (void *ctx);
} creator_t;

static const creator_t creators[] = {IFACE_NETLINK, &netlink_create};

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
    if (idx > NUM_CREATORS-1 || idx < 0)
        return CREATE_NOT_FOUND;
    return creators[idx].create((void *) ctx);
}
