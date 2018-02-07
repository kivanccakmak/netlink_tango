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
#include <net/ethernet.h>
#include <arpa/inet.h>
 
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>

#include "factory.h"
#include "iface.h"
#include "utils.h"

//TODO: only depend on factory.h and iface.h
//TODO: add iface queue into main
//TODO: partion main into init() - run()
//TODO: add signal handler to main
int main(int argc, char *argv[])
{
    int i = 0;
    int num_ifaces = IFACE_LAST;
    for (i = 0; i < IFACE_LAST; i++) {
        int ret;
        struct iface *iface = NULL;
        ret = create_iface(i, iface);
        if (ret) {
            sfree(iface);
            continue;
        }
    }
    return 0;
}
