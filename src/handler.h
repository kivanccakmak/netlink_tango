#ifndef _HANDLER_H
#define _HANDLER_H

int open_socket();

int msg_handler(struct sockaddr_nl *nl, struct nlmsghdr *msg);

#endif
