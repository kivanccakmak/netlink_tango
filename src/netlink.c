#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>

#include <net/ethernet.h>
#include <net/if.h>

#include "iface.h"
#include "utils.h"
#include "debug.h"
#include "eventq.h"

static int netlink_handle(void *ctx);
static int event_handler(void *ctx);
static int print_link_state(struct sockaddr_nl *nl, struct nlmsghdr *msg);
static int netlink_remove(void *ctx);

struct event_ctx {
    struct sockaddr_nl *nl;
    struct nlmsghdr *msg;
};

/**
 * @brief 
 *
 * @param[out] ctx
 *
 * @return 0 on succ, 1 on error
 */
int netlink_create(void *ctx)
{
    int fd;
    struct sockaddr_nl addr;
    struct iface *iface = NULL;

    iface = (struct iface*) ctx;
    memset((void *)&addr, 0, sizeof(addr));

    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        perror("socket() ");
        return 1;
    }

    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = RTMGRP_LINK|RTMGRP_IPV4_IFADDR;
    if (bind(fd,(struct sockaddr *)&addr,sizeof(addr))<0) {
        perror("bind() ");
        return 1;
    }
    iface->fd = fd;
    iface->handle = &netlink_handle;
    iface->remove = &netlink_remove;
    return 0;
}

void *netlink_init(void *ctx)
{
    while (1) {
        netlink_handle(ctx);
    }
    return NULL;
}

static int netlink_handle(void *ctx)
{
    int fd;
    int status;
    int ret = 0;
    char buf[4096];
    struct iovec iov = { buf, sizeof buf };
    struct sockaddr_nl snl;
    struct msghdr msg = { (void*)&snl, sizeof snl, &iov, 1, NULL, 0, 0};
    struct nlmsghdr *h;
    struct iface *iface = NULL;
    struct event *e = NULL;
    struct event_ctx *event_ctx = NULL;

    iface = (struct iface*) ctx;
    fd = iface->fd;

    status = recvmsg(fd, &msg, 0);

    if(status < 0) {
        /* Socket non-blocking so bail out once we have read everything */
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return ret;

        /* Anything else is an error */
        warningf("read_netlink: Error recvmsg: %d", status);
        perror("read_netlink: ");
        return status;
    }
        
    if(status == 0) {
        debugf("read_netlink: EOF");
    }

    /* We need to handle more than one message per 'recvmsg' */
    for (h = (struct nlmsghdr *) buf; NLMSG_OK (h, (unsigned int)status); h = NLMSG_NEXT (h, status)) {
        /* Finish reading */
        if (h->nlmsg_type == NLMSG_DONE)
            return ret;

        /* Message is some kind of error */
        if (h->nlmsg_type == NLMSG_ERROR) {
            warningf("read_netlink: Message is an error - decode TBD");
            return -1; // Error
        }

        event_ctx = (struct event_ctx*) malloc(sizeof(struct event_ctx));
        event_ctx->msg = h;
        event_ctx->nl = &snl;

        e = (struct event*) malloc(sizeof(struct event));
        e->ctx = (void *) event_ctx;
        e->handle = &event_handler;
        push_event(e);
        if(ret < 0) {
            infof("read_netlink: Message hander error %d", ret);
            return ret;
        }
    }

    return ret;
}

static int event_handler(void *ctx)
{
    struct event_ctx *event_ctx = NULL;
    struct sockaddr_nl *nl = NULL;
    struct nlmsghdr *msg = NULL;

    event_ctx = (struct event_ctx*) ctx;
    nl = event_ctx->nl;
    msg = event_ctx->msg;

    switch (msg->nlmsg_type)
    {
        case RTM_NEWADDR:
            infof("RTM_NEWADDR");
            print_link_state(nl, msg);
            break;
        case RTM_DELADDR:
            infof("RTM_DELADDR");
            print_link_state(nl, msg);
            break;
        case RTM_NEWROUTE:
            infof("RTM_NEWROUTE");
            print_link_state(nl, msg);
            break;
        case RTM_DELROUTE:
            infof("RTM_DELROUTE");
            print_link_state(nl, msg);
            break;
        case RTM_NEWLINK:
            infof("RTM_NEWLINK");
            print_link_state(nl, msg);
            break;
        case RTM_DELLINK:
            infof("RTM_DELLINK");
            print_link_state(nl, msg);
            break;
        default:
            warningf("msg_handler: Unknown netlink nlmsg_type %d\n",
                   msg->nlmsg_type);
            break;
    }

    return 0;
}

static int print_link_state(struct sockaddr_nl *nl, struct nlmsghdr *msg)
{
    int num_flags, ret;
    struct ifinfomsg *ifi;
    char mac[ETH_ALEN+1] = {0};
    char ifname[IFNAMSIZ+1] = {0};
    char ipaddr[INET_ADDRSTRLEN] = {0};
    char flags[4096] = {0};

    ifi = NLMSG_DATA(msg);
    if_indextoname(ifi->ifi_index, &ifname[0]);

    infof("ifname: %s\n", ifname);

    ret = get_iface_mac((const char *) &ifname[0], mac);
    if (ret == 0) {
        infof("mac: " MAC_STR, MAC_STR_ARGS(mac));
    }

    ret = get_iface_ip((const char *) &ifname[0], ipaddr);
    if (ret == 0) {
        infof("ip: %s", ipaddr);
    }

    ret = get_iface_flags(ifname, flags, &num_flags);
    if (!ret) {
        infof("flags: %s", flags);
    }

    return 0;
}

static int netlink_remove(void *ctx)
{
    return 0;
}
