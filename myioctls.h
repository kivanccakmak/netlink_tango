#ifndef _MY_IOCTLS_H
#define _MY_IOCTLS_H

int get_iface_ip(const char *ifname, char *ip_out);
int get_iface_mac(const char *ifname, char *mac_out);
int get_iface_flags(const char *ifname, char *flags_out, int *num_flags);

#endif
