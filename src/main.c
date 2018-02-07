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

#include "mydefs.h"
#include "myioctls.h"
#include "handler.h"

//TODO: move read_event to handler (or create one more layer)
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
    for (h = (struct nlmsghdr *) buf; NLMSG_OK (h, (unsigned int)status); h = NLMSG_NEXT (h, status)) {
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

int main(int argc, char *argv[])
{
    int sock = open_socket();

    printf("Started watching:\n");
    if (sock < 0) {
        printf("Open Error!");
    }

	while (1) {
		read_event(sock, msg_handler);
	}
    return 0;
}
