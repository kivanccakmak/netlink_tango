#include <asm/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
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
 
#define MAC_STR "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"
#define MAC_STR_ARGS(m) m[0], m[1], m[2], m[3], m[4], m[5]

#define MAX_STR_LEN 4096
#define MAX_IP_LEN 16

#define ADD_SIOC_FLAG(flags, type, str, num_flags, flags_out) \
    do { \
        if (flags & type) { \
            flags_out += sprintf(flags_out, str); \
            *num_flags += 1; \
        } \
    } while(0); \

static int get_iface_ip(const char *ifname, char *ip_out)
{
    int ret, fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket() ");
        return -1;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);

    ret = ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    if (ret < 0) {
        perror("ioctl(SIOCGIFADDR) ");
        return -1;
    }

    snprintf(ip_out, MAX_IP_LEN, "%s", inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));
    return 0;
}

static int get_iface_mac(const char *ifname, char *mac_out)
{
    struct ifreq ifr;
    int ret, fd;

    fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket() ");
        return -1;
    }

    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);

    ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
    close(fd);

    if (ret < 0) {
        perror("ioctl(SIOCGIFHWADDR) ");
        return -2;
    }

    memcpy(mac_out, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
    return ret;
}
 
int open_netlink()
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
 

int read_event(int fd, int (*msg_handler)(struct sockaddr_nl *,
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

/**
 * @brief get active flag words of device.
 *
 * @param[in] ifname
 * @param[out] flags_out
 * @param[out] num_flags
 *
 * @return -1 socket fail
 *         -2 ioctl fail
 *          0 succ
 *
 */
static int get_iface_flags(const char *ifname, char *flags_out, int *num_flags)
{
    int ret, fd;
    struct ifreq ifr;
    short flags;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket() ");
        return -1;
    }

    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);

    ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
    if (ret < 0) {
        perror("ioctl(SIOCGIFFLAGS) ");
        return -1;
    }

    flags = ifr.ifr_flags;

    ADD_SIOC_FLAG(flags, IFF_UP, "[IFF_UP]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_BROADCAST, "[IFF_BROADCAST]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_DEBUG, "[IFF_DEBUG]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_LOOPBACK, "[IFF_LOOPBACK]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_POINTOPOINT, "[IFF_POINTTOPOINT]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_RUNNING, "[IFF_RUNNING]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_NOARP, "[IFF_NOARP]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_PROMISC, "[IFF_PROMISC]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_NOTRAILERS, "[IFF_NOTRAILERS]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_ALLMULTI, "[IFF_ALLMULTI]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_MASTER, "[IFF_MASTER]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_SLAVE, "[IFF_SLAVE]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_MULTICAST, "[IFF_MULTICAST]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_PORTSEL, "[IFF_PORTSEL]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_AUTOMEDIA, "[IFF_AUTOMEDIA]", num_flags, flags_out);
    ADD_SIOC_FLAG(flags, IFF_DYNAMIC, "[IFF_DYNAMIC]", num_flags, flags_out);
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

    ret = get_iface_mac((const char *) &ifname[0], &mac[0]);
    if (ret == 0) {
        fprintf(stdout, "mac: " MAC_STR  "\n", MAC_STR_ARGS(mac));
    }

    ret = get_iface_ip((const char *) &ifname[0], &ipaddr[0]);
    if (ret == 0) {
        fprintf(stdout, "ip: %s\n", ipaddr);
    }

    ret = get_iface_flags(ifname, flags, &num_flags);
    if (!ret) {
        fprintf(stdout, "flags: %s\n", flags);
    }

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
