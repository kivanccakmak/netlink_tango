#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
 
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>

#include "mydefs.h"
#include "myioctls.h"

static int open_netlink()
{
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    struct sockaddr_nl addr;

    memset((void *)&addr, 0, sizeof(addr));

    if (fd < 0) {
        perror("socket() ");
        return -1;
    }

    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = RTMGRP_LINK|RTMGRP_IPV4_IFADDR;
    if (bind(fd,(struct sockaddr *)&addr,sizeof(addr))<0) {
        perror("bind() ");
        return -1;
    }
    return fd;
}
 

static int read_event(int fd, int (*msg_handler)(struct sockaddr_nl *,
                                               struct nlmsghdr *))
{
    int status;
    int ret = 0;
    char buf[4096];
    struct iovec iov = { buf, sizeof buf };
    struct sockaddr_nl snl;
    struct msghdr msg = { (void*)&snl, sizeof snl, &iov, 1, NULL, 0, 0};
    struct nlmsghdr *h;
    
    status = recvmsg(fd, &msg, 0);

    if(status < 0) {
        /* Socket non-blocking so bail out once we have read everything */
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return ret;

        /* Anything else is an error */
        printf("read_netlink: Error recvmsg: %d\n", status);
        perror("read_netlink: ");
        return status;
    }
        
    if(status == 0) {
        printf("read_netlink: EOF\n");
    }

    /* We need to handle more than one message per 'recvmsg' */
    for(h = (struct nlmsghdr *) buf; NLMSG_OK (h, (unsigned int)status); h = NLMSG_NEXT (h, status)) {
        /* Finish reading */
        if (h->nlmsg_type == NLMSG_DONE)
            return ret;

        /* Message is some kind of error */
        if (h->nlmsg_type == NLMSG_ERROR) {
            printf("read_netlink: Message is an error - decode TBD\n");
            return -1; // Error
        }

        /* Call message handler */
        if(msg_handler) {
            ret = (*msg_handler)(&snl, h);
            if(ret < 0) {
                printf("read_netlink: Message hander error %d\n", ret);
                return ret;
            }
        } else {
            printf("read_netlink: Error NULL message handler\n");
            return -1;
        }
    }

    return ret;
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

static int msg_handler(struct sockaddr_nl *nl, struct nlmsghdr *msg)
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


int main(int argc, char *argv[])
{
    int nls = open_netlink();

    printf("Started watching:\n");
    if (nls < 0) {
        printf("Open Error!");
    }

    while (1) {
        read_event(nls, msg_handler);
    }
    return 0;
}
