#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <net/ethernet.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

#include <netpacket/packet.h>
#include <arpa/inet.h>

#include <errno.h>

#include "mydefs.h"


#define ADD_SIOC_FLAG(flags, type, str, num_flags, flags_out) \
    do { \
        if (flags & type) { \
            flags_out += sprintf(flags_out, str); \
            *num_flags += 1; \
        } \
    } while(0); \

int get_iface_ip(const char *ifname, char *ip_out)
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

int get_iface_mac(const char *ifname, char *mac_out)
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
int get_iface_flags(const char *ifname, char *flags_out, int *num_flags)
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

    return 0;
}
