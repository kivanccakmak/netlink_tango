#include "factory.h"
#include "netlink.h"
#include "iface.h"

/**
 * @brief 
 *
 * @param[in] idx
 * @param[out] ctx
 *
 * @return 
 */
int create_iface(int idx, void *ctx)
{
    int ret = CREATE_NOT_FOUND;
    switch (idx) {
        case IFACE_NETLINK:
            if (netlink_create(ctx)) {
                ret = CREATE_FAIL;
            } else {
                ret = CREATE_SUCCESS;
            }
            break;
        default:
            break;
    }
    return ret;
}
