#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <net/ethernet.h>
#include <net/if.h>

#include "myioctls.h"
#include "mydefs.h"
#include "handler.h"

static int netlink_link_state(struct sockaddr_nl *nl, struct nlmsghdr *msg);

int msg_handler(struct sockaddr_nl *nl, struct nlmsghdr *msg)
{
    switch (msg->nlmsg_type)
    {
        case RTM_NEWADDR:
            printf("msg_handler: RTM_NEWADDR\n");
            netlink_link_state(nl, msg);
            break;
        case RTM_DELADDR:
            printf("msg_handler: RTM_DELADDR\n");
            netlink_link_state(nl, msg);
            break;
        case RTM_NEWROUTE:
            printf("msg_handler: RTM_NEWROUTE\n");
            netlink_link_state(nl, msg);
            break;
        case RTM_DELROUTE:
            printf("msg_handler: RTM_DELROUTE\n");
            netlink_link_state(nl, msg);
            break;
        case RTM_NEWLINK:
            printf("msg_handler: RTM_NEWLINK\n");
            netlink_link_state(nl, msg);
            break;
        case RTM_DELLINK:
            printf("msg_handler: RTM_DELLINK\n");
            netlink_link_state(nl, msg);
            break;
        default:
            printf("msg_handler: Unknown netlink nlmsg_type %d\n",
                   msg->nlmsg_type);
            break;
    }
    return 0;
}

static int netlink_link_state(struct sockaddr_nl *nl, struct nlmsghdr *msg)
{
    int num_flags, ret;
    struct ifinfomsg *ifi;
    char mac[ETH_ALEN+1] = {0};
    char ifname[IFNAMSIZ+1] = {0};
    char ipaddr[MAX_IP_LEN+1] = {0};
    char flags[MAX_STR_LEN] = {0};

    ifi = NLMSG_DATA(msg);
    if_indextoname(ifi->ifi_index, &ifname[0]);

    printf("ifname: %s\n", ifname);

    ret = get_iface_mac((const char *) &ifname[0], mac);
    if (ret == 0) {
        fprintf(stdout, "mac: " MAC_STR  "\n", MAC_STR_ARGS(mac));
    }

    ret = get_iface_ip((const char *) &ifname[0], ipaddr);
    if (ret == 0) {
        fprintf(stdout, "ip: %s\n", ipaddr);
    }

    ret = get_iface_flags(ifname, flags, &num_flags);
    if (!ret) {
        fprintf(stdout, "flags: %s\n", flags);
    }
    fprintf(stdout, "\n");

    return 0;
}
